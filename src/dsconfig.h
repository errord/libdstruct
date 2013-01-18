#ifndef _DSCONFIG_H_
#define _DSCONFIG_H_

namespace dstruct
{
  class cDSConfig
  {
  public:
    cDSConfig();
    ~cDSConfig();
    bool loadConfig(const std::string& configFile);
    void getPathNameList(const std::string& path, std::vector<std::string>& pathNameList);
    int getArraySize(const std::string& path);
    std::string getArrayStrByIdx(const std::string& path, int idx);
    int getArrayIntByIdx(const std::string& path, int idx);
    bool getArrayBoolByIdx(const std::string& path, int idx);
    double getArrayDoubleByIdx(const std::string& path, int idx);
    void getArrayValue(const std::string& path, std::vector<std::string>& valueList);
    std::string getValueStr();
    std::string getValueStr(const std::string& path);
    int getValueInt(const std::string& path);
    bool getValueBool(const std::string& path);
    double getValueDouble(const std::string& path);
    void setPathName(const std::string& name);
    void downPathName(const std::string& name);
    void downPathName(const std::string& name, int idx);
    void upPathName();

  private:
    bool readConfigFile(const std::string& configFile, std::string& text);

  private:
    dstruct::cDStruct dstruct;
    dstruct::cDStructNode *dstructNode;
    std::vector<dstruct::cDStructNode*> nodeStack;
  };
}

#endif /* _DSCONFIG_H_ */

