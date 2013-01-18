#ifndef _BASE_IO_H_
#define _BASE_IO_H_

class cBaseIo
{
 public:
  cBaseIo(){
  };
  
  virtual ~cBaseIo(){  
  };
  
  static bool readTextFile(const string& textFile, string& text);
  static bool writeTextFile(const string& textFile, string& text);
};

#endif // _BASE_IO_H_
