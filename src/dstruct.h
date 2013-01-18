#ifndef _D_STRUCT_H_
#define _D_STRUCT_H_

#include "dsbase.h"
#include "dsioformat.h"

namespace dstruct
{
  enum DSPathNodeType
    {
      unknownode,
      mapnode,
      arraynode
    };
  
  /* 路径节点 */
  class cDSPathNode
  {
  public:
    cDSPathNode();
    virtual ~cDSPathNode();

    void clear();
    void setType(DSPathNodeType type);
    DSPathNodeType getType();
    void setKey(const string& key);
    string& getKey();
    void setIndex(int index);
    int getIndex();
    
  private:
    DSPathNodeType mType;
    string mKey;
    int mIndex;
  };

  /* 路径解析 */
  class cDSPathParse
  {
  public:
    cDSPathParse();
    ~cDSPathParse();
    
    void setPath(const string& path);
    void reset();
    bool nextPathToken(cDSPathNode& pathNode);
    
  private:
    string mPath;
    const char* mPoint;
  };

  /* DStruct节点 */
  class cDStructNode
  {
  public:
    typedef map<string, cDStructNode*> mapPoolDef;
    typedef mapPoolDef* mapPoolPoint;
    typedef vector<cDStructNode*> arrayPoolDef;
    typedef arrayPoolDef* arrayPoolPoint;
    typedef map<string, cDStructNode*>::iterator mapPoolIter;
    typedef vector<cDStructNode*>::iterator arrayPoolIter;
  
    // construct destroy
  public:
    cDStructNode();
    virtual ~cDStructNode();
    cDStructNode(const cDStructNode& dstruct);
    cDStructNode(const cDStructNode* dstruct);
    
  public:
    mapPoolPoint copyMapPool() const;
    arrayPoolPoint copyArrayPool() const;
    void nodeClear();
    void clearMapPool();
    void clearArrayPool();
    void set(const string& path, const string& value);
    cDStructNode* getNode(const string& path);
    cDStructNode* insert(cDSPathNode& node);
    cDStructNode* find(cDSPathNode& node);
    bool isArray();
    bool isMap();
    /* return array size if is Array, other return -1 */
    int arraySize();
    /* return map size if is Map, other return -1 */
    int mapSize();
    string getValue();
    void getArrayValue(vector<string>& values);
    cDStructNode* getMapNode(const string& key);
    cDStructNode* getArrayNode(unsigned int index);
    void setValue(const string& value);
    void addMapNode(const string& key, cDStructNode* node);
    void addArrayNode(unsigned int index, cDStructNode* node);
    mapPoolIter mapPoolBegin();
    mapPoolIter mapPoolEnd();
    arrayPoolIter arrayPoolBegin();
    arrayPoolIter arrayPoolEnd();
  private:
    string mValue;
    mapPoolPoint mMapPool;
    arrayPoolPoint mArrayPool;
  };

  class cDStruct
  {
  public:
    cDStruct();
    cDStruct(iDStructIOFormat* inFormat);
    cDStruct(cDStructNode* structNode);
    virtual ~cDStruct();

    void clear();
    // 设置路径值
    void set(const string& path, const string& value);
    // 取根节点
    cDStructNode *getRootNode();
    // 取指定路径的值
    string get(const string& path);
    // 取指定路径的数组值
    void getArray(const string& path, vector<string>& values);
    // 取指定路径的节点
    cDStructNode *getNode(const string& path);
    void outDStruct(iDStructIOFormat* outFormat);
    bool inDStruct(iDStructIOFormat* inFormat);
    void traipsePath(cDStructNode* node, iDStructIOFormat* outFormat);
    void addend(cDStruct& dstruct);
    // 合并2个dstruct为一个新的dstruct并返回.需要调用者删除
    cDStruct *operator+(cDStruct& dstruct);
    // 追加一个dstruct,同addend函数
    void operator+=(cDStruct& dstruct);
    // 取节点,同getNode
    cDStructNode *operator[](const string& path);
    // 取值,同get
    string operator()(const string& path);
    
  private:
    cDStructNode mDSnode;
  };
};

#endif // _D_STRUCT_H_
