#include "dsbase.h"
#include "baseio.h"

bool
cBaseIo::readTextFile(const string& textFile, string& text)
{
  ifstream iFile(textFile.c_str(), std::ios::in);
  text = "";
  if (!iFile.is_open())
 	{
      return false;
    }
  string strTemp;
  while (!iFile.eof())
    {
      getline(iFile,strTemp);
      text += strTemp;
      text += "\n";
	}
  iFile.close();
  return true;
}

bool
cBaseIo::writeTextFile(const string& textFile, string& text)
{
  ofstream oFile(textFile.c_str(), std::ios::out);
  if (!oFile.is_open())
    {
      return false;
    }
  oFile << text;
  oFile.close();
  return true;
}
