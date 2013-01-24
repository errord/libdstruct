#include "dsbase.h"
#include "dsioformat.h"
#include "dstruct.h"
#include "dsconfig.h"

using namespace dstruct;

dstruct::cDStruct* cDSConfig::dstruct = NULL;
std::map<std::string, cDSConfig*> cDSConfig::config_obj_map;

static bool readConfigFile(const std::string& configFile, std::string& text)
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

cDSConfig::cDSConfig(dstruct::cDStructNode *dsnode) :
  dstructNode(dsnode)
{
}

cDSConfig::~cDSConfig() {}

cDSConfig*
cDSConfig::loadConfig(const std::string& configFile)
{
  string extendName;
  dstruct::iDStructIOFormat* ioFromat = NULL;
  string configContent;
  bool isload;
 
  assert(!cDSConfig::dstruct);
 
  if (configFile == "") {
    cerr << "没有配置文件名称" << endl;
    return NULL;
  }

  // 根据扩展名创建io对象
  if (configFile.length() <= 3) {
    cerr << "文件名错误" << endl;
    return NULL;
  }
  extendName = configFile.substr(configFile.length() - 3, 3);
  if (extendName == ".ds" || extendName == ".DS")
    ioFromat = new dstruct::cDSIOTextFormat();
  extendName = configFile.substr(configFile.length() - 4, 4);
  if (extendName == ".xml" || extendName == ".XML")
    ioFromat = new dstruct::cDSIOXmlFormat();
  if (ioFromat == NULL) {
    cerr << "不支持的扩展名" << endl;
    return NULL;
  }

  // 载入配置文件
  if (readConfigFile(configFile, configContent) == false) {
    cerr << "读配置文件错误" << endl;
    return NULL;
  }
  
  dstruct = new dstruct::cDStruct();
  
  ioFromat->reset();
  ioFromat->setDStructStream(configContent);
  isload = dstruct->inDStruct(ioFromat);
  delete ioFromat;
  if (!isload) {
    cerr << "解析" << configFile << "错误!!" << endl;
    return NULL;
  }
  cDSConfig *dsconfig = new cDSConfig(dstruct->getRootNode());
  cDSConfig::addConfig(std::string(""), dsconfig);
  return dsconfig;
}

cDSConfig*
cDSConfig::reloadConfig(const std::string& configFile)
{
  cDSConfig::unloadConfig();
  return cDSConfig::loadConfig(configFile);
}

void
cDSConfig::unloadConfig()
{
  if (dstruct) {
    delete dstruct;
    dstruct = NULL;
    cDSConfig::freeAllConfig();
  }
}

cDSConfig*
cDSConfig::findConfig(const std::string& path)
{
  return config_obj_map[path];
}

void
cDSConfig::addConfig(const std::string& path, cDSConfig *config)
{
  config_obj_map[path] = config;
}

void
cDSConfig::freeAllConfig()
{
  std::map<std::string, cDSConfig*>::iterator iter = config_obj_map.begin();
  for (; iter != config_obj_map.end(); iter++) {
    delete iter->second;
  }
}

cDSConfig*
cDSConfig::getSubConfig(const std::string& path)
{
  cDSConfig *subconfig = cDSConfig::findConfig(path);
  if (subconfig)
    return subconfig;

  cDStructNode *node = dstructNode->getNode(path);
  if (!node) {
    return NULL;
  }

  subconfig = new cDSConfig(node);
  cDSConfig::addConfig(node->getNodePath(), subconfig);
  return subconfig;
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

void
cDSConfig::getSubConfigList(const std::string& path, std::vector<cDSConfig*>& subConfigList)
{
  cDSConfig *subconfig;
  dstruct::cDStructNode::mapPoolIter pathIter;
  dstruct::cDStructNode *node = dstructNode->getNode(path);
  for (pathIter = node->mapPoolBegin();
       pathIter != node->mapPoolEnd();
       pathIter++) {

    subconfig = cDSConfig::findConfig(path);
    if (!subconfig) {
      subconfig = new cDSConfig(pathIter->second);
      cDSConfig::addConfig(pathIter->second->getNodePath(), subconfig);
    }
    subConfigList.push_back(subconfig);
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
