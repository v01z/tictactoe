#pragma once

#if defined(__unix__)  //Linux, *BSD, Mac OS X

#define CLEAR_SCR "clear"

#elif defined (_WIN32) //Both win32 and win64

#include <windows.h>
#define CLEAR_SCR "cls"

#else 



#endif

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
