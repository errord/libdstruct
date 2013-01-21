#ifndef _DSCONFIG_H_
#define _DSCONFIG_H_

namespace dstruct
{
  /* 
     loadConfig,reloadConfig及getSubConfig创建的对象由cDSConfig统一管理，
     调用unloadConfig统一释放

     例子:
     cDSConfig *config = cDSConfig::loadConfig(config_file);
     string s = config->getValueStr('aa.bb');
     cDSConfig *sub_config = config.getSubConfig('aa.cc');
     s = sub_config->getValueStr('dd.ee');
     cDSConfig::unloadConfig(); // 释放config, sub_config
  */
  class cDSConfig
  {
  public:
    static cDSConfig* loadConfig(const std::string& configFile);
    static cDSConfig* reloadConfig(const std::string& configFile);
    static void unloadConfig();

  private:
    static cDSConfig* findConfig(const std::string& path);
    static void addConfig(const std::string& path, cDSConfig *config);
    static void freeAllConfig();

  private:
    static dstruct::cDStruct *dstruct;
    static std::map<std::string, cDSConfig*> config_obj_map;

  public:
    cDSConfig(dstruct::cDStructNode *dsnode);
    ~cDSConfig();
    cDSConfig* getSubConfig(const std::string& path);
    void getPathNameList(const std::string& path, std::vector<std::string>& pathNameList);
    int getArraySize(const std::string& path);
    std::string getArrayStrByIdx(const std::string& path, int idx);
    int getArrayIntByIdx(const std::string& path, int idx);
    bool getArrayBoolByIdx(const std::string& path, int idx);
    double getArrayDoubleByIdx(const std::string& path, int idx);
    void getArrayValue(const std::string& path, std::vector<std::string>& valueList);

    /*
      取当前节点值
     */
    std::string getValueStr();
    std::string getValueStr(const std::string& path);
    int getValueInt(const std::string& path);
    bool getValueBool(const std::string& path);
    double getValueDouble(const std::string& path);

  private:
    dstruct::cDStructNode *dstructNode;
  };
}

#endif /* _DSCONFIG_H_ */

