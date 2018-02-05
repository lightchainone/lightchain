
#include "lc_string.h"

#ifndef HAVE_STRLCPY
	extern size_t
strlcpy(register char* s, register const char* t, register size_t n)
{
	const char*	o = t;

	if (n)
		do
		{
			if (!--n)
			{
				*s = 0;
				break;
			}
		} while ((*s++ = *t++) != 0);
	if (!n)
		while (*t++);
	return t - o - 1;
}
#endif


	extern int
lc_strcat(register char* dest, register const char* src, register size_t n)
{
	char *d, *end;
	end = dest+n-1;

	d = dest;
	for(; d < end  && *d != '\0'; d++);

	for(;d<end;d++,src++){
		if(!(*d = *src)){
			return 0;
		}
	}
	return -1;

}


	extern int
lc_strnlen(register char *str, register size_t maxlen)
{
	char *d, *end;
	end = str+maxlen-1;

	d = str;

	for(;d<end &&*d!= '\0';d++);
	return d-str;
}



	extern int
lc_strtohex(char *dst, const size_t dlen, char *src, const size_t slen)
{
	char *srcstart, *srcend, *dststart;
	static char hextarray[] = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f' };
	int high,low;

	dst[0] = '\0';
	if(dlen < slen * 2 + 1)
	{
		return -2;
	}

	srcstart = src;
	srcend = src + slen -1;
	dststart = dst;
	while(srcstart <= srcend)
	{
		unsigned char* p = (unsigned char*)srcstart;
		high = *p/16;
		low = *p%16;
		*dststart++ = hextarray[high];
		*dststart++ = hextarray[low];
		srcstart++;
	}
	*dststart = '\0';
	return 0;

}
