#include "dsbase.h"
#include "dsioformat.h"
#include "dstruct.h"

using namespace dstruct;

cDSPathNode::cDSPathNode(){};
cDSPathNode::~cDSPathNode(){};

inline void 
cDSPathNode::clear()
{
  mType = unknownode;
  mKey = "";
  mIndex = 0;
}

inline void
cDSPathNode::setType(DSPathNodeType type)
{
  mType = type;
}

inline DSPathNodeType
cDSPathNode::getType()
{
  return mType;
}

inline void
cDSPathNode::setKey(const string& key)
{
  mKey = key;
}

inline string&
cDSPathNode::getKey()
{
  return mKey;
}

inline void
cDSPathNode::setIndex(int index)
{
  mIndex = index;
}

inline int
cDSPathNode::getIndex()
{
  return mIndex;
}

cDSPathParse::cDSPathParse()
{
  reset();
}
 
cDSPathParse::~cDSPathParse()
{
}

inline void
cDSPathParse::setPath(const string& path)
{
  mPath = path;
  mPoint = mPath.c_str();
}

inline void
cDSPathParse::reset()
{
  mPath = "";
  mPoint = NULL;
}

bool
cDSPathParse::nextPathToken(cDSPathNode& pathNode)
{
  string key = "";
  string indexStr = "";
  int index = 0;
  DSPathNodeType type = unknownode;
  int stop = 1;
  
  pathNode.clear();
  // ANSI byte MBCS
  while(*mPoint != '\0' && stop == 1)
    {
      // multibyte
      if ((unsigned char)*mPoint > 127)
        {
          if (type == unknownode)
            type = mapnode;
          else if (type == arraynode)
            cout << "数组索引中不能包含双字节字符" << endl;
          key += *mPoint;
          mPoint++;
          key += *mPoint;
        }
      else
        {
          /*
           *
           * xx[1]
           * xx.yy
           * xx[1][2]
           * xx.yy.xx
           * [3].xx
           *
           */
          switch (*mPoint)
            {
            case '[':
              if (type == unknownode)
                {
                  type = arraynode;
                }
              else
                {
                  stop = 0;
                  mPoint--;
                }
              break;
            case ']':
              index = atoi(indexStr.c_str());
              stop = 0;
              break;
            case '.':
              if (type == unknownode)
                {
                  type = mapnode;
                }
              else
                {
                  stop = 0;
                  mPoint--;
                }
              break;
            case '=':
              stop = 0;
              mPoint--;
              break;
            case ' ':
              break;
            default:
              {
                if (type == unknownode)
                  {
                    type = mapnode;
                    key += *mPoint;
                  }
                else if (type == arraynode)
                  {    
                    indexStr += *mPoint;
                  }
                else if (type == mapnode)
                  {
                    key += *mPoint;
                  }
                else
                  {
                    cout << "unknow type (" << type << endl;
                    assert( 0 );
                  }
                
              }
            }
        }
      mPoint++;
    }
  pathNode.setType(type);
  pathNode.setKey(key);
  pathNode.setIndex(index);
  if (type != unknownode)
    return true;
  else
    return false;
}

///////////////////////////////////////////////////////

cDStructNode::cDStructNode()
{
  mMapPool = NULL;
  mArrayPool = NULL;
  mValue = "";
  nodeClear();
}
cDStructNode::~cDStructNode()
{
  nodeClear();
}

cDStructNode::cDStructNode(const cDStructNode& dstruct)
{
  this->mValue = dstruct.mValue;
  this->mMapPool = dstruct.copyMapPool();
  this->mArrayPool = dstruct.copyArrayPool();
  this->mNodePath = dstruct.mNodePath;
}

cDStructNode::cDStructNode(const cDStructNode* dstruct)
{
  this->mValue = dstruct->mValue;
  this->mMapPool = dstruct->copyMapPool();
  this->mArrayPool = dstruct->copyArrayPool();
  this->mNodePath = dstruct->mNodePath;
}

cDStructNode::mapPoolPoint
cDStructNode::copyMapPool() const
{
  mapPoolPoint mapPoint;
  mapPoolIter mapIter;
  cDStructNode* node;
  
  if (mMapPool == NULL)
    return NULL;
  
  mapPoint = new mapPoolDef;
  if (mapPoint == NULL)
    {
      memoryout();
      return NULL;
    }
  for(mapIter = mMapPool->begin();
      mapIter != mMapPool->end();
      mapIter++)
    {
      node = new cDStructNode(mapIter->second);
      if (node == NULL)
        {
          memoryout();
          return NULL;
        }
      (*mapPoint)[mapIter->first] = node;
    }
  return mapPoint;
}

cDStructNode::arrayPoolPoint
cDStructNode::copyArrayPool() const
{
  arrayPoolPoint arrayPoint;
  arrayPoolIter arrayIter;
  cDStructNode* node;
  
  if (mArrayPool == NULL)
    return NULL;
  
  arrayPoint = new arrayPoolDef;
  if (arrayPoint == NULL)
    {
      memoryout();
      return NULL;
    }
  for(arrayIter = mArrayPool->begin();
      arrayIter != mArrayPool->end();
      arrayIter++)
    {
      if (*arrayIter != NULL)
        { 
          node = new cDStructNode(*arrayIter);
          if (node == NULL)
            {
              memoryout();
              return NULL;
            }
        }
      else
        {
          node = NULL;
        }
      arrayPoint->push_back(node);
    }
  return arrayPoint;
}

void
cDStructNode::nodeClear()
{
  mValue = "";
  mNodePath = "";
  clearMapPool();
  clearArrayPool();  
}

bool
cDStructNode::isArray()
{
  if (mArrayPool)
    return true;
  return false;
}

bool
cDStructNode::isMap()
{
  if (mMapPool)
    return true;
  return false;
}

int
cDStructNode::arraySize()
{
  if (mArrayPool)
    return mArrayPool->size();
  return -1;
}

int
cDStructNode::mapSize()
{
  if (mMapPool)
    return mMapPool->size();
  return -1;
}

string
cDStructNode::getValue()
{
  return mValue;
}

void
cDStructNode::getArrayValue(vector<string>& values)
{
  arrayPoolIter iter;
  string value;
  if (mArrayPool == NULL)
    return;
  for (iter = arrayPoolBegin();
       iter != arrayPoolEnd();
       iter++) {
    if (*iter == NULL)
      value = "";
    else
      value = (*iter)->getValue();
    values.push_back(value);
  }
}

cDStructNode* 
cDStructNode::getMapNode(const string& key)
{
  mapPoolIter mapIter;
  if (mMapPool == NULL)
    return NULL;
  mapIter = mMapPool->find(key);
  if (mapIter == mMapPool->end())
    return NULL;
  return mapIter->second;
}

cDStructNode*
cDStructNode::getArrayNode(unsigned int index)
{
  cDStructNode* node;
  if (mArrayPool == NULL)
    return NULL;
  if (index >= mArrayPool->size())
    return NULL;
  node = mArrayPool->at(index);
  return node;
}

inline void
cDStructNode::setValue(const string& value)
{
  mValue = value;
}

inline void
cDStructNode::addMapNode(const string& key, cDStructNode* node)
{
  if (mMapPool == NULL)
    mMapPool = new mapPoolDef;
  std::string path = getNodePath();
  if (path == "") {
    path += key;
  } else {
    path += "." + key;
  }
  node->setNodePath(path);
  mMapPool->insert(pair<string, cDStructNode*>(key, node));
}


inline void
cDStructNode::addArrayNode(unsigned int index, cDStructNode* node)
{
  char buf[25];
  if (mArrayPool == NULL)
    mArrayPool = new arrayPoolDef;
  if (index >= mArrayPool->size())
    mArrayPool->resize(index+1);
  sprintf(buf, "%d", index);
  std::string path = getNodePath() + "[" + buf + "]";
  node->setNodePath(path);
  (*mArrayPool)[index] = node;
}

cDStructNode::mapPoolIter
cDStructNode::mapPoolBegin()
{
  if (mMapPool == NULL)
    return (mapPoolIter)NULL;
  return mMapPool->begin();
}

cDStructNode::mapPoolIter
cDStructNode::mapPoolEnd()
{
  if (mMapPool == NULL)
    return (mapPoolIter)NULL;
  return mMapPool->end();
}

cDStructNode::arrayPoolIter
cDStructNode::arrayPoolBegin()
{
  if (mArrayPool == NULL)
    return (arrayPoolIter)NULL;
  return mArrayPool->begin();
}

cDStructNode::arrayPoolIter
cDStructNode::arrayPoolEnd()
{
  if (mArrayPool == NULL)
    return (arrayPoolIter)NULL;
  return mArrayPool->end();
}

void
cDStructNode::clearMapPool()
{
  mapPoolIter mapIter;
  
  if (mMapPool != NULL)
    {    
      for(mapIter = mMapPool->begin();
          mapIter != mMapPool->end();
          mapIter++)
        {
          delete mapIter->second;
        }
      delete mMapPool;
      mMapPool = NULL;
    }
}

void
cDStructNode::clearArrayPool()
{
  arrayPoolIter arrayIter;
  
  if (mArrayPool != NULL)
    {
      for(arrayIter = mArrayPool->begin();
          arrayIter != mArrayPool->end();
          arrayIter++)
        {
          delete *arrayIter;
        }
      delete mArrayPool;
      mArrayPool = NULL;
    }
}

void
cDStructNode::set(const string& path, const string& value)
{
  cDSPathNode pathNode;
  cDSPathParse pathParse;
  cDStructNode* structNode = this;
  
  pathParse.reset();
  pathParse.setPath(path);
  
  while ( pathParse.nextPathToken(pathNode) == true )
    structNode = structNode->insert(pathNode);
  structNode->setValue(value);   
}

cDStructNode*
cDStructNode::getNode(const string& path)
{
  cDSPathNode pathNode;
  cDSPathParse pathParse;
  cDStructNode* structNode = this;
  string value = "";
  
  pathParse.reset();
  pathParse.setPath(path);
  
  while ( pathParse.nextPathToken(pathNode) == true )
    {
      structNode = structNode->find(pathNode);
      if (structNode == NULL)
        return NULL;
    }
  return structNode;
        
}

cDStructNode*
cDStructNode::insert(cDSPathNode& node)
{
  cDStructNode* structNode;

  // find old path
  structNode = find(node);
  if (structNode != NULL)
    return structNode;
  // insert new path
  structNode = new cDStructNode;
  if (structNode == NULL)
    {
      memoryout();
      return NULL;
    }
  if (node.getType() == mapnode)
    {
      addMapNode(node.getKey(), structNode);
    }
  else if (node.getType() == arraynode)
    {
      addArrayNode(node.getIndex(), structNode);
    }
  return structNode;
}

cDStructNode*
cDStructNode::find(cDSPathNode& node)
{
  cDStructNode* structNode = NULL;
        
  if (node.getType() == mapnode)
    {
      structNode = getMapNode(node.getKey());
    }
  else if (node.getType() == arraynode)
    {
      structNode = getArrayNode(node.getIndex());
    }
  return structNode;        
}

std::string&
cDStructNode::getNodePath()
{
  return mNodePath;
}

void
cDStructNode::setNodePath(std::string& path)
{
  mNodePath = path;
}

///////////////////////////////////////////////////////////

cDStruct::cDStruct() {};
cDStruct::cDStruct(cDStructNode* structNode) : mDSnode(structNode) {};
cDStruct::cDStruct(iDStructIOFormat* inFormat)
{
  inDStruct(inFormat);
};

cDStruct::~cDStruct() 
{
  mDSnode.nodeClear();
};


void
cDStruct::clear()
{
  mDSnode.nodeClear();
}

void
cDStruct::set(const string& path, const string& value)
{
  return mDSnode.set(path, value);
}

cDStructNode*
cDStruct::getRootNode()
{
  return &mDSnode;
}

string
cDStruct::get(const string& path)
{
  cDStructNode* node;
  
  node = mDSnode.getNode(path);
  if (node == NULL)
    return "";
  return node->getValue();
}

void
cDStruct::getArray(const string& path, vector<string>& values)
{
  cDStructNode *node;
  node = mDSnode.getNode(path);
  if (node == NULL)
    return ;
  return node->getArrayValue(values);
}


cDStructNode*
cDStruct::getNode(const string& path)
{
  return mDSnode.getNode(path);
}

void
cDStruct::outDStruct(iDStructIOFormat* outFormat)
{
  assert( outFormat != NULL );
  
  outFormat->startLevel();
  traipsePath(&mDSnode, outFormat);
  outFormat->endLevel();
}

bool
cDStruct::inDStruct(iDStructIOFormat* inFormat)
{
  assert( inFormat != NULL );
  string path;
  string value;
  int r;
  
  while( (r = inFormat->inPathValue(path, value)) == 1 )
      set(path, value);
  if (r == 0)
    return false;
  return true;
}

void
cDStruct::traipsePath(cDStructNode* node, iDStructIOFormat* outFormat)
{
  cDStructNode::mapPoolIter mapIter;
  cDStructNode::arrayPoolIter arrIter;
  int index = 0;

  outFormat->enterLevel();
  outFormat->outValue(node->getValue());
  for(mapIter = node->mapPoolBegin();
      mapIter != node->mapPoolEnd();
      mapIter++)
    {
      outFormat->outHashKey(mapIter->first);
      traipsePath(mapIter->second, outFormat);
    }
  for(arrIter = node->arrayPoolBegin();
      arrIter != node->arrayPoolEnd();
      arrIter++)
    {
      index++;
      if (*arrIter == NULL)
        continue;
      outFormat->outArrayIndex(index-1);
      traipsePath(*arrIter, outFormat);
    }
  outFormat->exitLevel();
}

void
cDStruct::addend(cDStruct& dstruct)
{
  cDSIOTextFormat textFormat;
  string textStream;
  
  dstruct.outDStruct((iDStructIOFormat*)&textFormat);
  textStream = textFormat.getDStructStream();
  inDStruct((iDStructIOFormat*)&textFormat);
}

cDStruct*
cDStruct::operator+(cDStruct& dstruct)
{
  cDStruct* newDstruct;
  
  newDstruct = new cDStruct(&mDSnode);
  newDstruct->addend(dstruct);
  return newDstruct;
}

void
cDStruct::operator+=(cDStruct& dstruct)
{
  addend(dstruct);
}

cDStructNode* 
cDStruct::operator[](const string& path)
{
  return mDSnode.getNode(path);  
}


string
cDStruct::operator()(const string& path)
{
  return get(path);
}
