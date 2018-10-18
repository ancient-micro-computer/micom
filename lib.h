#include <iostream>
#include <cstring>

using namespace std;

#define strcpy_s(a, b, c) strcpy(a, c)
#define strcat_s(a, b, c) strcat(a, c)
#define strtok_s(a, b, c) strtok(a, b)
#define strncpy_s(a,b,c,d) strncpy(a, c, d)
#define strncat_s(a,b,c,d) strncat(a, c, d)

unsigned long htoi(char *str);
int Split(char *p_Splitstr, const char *p_Delimiter);
int Replace(char *p_Src, char *p_a, char *p_b, char *io_Dst);
char *Left(char *p_Src, int p_Len, char *io_Dst);
char *Right(char *p_Src, int p_Len, char *io_Dst);
char *LCase(char *p_Src, char *io_Dst);
char *UCase(char *p_Src, char *io_Dst);
int StrScrape(char *inStr, char *outStr);
#ifndef WIN32
char tolower(char c);
char toupper(char c);
#endif
int isNumeric(char *val);
unsigned long Hex(char *hexval);