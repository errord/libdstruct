#include "dsbase.h"
#include "dsioformat.h"
#include "dstruct.h"

using namespace dstruct;

void
iDStructIOFormat::reset()
{
  mDSStream = "";
  mStreamPos = 0;
  mLinenum = 1;
  mPath.clear();
}

void
iDStructIOFormat::setDStructStream(const string& dstream)
{
  mDSStream = dstream;
}

string&
iDStructIOFormat::getDStructStream()
{
  return mDSStream;        
}

bool
iDStructIOFormat::isMultibyte(char c)
{
  if ((unsigned char)c > 127)
    return true;
  return false;
}

void
iDStructIOFormat::multibyte(string& mb)
{
  mb = "";
  mb += mDSStream[mStreamPos];
  mStreamPos++;
  mb += mDSStream[mStreamPos];
}

void
iDStructIOFormat::addlineNum()
{
  mLinenum++;
}

string
iDStructIOFormat::outLineNum(int offset = 0)
{
  char buff[1024];
  sprintf(buff, "%d", mLinenum + offset);
  string m =  "第";
  m += buff;
  m += "行,";
  return m;
}

void
iDStructIOFormat::makePath(string& path)
{
  vector<string>::iterator vectIter;
  path = "";
  for(vectIter = mPath.begin();
      vectIter != mPath.end();
      vectIter++)
    {
      path += *vectIter;
    }
}

void
cDSIOTextFormat::enterLevel()
{
}

void
cDSIOTextFormat::exitLevel()
{
  if (mPath.size() > 0)
    mPath.pop_back();
}

void
cDSIOTextFormat::outHashKey(const string& key)
{
  string path;
  if (mPath.size() == 0)
    path = key;
  else
    path = "." + key;
  mPath.push_back(path);
}

void
cDSIOTextFormat::outArrayIndex(int index)
{
  char buff[1024];
  sprintf(buff, "%d", index);
  string path = "[";
  path += buff;
  path += "]";
  mPath.push_back(path);
}

void
cDSIOTextFormat::outValue(const string& value)
{
  string path;
  
  if (value == "")
    return;
  makePath(path);
  path += " = ";
  path += "\"";
  path += value;
  path += "\";";
  path += "\n";
  mDSStream += path;
}

void
cDSIOTextFormat::reset()
{
  iDStructIOFormat::reset();
}

int
cDSIOTextFormat::inPathValue(string& path, string& value)
{
  enum linestate
    {
      estart,
      epath,
      eequal,
      evalue,
      ecomm,
      esemicolonprefix,
      esemicolonpostfix,
      eexit
    };
  string mb;
  bool ismb;
  linestate state = estart;
  if (mStreamPos >= mDSStream.length())
    return 2;
  char c;
  path = "";
  value = "";
  while(state != eexit)
    {
      if (mStreamPos >= mDSStream.length())
        {
          if (state != estart && state != ecomm)
            {    
              cerr << outLineNum() << "没有结束符(;)" << endl;
              return 0;
            }
          return 2;
        }
      c = mDSStream[mStreamPos];
      if (c == '\r' && mDSStream[mStreamPos+1] == '\n')
        {
          mStreamPos++;
          continue;
        }
      ismb = isMultibyte(c);
      if (ismb == true)
        multibyte(mb);
      else if (c == '\r' || c == '\n')
        addlineNum();

      switch ( state )
        {
        case estart:
          if (ismb == true)
              state = epath;
          else if (c == '#')
              state = ecomm;
          else if (c == '\r' || c == '\n')
            state = estart;
          else if (c == ' ' || c == '\t')
            state = estart;
          else
              state = epath;
          break;

        case ecomm:
          if (ismb == false && 
              c == '\n')
            state = estart;
          break;
          
        case epath:
          if (ismb == false &&
              c == '=')
            state = eequal;
          break;
          
        case eequal:
          if (ismb == false &&
              c == '\"')
            {    
              state = esemicolonprefix;
            }
          else if ( ! (ismb == false &&
                       (c == ' ' || c == '\t')))
            {
              cerr << outLineNum() << "等号右边只能是字符串" << endl;
              return 0;
            }
          break;
          
        case evalue:
          if (isMultibyte(c) == false &&
              c == '\"')
            state = esemicolonpostfix;
          break;
          
        case esemicolonprefix:
          if (isMultibyte(c) == false &&
              c == '\"')
              state = esemicolonpostfix;
          else
            state = evalue;
          break;

        case esemicolonpostfix:
          if (c == ';')
            {
              state = eexit;
            }
          else
            {
              cerr << outLineNum(-1) << "错误的结束符号" << endl;
              return 0;
            }
        }
      if (ismb == true)
        {
          if (state == epath)
            path += mb;
          else if (state == evalue)
            value += mb;
        }
      else
        {    
          if (state == epath)
            path += c;
          else if (state == evalue)
            value += c;
        }
      mStreamPos++;      
    }
  return 1;
}

////////////////////////////////////////////

cXmlNode::cXmlNode()
{
  reset();
};

cXmlNode::~cXmlNode(){};

inline void
cXmlNode::reset()
{
  mNodeType = unknow;
  mTagName = "";
  mTagAttrMap.clear();
}

inline void
cXmlNode::setNodeType(xmlNodeType type)
{
  mNodeType = type; 
}

inline void
cXmlNode::setTagName(string& name)
{
  mTagName = name;
}

inline void
cXmlNode::setAttr(string& name, string& value)
{
  mTagAttrMap[name] = value; 
}

inline xmlNodeType
cXmlNode::getNodeType()
{
  return mNodeType; 
}

inline string&
cXmlNode::getTagName()
{
  return mTagName; 
}

inline string
cXmlNode::getAttr(string& name)
{
  return mTagAttrMap[name]; 
}


inline bool
cXmlNode::isAttrExist(string& name)
{
  tagAttrMapIter mapIter;
  mapIter = mTagAttrMap.find(name);
  if (mapIter == mTagAttrMap.end())
    return false;
  return true;
}

////////////////////////////////////////////

cDSIOXmlFormat::cDSIOXmlFormat()
{
  reset();
};
cDSIOXmlFormat::~cDSIOXmlFormat() {};

void
cDSIOXmlFormat::reset()
{
  iDStructIOFormat::reset();
  mXmlHead = "<?xml version=\"1.0\" encoding=\"GB2312\" ?>";
  mDStructItem = "DStruct";
  mItemLevel = 0;
  mStreamPoint = NULL;
  mFirstParse = true;
}

void
cDSIOXmlFormat::startLevel()
{
  mDSStream = mXmlHead;
  mDSStream += "\n\n";
  mDSStream += "<";
  mDSStream += mDStructItem;
  mDSStream += ">\n";
}

void
cDSIOXmlFormat::endLevel()
{
  mDSStream += "</";
  mDSStream += mDStructItem;
  mDSStream += ">\n";
}

void
cDSIOXmlFormat::makeItem(const string& key, const string& type)
{
  mDSStream += makeIndent();
  mDSStream += "<Item name=\"";
  mDSStream += key;
  mDSStream += "\" type=\"";
  mDSStream += type;
  mDSStream += "\">\n";
}

string
cDSIOXmlFormat::makeIndent()
{
  int i;
  string indent = "";
  
  for(i = 0; i < mItemLevel; i++)
    {
      indent += "\t";
    }
  return indent;
}

void
cDSIOXmlFormat::enterLevel()
{
  mItemLevel++;
}

void
cDSIOXmlFormat::exitLevel()
{
  if (mItemLevel > 1)
    {
      mItemLevel--;
      mDSStream += makeIndent();
      mDSStream += "</Item>\n";
    }
}

void
cDSIOXmlFormat::outHashKey(const string& key)
{
  const string type = "key";
  makeItem(key, type);
}

void
cDSIOXmlFormat::outArrayIndex(int index)
{
  string type = "index";
  char buff[1024];
  string indexStr;
  sprintf(buff, "%d", index);
  indexStr = buff;
  makeItem(indexStr, type);
}

void
cDSIOXmlFormat::outValue(const string& value)
{
  if (value == "")
    return;
  mDSStream += makeIndent();
  mDSStream += "<Value>";
  mDSStream += value;
  mDSStream += "</Value>\n";
}

int
cDSIOXmlFormat::inPathValue(string& path, string& value)
{
  cXmlNode xmlNode;
  string tagName = "";
  path = "";
  value = "";
  
  if (mFirstParse == true)
    { 
      nextXmlNode(xmlNode);
      if (xmlNode.getNodeType() != declarationnode)
        {
          cerr << "错误XML声明节点" << endl;
          return 0;
        }
      nextXmlNode(xmlNode);
      if ( ! ((xmlNode.getNodeType() == node) &&
              (xmlNode.getTagName() == "DStruct")) )
        {
          cerr << "XML文件格式错误" << endl;
          return 0;
        }
      mFirstParse = false;
    }

  string name = "name";
  string type = "type";
  while (1)
    {
      if (nextXmlNode(xmlNode) == false)
        {
          cerr << outLineNum() << "无法获取标签" << endl;
          return 0;
        }
      tagName = xmlNode.getTagName();
      if (xmlNode.getNodeType() == endnode &&
          tagName == "DStruct")
        {
          assert( mPath.size() == 0 );
          return 2;
        }

      if (tagName == "Item")
        {
          if (xmlNode.getNodeType() == node)
            {
              string nameValue;
              string typeValue;
              string pathNode;
              
              nameValue = xmlNode.getAttr(name);
              typeValue = xmlNode.getAttr(type);
              if (typeValue == "key")
                {
                  if (mPath.size() == 0)
                    pathNode = nameValue;
                  else
                    pathNode = "." + nameValue;
                  mPath.push_back(pathNode);
                }
              else if (typeValue == "index")
                {
                  pathNode = "[";
                  pathNode += nameValue;
                  pathNode += "]";
                  mPath.push_back(pathNode);
                }
              else
                {
                  cerr << outLineNum() << "无法识别的属性:" << typeValue << endl;
                  return 0;
                }
            }
          else
            {
              assert( mPath.size() > 0 );
              mPath.pop_back();
            }
          
        }
      else if (tagName == "Value")
        {
          xmlValue(value);
          makePath(path);
          nextXmlNode(xmlNode);
          if (xmlNode.getNodeType() == endnode &&
              xmlNode.getTagName() == "Value")
            return 1;
          cerr << outLineNum() << "没有以</Value>标签" << endl;
          return 0;
        }
      else
        {
          cerr << outLineNum() << "无法识别的XML标签:" << tagName  << endl;
          return 0;
        }
    }
  return 0;
}

bool
cDSIOXmlFormat::nextXmlNode(cXmlNode& xmlNode)
{
  string mb;
  bool ismb;
  enum xmlstate
    {
      estart,
      etags,
      etage,
      etag,
      etagspace,
      etagattrname,
      etagattrequal,
      etagattrvalue,
      etagattrvaluee,
      evalue,
      ecomm
    };
  xmlstate state = estart;
  xmlstate laststate = estart;
  char c;
  string tagName = "";
  string tagAttrName = "";
  string tagAttrValue = "";
  xmlNode.reset();
  
  while(state != etage)
    {
      if (mStreamPos >= mDSStream.length())
        {
          if (state != estart)
            cerr << outLineNum() << "错误的结束" << endl;
          return false;
        }
      c = mDSStream[mStreamPos];
      ismb = isMultibyte(c);
      if (ismb == true)
        {
          if (state != etag && state != etagattrname &&
              state != etagattrvalue && state != evalue &&
              state != ecomm)
            {
              cerr << outLineNum() << "不能识别的双字节字符" << endl;
              return false;
            }
          multibyte(mb);
          c = '\0';
        }
      else if (c == '\n')
        {
          addlineNum();
        }

      mStreamPos++;
      
      switch( state )
        {
        case estart:
          if (c == '<')
            state = etags;
          else
            continue;
          break;
        case etags:
          if (c == '?')
            {
              xmlNode.setNodeType(declarationnode);
            }
          else if (c == '/')
            {    
              xmlNode.setNodeType(endnode);
            }
          else
            {    
              xmlNode.setNodeType(node);
              mStreamPos--;
            }
          state = etag;
          break;
        case etag:
          if (c == '>')
            {
              laststate = etag;
              state = etage;
              xmlNode.setTagName(tagName);
            }
          else if (c == ' ' || c == '\t')
            {
              laststate = etag;
              state = etagspace;
              xmlNode.setTagName(tagName);
            }
          else
            {
              if (c == '\0')
                tagName += mb;
              else
                tagName += c;
            }
          break;
        case etagattrname:
          if (c == '>')
            {
              cerr << outLineNum() << "没有=号及属性值" << endl;
              return false;
            }
          else if (c == '=')
            {
              laststate = etagattrname;
              state = etagattrequal;
              if (xmlNode.isAttrExist(tagAttrName) == true)
                {
                  cerr << outLineNum() << "重复的属性" << endl;
                  return false;
                }
            }
          else if (c == ' ' || c == '\t')
            {
              laststate = etagattrname;
              state = etagspace;
              if (xmlNode.isAttrExist(tagAttrName) == true)
                {
                  cerr << outLineNum() << "重复的属性" << endl;
                  return false;
                }              
            }
          else
            {
              if (c == '\0')
                tagAttrName += mb;
              else
                tagAttrName += c;
            }
          break;

        case etagattrequal:
          if (c == '>')
            {
              cerr << outLineNum() << "没有属性值" << endl;
              return false;
            }
          else if (c == ' ')
            {
              state = etagspace;
            }
          else if (c == '"')
            {
              state = etagattrvalue;
            }
          else
            {
              cerr << outLineNum() << "属性赋值时,此处只能出现做引号" << endl;
              return false;
            }
          break;

        case etagattrvalue:
          if (c == '"')
            {
              state = etagattrvaluee;
              xmlNode.setAttr(tagAttrName, tagAttrValue);
              tagAttrName = "";
              tagAttrValue = "";
            }
          else
            {
              if (c == '\0')
                tagAttrValue += mb;
              else
                tagAttrValue += c;
            }
          break;
        case etagattrvaluee:
          if (c == '>')
            {
              state = etage;
            }
          else if (c == ' ' || c == '\t')
            {
              laststate = etagattrvaluee;
              state = etagspace;
            }
          else
            {
              cerr << outLineNum() << "属性需要使用空格分隔" << endl;
              return false;
            }
          break;
        case etagspace:
          if (c == ' ' || c == '\t')
            continue;
          
          switch ( laststate )
            {
            case etag:
              if (c == '>')
                {
                  state = etage;
                  continue;
                }
              state = etagattrname;
              mStreamPos--;
              break;
            case etagattrname:
              if (c == '=')
                {
                  state = etagattrequal;
                }
              else
                {
                  cerr << outLineNum() << "属性后面需要有\"=\"号" << endl;
                  return false;
                }
              break;
            case etagattrequal:
              if (c == '"')
                {    
                  state = etagattrvalue;
                }
              else
                {
                  cerr << outLineNum() << "属性值需要从引号开始" << endl;
                  return false;
                }
              break;
            case etagattrvaluee:
              if (c == '?' && xmlNode.getNodeType() == declarationnode)
                {
                }
              else if (c == '>')
                {
                  state = etage;
                }
              else
                {
                  state = etagattrname;
                  mStreamPos--;
                }
              break;
            }
          break; 
        }
    }
  return true;
}

void
cDSIOXmlFormat::xmlValue(string& value)
{
  int state = 1;
  char c;
  string mb;
  bool ismb;
  value = "";
  if (mStreamPos >= mDSStream.length())
      return;
  while ( state )
    {
      c = mDSStream[mStreamPos];
      ismb = isMultibyte(c);
      if (ismb == true)
        {
          multibyte(mb);
          value += mb;
        }
      else if (c == '<')
        {
          return;
        }
      else
        {
          value += c;
        }
      mStreamPos++;
    }
}

