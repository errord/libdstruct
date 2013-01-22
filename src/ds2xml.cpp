#include "dsbase.h"
#include "dsioformat.h"
#include "dstruct.h"
#include "baseio.h"

using namespace dstruct;

void usage()
{
  cout << "用法: ds2xml ds格式配置文件 xml格式配置文件" << endl;
  cout << "说明:" << endl;
  cout << "\t把ds格式配置文件转换为xml格式配置文件" << endl;
}

int main(int argc, char** argv)
{
  cDStruct dstruct;
  cDSIOTextFormat textFormat;
  cDSIOXmlFormat xmlFormat;
  string dscfgfile;
  string xmlcfgfile;
  string dscfgStream;
  string xmlcfgStream;

  if (argc < 3)
    {
      usage();
      return 0;
    }

  dscfgfile = argv[1];
  xmlcfgfile = argv[2];
 
   // string resource
  string opendsstr = "打开ds格式文件[";
  opendsstr += dscfgfile;
  opendsstr += "]";

  string parsedsstr = "解析[";
  parsedsstr += dscfgfile;
  parsedsstr += "]";

  string outxmlstr = "输出xml格式文件[";
  outxmlstr += xmlcfgfile;
  outxmlstr += "]";

  cout << opendsstr << "..." << endl;
  if (cBaseIo::readTextFile(dscfgfile, dscfgStream) == false)
    {
      cerr << opendsstr << "失败!!!" << endl;
      return 0;
    }

  cout << parsedsstr << "..." << endl;
  textFormat.setDStructStream(dscfgStream);
  if (dstruct.inDStruct((iDStructIOFormat*)&textFormat) == false)
    {
      cerr << parsedsstr << "失败!!!" << endl;
      return 0;
    }
  cout << outxmlstr << "..." << endl;
  dstruct.outDStruct((iDStructIOFormat*)&xmlFormat);
  xmlcfgStream = xmlFormat.getDStructStream();
  if (cBaseIo::writeTextFile(xmlcfgfile, xmlcfgStream) == false)
    {
      cerr << outxmlstr << "失败!!!" << endl;
      return 0;
    }
  cout << "转换完成!!!" << endl;
  return 0;
}
