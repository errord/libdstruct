#ifndef _EASYCHECK_H_
#define _EASYCHECK_H_

void echeck_start(const char* info);
void echeck(const char* info, int b, const char* file, int line);
void echeck_end();
#define ECHECK(i, b) echeck(i, b, __FILE__, __LINE__)

#endif /* _EASYCHECK_H_ */

