#ifndef __LC_STRING_H__
#define __LC_STRING_H__
#include <string.h>





#ifndef HAVE_STRLCPY

extern size_t strlcpy(register char* t, register const char* s, register size_t n);
#endif


extern int lc_strcat(register char *dest, register const char *src, register size_t n);


extern int lc_strnlen(register char *str, register size_t maxlen);


extern int lc_strtohex(char *dst, const size_t dlen, char *src, const size_t slen);
#endif


