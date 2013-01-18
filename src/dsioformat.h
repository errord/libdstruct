#ifndef _DSTRUCT_IO_FORMAT_H_
#define _DSTRUCT_IO_FORMAT_H_

namespace dstruct
{  
  class iDStructIOFormat
  {
  public:
    iDStructIOFormat(){
      reset();
    };
    virtual ~iDStructIOFormat(){};
    virtual void startLevel() = 0;
    virtual void endLevel() = 0;
    virtual void enterLevel() = 0;
    virtual void exitLevel() = 0;
    virtual void outHashKey(const string& key) = 0;
    virtual void outArrayIndex(int index) = 0;
    virtual void outValue(const string& key) = 0;
    // return 2 if read eof
    // return 1 if read item success
    // return 0 if read item failed
    virtual int inPathValue(string& path, string& value) = 0;
    void reset();
    void setDStructStream(const string& dstream);
    string& getDStructStream();
    bool isMultibyte(char c);
    void multibyte(string& mb);
    void addlineNum();
    string outLineNum(int offset);
    void makePath(string& path);

  protected:
    string mDSStream;
    unsigned int mStreamPos;
    vector<string> mPath;
  private:
    int mLinenum;
  };
  
  /*
   *     =============== implement  =======================
   */
  class cDSIOTextFormat : public iDStructIOFormat
  {
  public:
    cDSIOTextFormat(){
      reset();
    };
    virtual ~cDSIOTextFormat(){};
    void startLevel() {};
    void endLevel() {};
    void enterLevel();
    void exitLevel();
    void outHashKey(const string& key);
    void outArrayIndex(int index);
    void outValue(const string& value);
    int inPathValue(string& path, string& value);
    void reset();
  };

  /////////////////////////////////////////////////

  enum xmlNodeType
    {
      unknow,
      declarationnode,
      commnode,
      node,
      endnode
    };

  class cXmlNode
  {
  public:
    typedef map<string, string>::iterator tagAttrMapIter;

  public:
    cXmlNode();
    ~cXmlNode();

    void reset();
    void setNodeType(xmlNodeType type);
    void setTagName(string& name);
    void setAttr(string& name, string& value);    
    xmlNodeType getNodeType();
    string& getTagName();
    string getAttr(string& name);
    bool isAttrExist(string& name);
    
  private:
    xmlNodeType mNodeType;
    string mTagName;
    map<string, string> mTagAttrMap;
  };

  class cDSIOXmlFormat : public iDStructIOFormat
  {
  public:
    cDSIOXmlFormat();
    ~cDSIOXmlFormat();

    void startLevel();
    void endLevel();
    void enterLevel();
    void exitLevel();
    void outHashKey(const string& key);
    void outArrayIndex(int index);
    void outValue(const string& value);
    int inPathValue(string& path, string& value);
    
  public:
    void makeItem(const string& key, const string& type);
    string makeIndent();
    bool nextXmlNode(cXmlNode& xmlNode);
    void xmlValue(string& value);
    void reset();
    
  private:
    string mXmlHead;
    string mDStructItem;
    int mItemLevel;
    char* mStreamPoint;
    bool mFirstParse;
  };
}

#endif // _DSTRUCT_IO_FORMAT_H_
