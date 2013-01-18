#ifndef _LIBDSTRUCT_BASE_H_
#define _LIBDSTRUCT_BASE_H_

#include <assert.h>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

inline void memoryout(const char* file = __FILE__, int line = __LINE__)
{
  cerr << "memory out at file: " << __FILE__ << " line: " << line << " !!!" << endl;
}


#endif /* _LIBDSTRUCT_BASE_H_ */

