#include "dsbase.h"
#include "dsioformat.h"
#include "dstruct.h"
#include "dsconfig.h"

using namespace dstruct;

cDSConfig::cDSConfig()
{
  dstructNode = NULL;
}

cDSConfig::~cDSConfig() {}

bool
cDSConfig::loadConfig(const std::string& configFile)
{
  string extendName;
  dstruct::iDStructIOFormat* ioFromat = NULL;
  string configContent;
  bool isload;
  
  if (configFile == "") {
    cerr << "没有配置文件名称" << endl;
    return false;
  }

  // 根据扩展名创建io对象
  if (configFile.length() <= 3) {
    cerr << "文件名错误" << endl;
    return false;
  }
  extendName = configFile.substr(configFile.length() - 3, 3);
  if (extendName == ".ds" || extendName == ".DS")
    ioFromat = new dstruct::cDSIOTextFormat();
  extendName = configFile.substr(configFile.length() - 4, 4);
  if (extendName == ".xml" || extendName == ".XML")
    ioFromat = new dstruct::cDSIOXmlFormat();
  if (ioFromat == NULL) {
    cerr << "不支持的扩展名" << endl;
    return false;
  }

  // 载入配置文件
  if (readConfigFile(configFile, configContent) == false) {
    cerr << "读配置文件错误" << endl;
    return false;
  }
    
  ioFromat->reset();
  ioFromat->setDStructStream(configContent);
  isload = dstruct.inDStruct(ioFromat);
  delete ioFromat;
  return isload;
}

void
cDSConfig::getPathNameList(const std::string& path, std::vector<string>& pathNameList)
{
  dstruct::cDStructNode::mapPoolIter pathIter;
  dstruct::cDStructNode *node = dstructNode->getNode(path);
  for (pathIter = node->mapPoolBegin();
       pathIter != node->mapPoolEnd();
       pathIter++) {
    
    pathNameList.push_back(pathIter->first);
  }
}

int
cDSConfig::getArraySize(const std::string& path)
{
  dstruct::cDStructNode *node;
  node = dstructNode->getNode(path);
  if (node)
    return node->arraySize();
  return -1;
}

std::string
cDSConfig::getArrayStrByIdx(const std::string& path, int idx)
{
  dstruct::cDStructNode *node;
  dstruct::cDStructNode *arrayNode;
  node = dstructNode->getNode(path);
  if (node == NULL)
    return std::string("");
  arrayNode = node->getArrayNode(idx);
  if (node == NULL)
    return std::string("");
  return arrayNode->getValue();
}

int
cDSConfig::getArrayIntByIdx(const std::string& path, int idx)
{
  std::string valueStr = getArrayStrByIdx(path, idx);
  if (valueStr == "")
    return 0;
  int value;
  value = atoi(valueStr.c_str());
  return value;
}

bool
cDSConfig::getArrayBoolByIdx(const std::string& path, int idx)
{
  std::string valueStr = getArrayStrByIdx(path, idx);
  if (valueStr == "")
    return 0;
  std::transform(valueStr.begin(), valueStr.end(), valueStr.begin(), ::toupper);
  if (valueStr == "TRUE" || valueStr == "YES")
    return true;
  return false;
}

double
cDSConfig::getArrayDoubleByIdx(const std::string& path, int idx)
{
  float value;
  std::string valueStr = getArrayStrByIdx(path, idx);
  if (valueStr == "")
    return 0;
  sscanf(valueStr.c_str(), "%f", &value);
  return (double)value;
}

void
cDSConfig::getArrayValue(const std::string& path, std::vector<std::string>& valueList)
{
  dstruct::cDStructNode *node;
  node = dstructNode->getNode(path);
  if (node)
    node->getArrayValue(valueList);
}

std::string
cDSConfig::getValueStr()
{
  return dstructNode->getValue();
}


std::string
cDSConfig::getValueStr(const std::string& path)
{
  dstruct::cDStructNode *node;
  if (dstructNode == NULL)
    return dstruct(path);
  
  node = dstructNode->getNode(path);
  if (!node)
    return "";
  return node->getValue();
}

int
cDSConfig::getValueInt(const std::string& path)
{
  int value;
  std::string valueStr;
  
  valueStr = getValueStr(path);
  if (valueStr == "")
    return 0;
  value = atoi(valueStr.c_str());
  return value;
}

bool
cDSConfig::getValueBool(const std::string& path)
{
  std::string valueStr;
  
  valueStr = getValueStr(path);
  std::transform(valueStr.begin(), valueStr.end(), valueStr.begin(), ::toupper);
  if (valueStr == "TRUE" || valueStr == "YES")
    return true;
  return false; 
}

double
cDSConfig::getValueDouble(const std::string& path)
{
  float value;
  std::string valueStr;
  
  valueStr = getValueStr(path);
  if (valueStr == "")
    return 0.0f;
  sscanf(valueStr.c_str(), "%f", &value);
  return (double)value;
}

void
cDSConfig::setPathName(const std::string& name)
{
    dstructNode = dstruct[name];
}

void
cDSConfig::downPathName(const std::string& name)
{
  nodeStack.push_back(dstructNode);
  if (dstructNode == NULL)
    dstructNode = dstruct[name];
  else
    dstructNode = dstructNode->getNode(name);
}

void
cDSConfig::downPathName(const std::string& name, int idx)
{
  dstruct::cDStructNode *node;
  nodeStack.push_back(dstructNode);
  if (dstructNode == NULL) {
    node = dstruct.getNode(name);
    dstructNode = node->getArrayNode(idx);
  } else {
    node = dstructNode->getNode(name);
    dstructNode = node->getArrayNode(idx);
  }
}

void
cDSConfig::upPathName()
{
  dstructNode = nodeStack.back();
  nodeStack.pop_back();
}

bool
cDSConfig::readConfigFile(const std::string& configFile, std::string& text)
{
  std::ifstream iFile(configFile.c_str(), std::ios::in);
  text = "";
  if (!iFile.is_open())
 	{
      return false;
    }
  std::string strTemp;
  while (!iFile.eof())
    {
      std::getline(iFile,strTemp);
      text += strTemp;
      text += "\n";
	}
  iFile.close();
  return true;
}

