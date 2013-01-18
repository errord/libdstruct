#include "dsbase.h"
#include "dsioformat.h"
#include "dstruct.h"
#include "baseio.h"

using namespace dstruct;

void usage()
{
  cout << "用法: xml2ds xml格式配置文件 ds格式配置文件" << endl;
  cout << "说明:" << endl;
  cout << "\t把xml格式配置文件转换为ds格式配置文件" << endl;
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

  xmlcfgfile = argv[1];
  dscfgfile = argv[2];
 
   // string resource
  string openxmlstr = "打开xml格式文件[";
  openxmlstr += xmlcfgfile;
  openxmlstr += "]";

  string parsexmlstr = "解析[";
  parsexmlstr += xmlcfgfile;
  parsexmlstr += "]";

  string outdsstr = "输出ds格式文件[";
  outdsstr += dscfgfile;
  outdsstr += "]";

  cout << openxmlstr << "..." << endl;
  if (cBaseIo::readTextFile(xmlcfgfile, xmlcfgStream) == false)
    {
      cerr << openxmlstr << "失败!!!" << endl;
      return 0;
    }

  cout << parsexmlstr << "..." << endl;
  xmlFormat.setDStructStream(xmlcfgStream);
  if (dstruct.inDStruct((iDStructIOFormat*)&xmlFormat) == false)
    {
      cerr << parsexmlstr << "失败!!!" << endl;
      return 0;
    }
  cout << outdsstr << "..." << endl;
  dstruct.outDStruct((iDStructIOFormat*)&textFormat);
  dscfgStream = textFormat.getDStructStream();
  if (cBaseIo::writeTextFile(dscfgfile, dscfgStream) == false)
    {
      cerr << outdsstr << "失败!!!" << endl;
      return 0;
    }
  cout << "转换完成!!!" << endl;
  return 0;
}
