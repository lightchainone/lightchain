#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <assert.h>
#include "ul_log.h"
#include "ul_exlink.h"
#include "ul_url.h"
#include "ul_char.h"

#ifndef SAFE_CTYPE_H
#define SAFE_CTYPE_H

/* Catch erroneous use of ctype macros.  Files that really know what
   they're doing can disable this check by defining the
   I_REALLY_WANT_CTYPE_MACROS preprocessor constant. */

#ifndef I_REALLY_WANT_CTYPE_MACROS

#undef isalpha
#define isalpha *** Please use ISALPHA ***
#undef isalnum
#define isalnum *** Please use ISALNUM ***
#undef isblank
#define isblank *** Please use ISBLANK ***
#undef iscntrl
#define iscntrl *** Please use ISCNTRL ***
#undef isdigit
#define isdigit *** Please use ISDIGIT ***
#undef isgraph
#define isgraph *** Please use ISGRAPH ***
#undef islower
#define islower *** Please use ISLOWER ***
#undef isprint
#define isprint *** Please use ISPRINT ***
#undef ispunct
#define ispunct *** Please use ISPUNCT ***
#undef isspace
#define isspace *** Please use ISSPACE ***
#undef isupper
#define isupper *** Please use ISUPPER ***
#undef isxdigit
#define isxdigit *** Please use ISXDIGIT ***

#endif /* I_REALLY_WANT_CTYPE_MACROS */

/* Categories.  */

enum {
	/* In C99 */
	_sch_isblank = 0x0001,		/* space \t */
	_sch_iscntrl = 0x0002,		/* nonprinting characters */
	_sch_isdigit = 0x0004,		/* 0-9 */
	_sch_islower = 0x0008,		/* a-z */
	_sch_isprint = 0x0010,		/* any printing character including ' ' */
	_sch_ispunct = 0x0020,		/* all punctuation */
	_sch_isspace = 0x0040,		/* space \t \n \r \f \v */
	_sch_isupper = 0x0080,		/* A-Z */
	_sch_isxdigit = 0x0100,		/* 0-9A-Fa-f */

	/* Extra categories useful to cpplib.  */
	_sch_isidst = 0x0200,		/* A-Za-z_ */
	_sch_isvsp = 0x0400,		/* \n \r */
	_sch_isnvsp = 0x0800,		/* space \t \f \v \0 */

	/* Combinations of the above.  */
	_sch_isalpha = _sch_isupper | _sch_islower,	/* A-Za-z */
	_sch_isalnum = _sch_isalpha | _sch_isdigit,	/* A-Za-z0-9 */
	_sch_isidnum = _sch_isidst | _sch_isdigit,	/* A-Za-z0-9_ */
	_sch_isgraph = _sch_isalnum | _sch_ispunct,	/* isprint and not space */
	_sch_iscppsp = _sch_isvsp | _sch_isnvsp	/* isspace + \0 */
};

/* Character classification.  */
extern const unsigned short _sch_istable[256];

#define _sch_test(c, bit) (_sch_istable[(c) & 0xff] & (bit))

#define ISALPHA(c)  _sch_test(c, _sch_isalpha)
#define ISALNUM(c)  _sch_test(c, _sch_isalnum)
#define ISBLANK(c)  _sch_test(c, _sch_isblank)
#define ISCNTRL(c)  _sch_test(c, _sch_iscntrl)
#define ISDIGIT(c)  _sch_test(c, _sch_isdigit)
#define ISGRAPH(c)  _sch_test(c, _sch_isgraph)
#define ISLOWER(c)  _sch_test(c, _sch_islower)
#define ISPRINT(c)  _sch_test(c, _sch_isprint)
#define ISPUNCT(c)  _sch_test(c, _sch_ispunct)
#define ISSPACE(c)  _sch_test(c, _sch_isspace)
#define ISUPPER(c)  _sch_test(c, _sch_isupper)
#define ISXDIGIT(c) _sch_test(c, _sch_isxdigit)

#define ISIDNUM(c)	_sch_test(c, _sch_isidnum)
#define ISIDST(c)	_sch_test(c, _sch_isidst)
#define IS_VSPACE(c)	_sch_test(c, _sch_isvsp)
#define IS_NVSPACE(c)	_sch_test(c, _sch_isnvsp)
#define IS_SPACE_OR_NUL(c)	_sch_test(c, _sch_iscppsp)

/* Character transformation.  */
extern const unsigned char _sch_toupper[256];
extern const unsigned char _sch_tolower[256];
#define TOUPPER(c) _sch_toupper[(c) & 0xff]
#define TOLOWER(c) _sch_tolower[(c) & 0xff]

#endif /* SAFE_CTYPE_H */

#define bl _sch_isblank
#define cn _sch_iscntrl
#define di _sch_isdigit
#define is _sch_isidst
#define lo _sch_islower
#define nv _sch_isnvsp
#define pn _sch_ispunct
#define pr _sch_isprint
#define sp _sch_isspace
#define up _sch_isupper
#define vs _sch_isvsp
#define xd _sch_isxdigit

/* Masks.  */
#define L  lo|is   |pr			/* lower case letter */
#define XL lo|is|xd|pr			/* lowercase hex digit */
#define U  up|is   |pr			/* upper case letter */
#define XU up|is|xd|pr			/* uppercase hex digit */
#define D  di   |xd|pr			/* decimal digit */
#define P  pn      |pr			/* punctuation */
#define _  pn|is   |pr			/* underscore */

#define C           cn			/* control character */
#define Z  nv      |cn			/* NUL */
#define M  nv|sp   |cn			/* cursor movement: \f \v */
#define V  vs|sp   |cn			/* vertical space: \r \n */
#define T  nv|sp|bl|cn			/* tab */
#define S  nv|sp|bl|pr			/* space */

/* Are we ASCII? */
#if '\n' == 0x0A && ' ' == 0x20 && '0' == 0x30 \
	&& 'A' == 0x41 && 'a' == 0x61 && '!' == 0x21 \
&& EOF == -1

const unsigned short _sch_istable[256] = {
	Z, C, C, C, C, C, C, C,		/* NUL SOH STX ETX  EOT ENQ ACK BEL */
	C, T, V, M, M, V, C, C,		/* BS  HT  LF  VT   FF  CR  SO  SI  */
	C, C, C, C, C, C, C, C,		/* DLE DC1 DC2 DC3  DC4 NAK SYN ETB */
	C, C, C, C, C, C, C, C,		/* CAN EM  SLC ESC  FS  GS  RS  US  */
	S, P, P, P, P, P, P, P,		/* SP  !   "   #    $   %   &   '   */
	P, P, P, P, P, P, P, P,		/* (   )   *   +    ,   -   .   /   */
	D, D, D, D, D, D, D, D,		/* 0   1   2   3    4   5   6   7   */
	D, D, P, P, P, P, P, P,		/* 8   9   :   ;    <   =   >   ?   */
	U, U, U, U, U, U, U, U,		/* H   I   J   K    L   M   N   O   */
	U, U, U, U, U, U, U, U,		/* P   Q   R   S    T   U   V   W   */
	U, U, U, P, P, P, P, _,		/* X   Y   Z   [    \   ]   ^   _   */
	P, XL, XL, XL, XL, XL, XL, L,	/* `   a   b   c    d   e   f   g   */
	L, L, L, L, L, L, L, L,		/* h   i   j   k    l   m   n   o   */
	L, L, L, L, L, L, L, L,		/* p   q   r   s    t   u   v   w   */
	L, L, L, P, P, P, P, C,		/* x   y   z   {    |   }   ~   DEL */

	/* high half of unsigned char is locale-specific, so all tests are
	 * false in "C" locale */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

const unsigned char _sch_tolower[256] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	64,

	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

	91, 92, 93, 94, 95, 96,

	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',

	123, 124, 125, 126, 127,

	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
	144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
	176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,

	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
};

const unsigned char _sch_toupper[256] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
	32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
	48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
	64,

	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

	91, 92, 93, 94, 95, 96,

	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',

	123, 124, 125, 126, 127,

	128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143,
	144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159,
	160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175,
	176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191,

	192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207,
	208, 209, 210, 211, 212, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223,
	224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239,
	240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255,
};

#else
#error "Unsupported host character set"
#endif /* not ASCII */


/*
   int end(char p)
   {
   if (p == '"' || p=='\'' || p == ' ' || p=='\r' || p == '\n' || p == '\t' || p == '>')
   return 1;
   return 0;
   }
   */

int getlink(char *p, int offset, char *url, char *text)
{
	int i = 0;
	char *q;
	char end_char = -1;
	if (*p == '"' || *p == '\'') {
		end_char = *p;
		p++;
	}
	while (ISSPACE(*p))
		p++;
	/* get link url */
	if (end_char == -1) {
		while (!ISSPACE(*p) && *p != '>') {
			url[i] = *(p + offset);
			i++;
			p++;
			if (i == UL_MAX_URL_LEN ) {
				return 0;
			}
		}
	} else {
		while (*p != end_char && *p != '>') {
			url[i] = *(p + offset);
			i++;
			p++;
			if (i == UL_MAX_URL_LEN ) {
				return 0;
			}
		}
		//trim last space char 
		while (i > 0 && ISSPACE(url[i - 1]))
			i--;
	}
	url[i] = '\0';


	while (ISSPACE(*p))
		p++;
	/* get ancor text */
	p = strchr(p, '>');

	if (p == NULL)
		return 0;

	p++;
	while (ISSPACE(*p))
		p++;
	q = strstr(p, "</a");
	if (q == NULL) {
		text[0] = '\0';
		return 1;
	}
	q--;

	i = 0;
	while (p <= q) {
		text[i] = *(p + offset);
		i++;
		p++;
		if (i == UL_MAX_TEXT_LEN || *p == '\0') {
			text[0] = '\0';
			return 1;
		}
	}
	text[i] = '\0';
	return 1;
}

char *strlink(char *page)
{
	char *p, *q;
	p = page;


  again:
	q = strchr(p, '<');
	if (q == NULL)
		return NULL;
	q++;
	if (strncmp(q, "script", 6) == 0) {
		//skip script tag
		p = strstr(q, "</script>");
		if (p == NULL)
			return NULL;
		p += strlen("</script>");
		goto again;
	} else if (strncmp(q, "a ", 2) == 0) {
		//link tag
		char *last;
		last = strchr(q, '>');
		if (last == NULL)
			return NULL;
		p = strstr(q, " href");
		if (p == NULL || p > last) {
			//no href varible
			p = last + 1;
			goto again;
		}
		p += strlen(" href");
		while (ISSPACE(*p))
			p++;
		if (*p != '=') {
			//not href variable
			p = last + 1;
			goto again;
		}
		p++;
		return p;
	} else if (strncmp(q, "frame ", 6) == 0 || strncmp(q, "iframe ", 7) == 0) {
		char *last;
		last = strchr(q, '>');
		if (last == NULL)
			return NULL;
		p = strstr(q, " src");
		if (p == NULL || p > last) {
			//no src varialbe
			p = last + 1;
			goto again;
		}

		p += strlen(" src");
		while (ISSPACE(*p))
			p++;
		if (*p != '=') {
			//not href variable
			p = last + 1;
			goto again;
		}
		p++;
		return p;
	} else if (strncmp(q, "!--", 3) == 0) {
		//skip comments tag
		q += 3;
		p = strstr(q, "--");
		if (p == NULL)
			return NULL;
		p = strchr(p, '>');
		if (p == NULL)
			return NULL;
		p++;
		goto again;



	} else {
		//skip this tag
		p = strchr(q, '>');
		if (p == NULL)
			return NULL;
		p++;
		goto again;
	}

}
int is_url(char *url)
{
	if (strncmp(url, "http://", 7) == 0)
		return 1;
	return 0;
}

int is_abs_path(char *path)
{
	if (*path == '/')
		return 1;
	else
		return 0;
}

int is_rel_path(char *url)
{
	char *p;
	if (strncmp(url, "http://", 7) == 0)
		return 0;
	p = strchr(url, ':');
	if (p != NULL && p - url <= 10)
		//path, 10 is the length of the longest shemas javascript
		return 0;
	if (is_abs_path(url))
		return 0;
	return 1;

}


int is_path(char *url)
{
	return is_abs_path(url) || is_rel_path(url);
}

/* only the http:// and the path is a legal url*/
int is_pathurl(char *url)
{
	return is_url(url) || is_path(url);
}


/**
 *
 * - 0 鎴愬姛
 * - -1 澶辫触
 */
int pure_anchor_text(char *text)
{
	char buffer[UL_MAX_TEXT_LEN];
	char *p, *q;
	q = buffer;
	p = text;
	//strip  all tag
	while (*p != '\0') {
		if (*p != '<')
			*q++ = *p++;
		else {
			p = strchr(p, '>');
			if (p == NULL) {
				// error
				text[0] = '\0';
				return -1;
			}
			p++;
		}
	}
	*q = '\0';

	//trim space char 
	p = buffer;
	while (*p != '\0' && ISSPACE(*p))
		p++;
	q = buffer + strlen(buffer) - 1;
	while (q > p && ISSPACE(*q))
		q--;
	q++;
	*q = '\0';
	//copy p to text with repacing \r \n to space char
	while (*p) {
		if (ISSPACE(*p)) {
			*text++ = ' ';
			p++;
			while (ISSPACE(*p)) {
				p++;
			}
		} else {
			*text++ = *p++;
		}
	}
	*text = '\0';
	return 0;

}

#define MAX_PAGE_SIZE 256000

int exrellink(char *page, link_info_t link_info[], int num)
{
	char *p;
	char lower_page[MAX_PAGE_SIZE];

	int n = 0;
	
	ul_tolowerstr((unsigned char *) lower_page, (unsigned char *) page);

	p = lower_page;

	while ((p = strlink(p)) != NULL) {
		/* 这个地方两个参数：p, page - lower_page的意义是：getlink p+offset = page+(p - lower_page) */
		if (!getlink(p, page - lower_page, link_info[n].url, link_info[n].text)) {
			ul_writelog(UL_LOG_DEBUG,
						"Error link in number %ld character(...%.32s...).",
						(unsigned long) (p - page), p);
			continue;
		}
		if (!is_pathurl(link_info[n].url))
		{
			continue;
		}

		if (pure_anchor_text(link_info[n].text) < 0) /**< error occur */
		{
			continue;
		}

		n++;
		if (n == num)
			break;
	}
	return n;
}


int exlink(char *url, char *page, link_info_t link_info[], int num)
{
	int linknum;
	char domain[UL_MAX_SITE_LEN], port[UL_MAX_PORT_LEN], dir[UL_MAX_PATH_LEN];
	char staticpart_url[UL_MAX_URL_LEN];
	char *p;

	//  assert(is_url(url));
	if (!is_url(url)) {
		ul_writelog(UL_LOG_DEBUG, "%s is not url", url);
		return -1;
	}


	ul_get_static_part(url, staticpart_url);
	ul_parse_url(staticpart_url, domain, port, dir);
	p = strrchr(dir, '/');
	if (p == NULL) {
		ul_writelog(UL_LOG_DEBUG, "error dir:%s", dir);
		return -1;
	}
	*(p + 1) = '\0';

	linknum = exrellink(page, link_info, num);
	for (int i = 0; i < linknum; i++) {
		//assert(is_pathurl(link_info[i].url));
		//absolute path
		if (is_abs_path(link_info[i].url)) {
			//too long path
			if (strlen(link_info[i].url) >= UL_MAX_PATH_LEN) {
				ul_writelog(UL_LOG_DEBUG, "too long path:%s", link_info[i].url);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}

			//single path
			strcpy(link_info[i].path, link_info[i].url);
			if (!ul_single_path(link_info[i].path)) {
				ul_writelog(UL_LOG_DEBUG, "error path:%s", link_info[i].path);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}

			//normalize path
			ul_normalize_path(link_info[i].path);

			//domain and port   
			strcpy(link_info[i].domain, domain);
			strcpy(link_info[i].port, port);

			//relative path
		} else if (is_rel_path(link_info[i].url)) {
			//too long path
			if (strlen(link_info[i].url) + strlen(dir) >= UL_MAX_PATH_LEN) {
				ul_writelog(UL_LOG_DEBUG, "too long path:%s%s", dir, link_info[i].url);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}

			//script
			if (strncasecmp(link_info[i].url, "javascript:", 11) == 0) {
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}

			//single path
			strcpy(link_info[i].path, dir);
			strcat(link_info[i].path, link_info[i].url);
			if (!ul_single_path(link_info[i].path)) {
				ul_writelog(UL_LOG_DEBUG, "error path:%s", link_info[i].path);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}

			//normalize path
			ul_normalize_path(link_info[i].path);

			//domain and port
			strcpy(link_info[i].domain, domain);
			strcpy(link_info[i].port, port);

			//url
		} else if (is_url(link_info[i].url)) {
			if (0 ==
				ul_parse_url(link_info[i].url, link_info[i].domain, link_info[i].port,
							 link_info[i].path)) {
				ul_writelog(UL_LOG_DEBUG, "ul_parse_urll error, url=%s", link_info[i].url);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}
			ul_tolowerstr_singlebuf((unsigned char *) link_info[i].domain);

			//single path
			if (!ul_single_path(link_info[i].path)) {
				ul_writelog(UL_LOG_DEBUG, "error path:%s", link_info[i].path);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}

			//normalize path
			ul_normalize_path(link_info[i].path);

		} else {
			ul_writelog(UL_LOG_DEBUG, "parse error!\n");
			return -1;
			//assert(0);
		}

	}

	return linknum;

}

static char *protostrings[] = {
	"cid:",
	"clsid:",
	"file:",
	"finger:",
	"ftp:",
	"gopher:",
	"hdl:",
	"http:",
	"https:",
	"ilu:",
	"ior:",
	"irc:",
	"java:",
	"javascript:",
	"lifn:",
	"mailto:",
	"mid:",
	"news:",
	"nntp:",
	"path:",
	"prospero:",
	"rlogin:",
	"service:",
	"shttp:",
	"snews:",
	"stanf:",
	"telnet:",
	"tn3270:",
	"wais:",
	"whois++:",
	NULL
};

#define MINVAL(x, y) ((x) < (y) ? (x) : (y))
#define DEFAULT_HTTP_PORT 80
#define DEFAULT_FTP_PORT 21
#define ARRAY_SIZE(array) (sizeof (array) / sizeof (*(array)))
#define FREE_MAYBE(foo) do { if (foo) free (foo); } while (0)
#define EQ 0

struct proto {
	char *name;
	unsigned short port;
};
static struct proto sup_protos[] = {
	{"http://", DEFAULT_HTTP_PORT},
	//  { "ftp://",  DEFAULT_FTP_PORT },
	/*{ "file://", URLFILE, DEFAULT_FTP_PORT }, */
};
struct tag_attr {
	char *tag;
	char *attr;
};
typedef struct {
	int at_value, in_quote;
	char quote_char;
	char *tag, *attr;
	char *base;
} state_t;

char *strdupdelim(const char *beg, const char *end)
{
	if (beg == NULL || end==NULL) return NULL;
	char *res = (char *) malloc(end - beg + 1);
	if (res==NULL) return NULL;
	memcpy(res, beg, end - beg);
	res[end - beg] = '\0';
	return res;
}

static int idmatch(struct tag_attr *tags, const char *tag, const char *attr)
{
	int i;

	if (tag == NULL || attr == NULL)
		return 0;

	for (i = 0; tags[i].tag; i++)
		/* Loop through all the tags wget ever cares about. */
		if (!strcasecmp(tags[i].tag, tag) && !strcasecmp(tags[i].attr, attr))
			/* The tag and attribute matched one of the ones wget cares about. */
		{
			/* If we get to here, --follow-tags isn't being used, and --ignore-tags,                                 
			 * if specified, didn't include this tag, so it's okay to follow. */
			return 1;
		}
	return 0;					/* not one of the tag/attribute pairs wget ever cares about */
}

void GetAnchorText(const char *buf, char *text, int bufsize)
{
	int errflag = 0, i = 0;
	for (; bufsize && *buf != '>'; ++buf, --bufsize);
	if (!bufsize)
		return;
	buf++;
	bufsize--;

	while (1) {
		if (*buf != '<') {
			if ((*buf != '\r') && (*buf != '\n')) {
				text[i++] = *buf;
				buf++;
			} else {
				text[i++] = ' ';
				buf++;
			}
		} else {
			for (; bufsize && *buf != '>'; ++buf, --bufsize) {
				if (*buf == '/' && (*(buf + 1) == 'a' || *(buf + 1) == 'A')) {
					goto end;
				}
			}
			if (!bufsize) {
				errflag = 1;
				break;
			}
			if (*buf == '>') {
				buf++;
				--bufsize;
			}
		}
		if (i >= UL_MAX_TEXT_LEN) {
			errflag = 1;
			break;
		}
	}
  end:
	if (errflag) {
		text[i] = 0;
		ul_writelog(UL_LOG_DEBUG, "GetAnchorText err %s", text);
		memset(text, 0, UL_MAX_TEXT_LEN);
	} else {
		text[i] = 0;
		//      ul_writelog(UL_LOG_DEBUG,"GetAnchorText success %s",text);
	}
}

const char *htmlfindurl(const char *buf, int bufsize, int *size, int init, int dash_p_leaf_HTML,
						char *text, state_t * s)
{
	const char *p, *ph;
	//  state_t    *s = &global_state;

	static struct tag_attr html_allow[] = {
		//      { "script", "src" },
		//      { "img", "src" },
		//      { "img", "href" },
		//      { "body", "background" },
		{"frame", "src"},
		{"iframe", "src"},
		//      { "fig", "src" },
		//      { "overlay", "src" },
		//      { "applet", "code" },
		//      { "script", "src" },
		//      { "embed", "src" },
		//      { "bgsound", "src" },
		//      { "img", "lowsrc" },
		//      { "input", "src" },
		//      { "layer", "src" },
		//      { "table", "background"},
		//      { "th", "background"},
		//      { "td", "background"},
		/* Tags below this line are treated specially.  */
		{"a", "href"},
		{"area", "href"},
		//      { "base", "href" },
		{"link", "href"},
		//      { "link", "rel" },
		{"meta", "content"},
		{NULL, NULL}
	};

	if (init)
		memset(s, 0, sizeof(*s));
	text[0] = '\0';
	while (1) {
		const char *link_href = NULL;
		const char *link_rel = NULL;
		int link_href_saved_size = 0;	/* init. just to shut up warning */

		if (!bufsize)
			break;
		/* Let's look for a tag, if we are not already in one.  */
		if (!s->at_value) {
			/* Find '<'.  */
			if (*buf != '<')
				for (; bufsize && *buf != '<'; ++buf, --bufsize);
			if (!bufsize)
				break;
			/* Skip spaces.  */
			for (++buf, --bufsize; bufsize && ISSPACE(*buf) && *buf != '>'; ++buf, --bufsize);
			if (!bufsize)
				break;
			p = buf;
			/* Find the tag end.  */
			for (; bufsize && !ISSPACE(*buf) && *buf != '>' && *buf != '='; ++buf, --bufsize);
			if (!bufsize)
				break;
			if (*buf == '=') {
				/* <tag=something> is illegal.  Just skip it.  */
				++buf, --bufsize;
				continue;
			}
			if (p == buf) {
				/* *buf == '>'.  */
				++buf, --bufsize;
				continue;
			}
			s->tag = strdupdelim(p, buf);
			if (*buf == '>') {
				free(s->tag);
				s->tag = NULL;
				++buf, --bufsize;
				continue;
			}
		} else {				/* s->at_value */

			/* Reset AT_VALUE.  */
			s->at_value = 0;
			/* If in quotes, just skip out of them and continue living.  */
			if (s->in_quote) {
				s->in_quote = 0;
				for (; bufsize && *buf != s->quote_char; ++buf, --bufsize);
				if (!bufsize)
					break;
				++buf, --bufsize;
			}
			if (!bufsize)
				break;
			if (*buf == '>') {
				FREE_MAYBE(s->tag);
				FREE_MAYBE(s->attr);
				s->tag = s->attr = NULL;
				continue;
			}
		}
		/* Find the attributes.  */
		do {
			FREE_MAYBE(s->attr);
			s->attr = NULL;
			if (!bufsize)
				break;
			/* Skip the spaces if we have them.  We don't have them at
			 * places like <img alt="something"src="something-else">.
			 * ^ no spaces here */
			if (ISSPACE(*buf))
				for (++buf, --bufsize; bufsize && ISSPACE(*buf) && *buf != '>'; ++buf, --bufsize);
			if (!bufsize || *buf == '>')
				break;
			if (*buf == '=') {
				/* This is the case of <tag = something>, which is illegal.  Just skip it.  */
				++buf, --bufsize;
				continue;
			}
			p = buf;
			/* Find the attribute end.  */
			for (; bufsize && !ISSPACE(*buf) && *buf != '>' && *buf != '='; ++buf, --bufsize);
			if (!bufsize || *buf == '>')
				break;
			/* Construct the attribute.  */
			s->attr = strdupdelim(p, buf);
			/* Now we must skip the spaces to find '='.  */
			if (*buf != '=') {
				for (; bufsize && ISSPACE(*buf) && *buf != '>'; ++buf, --bufsize);
				if (!bufsize || *buf == '>')
					break;
			}
			/* If we still don't have '=', something is amiss.  */
			if (*buf != '=')
				continue;
			/* Find the beginning of attribute value by skipping the spaces.  */
			++buf, --bufsize;
			for (; bufsize && ISSPACE(*buf) && *buf != '>'; ++buf, --bufsize);
			if (!bufsize || *buf == '>')
				break;
			ph = NULL;
			/* The value of an attribute can, but does not have to be quoted.  */
			if (*buf == '\"' || *buf == '\'') {
				s->in_quote = 1;
				s->quote_char = *buf;
				p = buf + 1;
				for (++buf, --bufsize; bufsize && *buf != s->quote_char && *buf != '\n';
					 ++buf, --bufsize)
					if (!ph && *buf == '#' && *(buf - 1) != '&')
						ph = buf;
				if (!bufsize) {
					s->in_quote = 0;
					break;
				}
				if (*buf == '\n') {
					/* #### Is the following logic good?
					 * Obviously no longer in quote.  It might be well
					 * to check whether '>' was encountered, but that
					 * would be encouraging writers of invalid HTMLs,
					 * and we don't want that, now do we?  */
					s->in_quote = 0;
					continue;
				}
			} else {
				p = buf;
				for (; bufsize && !ISSPACE(*buf) && *buf != '>'; ++buf, --bufsize)
					if (!ph && *buf == '#' && *(buf - 1) != '&')
						ph = buf;
				if (!bufsize)
					break;
			}
			/* If '#' was found unprotected in a URI, it is probably an HTML marker, or color spec.  */
			*size = (ph ? ph : buf) - p;
			/* The URI is liable to be returned if:
			 * 1) *size != 0;
			 * 2) its tag and attribute are found in html_allow.  */
			if (*size && idmatch(html_allow, s->tag, s->attr)) {
				if (strcasecmp(s->tag, "a") == EQ || strcasecmp(s->tag, "area") == EQ) {
					/* Only follow these if we're not at a -p leaf node, as they always link to external documents. */
					if (strcasecmp(s->tag, "a") == EQ) {
						GetAnchorText(buf, text, bufsize);
					}
					if (!dash_p_leaf_HTML) {
						s->at_value = 1;
						return p;
					}
				} else if (!strcasecmp(s->tag, "base") && !strcasecmp(s->attr, "href")) {
					FREE_MAYBE(s->base);
					s->base = strdupdelim(p, buf);
				} else if (strcasecmp(s->tag, "link") == EQ) {
					if (strcasecmp(s->attr, "href") == EQ) {
						link_href = p;
						link_href_saved_size = *size;	/* for restoration below */
					} else if (strcasecmp(s->attr, "rel") == EQ)
						link_rel = p;

					if (link_href != NULL && link_rel != NULL)
						/* Okay, we've now seen this <LINK> tag's HREF and REL
						 * attributes (they may be in either order), so it's now
						 * possible to decide if we want to traverse it. */
						if (!dash_p_leaf_HTML || strncasecmp(link_rel, "stylesheet",
															 sizeof("stylesheet") - 1) == EQ)
							/* In the normal case, all <LINK> tags are fair game.
							 * In the special case of when -p is active, however, and
							 * we're at a leaf node (relative to the -l max. depth) in
							 * the HTML document tree, the only <LINK> tag we'll
							 * follow is a <LINK REL="stylesheet">, as it's necessary
							 * for displaying this document properly.  We won't follow
							 * other <LINK> tags, like <LINK REL="home">, for
							 * instance, as they refer to external documents.
							 * 
							 * Note that the above strncasecmp() will incorrectly
							 * consider something like '<LINK REL="stylesheet.old"' as
							 * equivalent to '<LINK REL="stylesheet"'.  Not really
							 * worth the trolcle to explicitly check for such cases --
							 * if time is spent, it should be spent ripping out wget's
							 * somewhat kludgy HTML parser and hooking in a real,
							 * componentized one. */
						{
							/* When we return, the 'size' IN/OUT parameter
							 * determines where in the buffer the end of the current
							 * attribute value is.  If REL came after HREF in this
							 * <LINK> tag, size is currently set to the size for
							 * REL's value -- set it to what it was when we were
							 * looking at HREF's value. */
							*size = link_href_saved_size;
							s->at_value = 1;
							return link_href;
						}
				} else if (!strcasecmp(s->tag, "meta") && !strcasecmp(s->attr, "content")) {
					/* Some pages use a META tag to specify that the page
					 * be refreshed by a new page after a given number of
					 * seconds.  We need to attempt to extract an URL for
					 * the new page from the other garbage present.  The
					 * general format for this is:                  
					 * <META HTTP-EQUIV=Refresh CONTENT="0; URL=index2.html">
					 * 
					 * So we just need to skip past the "0; URL="
					 * garbage to get to the URL.  META tags are also
					 * used for specifying random things like the page
					 * author's name and what editor was used to create
					 * it.  So we need to be careful to ignore them and
					 * not assume that an URL will be present at all.  */
					for (; *size && ISDIGIT(*p); p++, *size -= 1);
					if (*p == ';') {
						for (p++, *size -= 1; *size && ISSPACE(*p); p++, *size -= 1);
						if (!strncasecmp(p, "URL=", 4)) {
							p += 4, *size -= 4;
							s->at_value = 1;
							return p;
						}
					}
				} else {
					s->at_value = 1;
					return p;
				}
			}
			/* Exit from quote.  */
			if (*buf == s->quote_char) {
				s->in_quote = 0;
				++buf, --bufsize;
			}
		} while (*buf != '>');
		FREE_MAYBE(s->tag);
		FREE_MAYBE(s->attr);
		s->tag = s->attr = NULL;
		if (!bufsize)
			break;
	}

	FREE_MAYBE(s->tag);
	FREE_MAYBE(s->attr);
	FREE_MAYBE(s->base);
	memset(s, 0, sizeof(*s));	/* just to be sure */
	return NULL;
}

char *html_decode_entities(const char *beg, const char *end)
{
	char *newstr = (char *) malloc(end - beg + 1);	/* assume worst-case. */
	const char *from = beg;
	char *to = newstr;

	while (from < end) {
		if (*from != '&')
			*to++ = *from++;
		else {
			const char *save = from;
			int remain;

			if (++from == end)
				goto lose;
			remain = end - from;

			if (*from == '#') {
				int numeric;
				++from;
				if (from == end || !ISDIGIT(*from))
					goto lose;
				for (numeric = 0; from < end && ISDIGIT(*from); from++)
					numeric = 10 * numeric + (*from) - '0';
				if (from < end && ISALPHA(*from))
					goto lose;
				numeric &= 0xff;
				*to++ = numeric;
			}
#define FROB(literal) (remain >= (int)(sizeof (literal) - 1)			\
		&& !memcmp (from, literal, sizeof (literal) - 1)	\
		&& (*(from + sizeof (literal) - 1) == ';'		\
			|| remain == sizeof (literal) - 1			\
			|| !ISALNUM (*(from + sizeof (literal) - 1))))
			else if (FROB("lt"))
				*to++ = '<', from += 2;
			else if (FROB("gt"))
				*to++ = '>', from += 2;
			else if (FROB("amp"))
				*to++ = '&', from += 3;
			else if (FROB("quot"))
				*to++ = '\"', from += 4;
			/* We don't implement the "Added Latin 1" entities proposed
			 * by rfc1866 (except for nbsp), because it is unnecessary
			 * in the context of Wget, and would require hashing to work
			 * efficiently.  */
			else if (FROB("nbsp"))
				*to++ = 160, from += 4;
			else
				goto lose;
#undef FROB
			/* If the entity was followed by `;', we step over the `;'.
			 * Otherwise, it was followed by either a non-alphanumeric
			 * or EOB, in which case we do nothing.  */
			if (from < end && *from == ';')
				++from;
			continue;

		  lose:
			/* This was not an entity after all.  Back out.  */
			from = save;
			*to++ = *from++;
		}
	}
	*to++ = '\0';
	/* #### Should we try to do this: */
#if 0
	newstr = realloc(newstr, to - newstr);
#endif
	return newstr;
}

int wget_exrellink(char *page, int pagesize, link_info_t * link_info, int num)
{
	//char *orig_buf;
	const char *buf;
	int step, first_time = 1;
	int count = 0;

	state_t global_state;

	for (buf = page;
		 (buf =
		  htmlfindurl(buf, pagesize - (buf - page), &step, first_time, 0, link_info[count].text,
					  &global_state)); buf += step) {
		int i, no_proto;
		int size = step;
		const char *pbuf = buf;
		//char *constr, *base;
		//const char *cbase;
		char *needs_freeing, *url_data;
		first_time = 0;

		while ((pbuf < buf + step) && ISSPACE(*pbuf)) {
			++pbuf;
			--size;
		}
		while (size && ISSPACE(pbuf[size - 1]))
			--size;
		if (!size)
			break;

		needs_freeing = url_data = html_decode_entities(pbuf, pbuf + size);
		size = strlen(url_data);

		for (i = 0; protostrings[i]; i++) {
			if (!strncasecmp
				(protostrings[i], url_data, MINVAL((int) strlen(protostrings[i]), size)))
				break;
		}
		if (protostrings[i]
			&& !(strncasecmp(url_data, "http:", 5) == 0
				 && strncasecmp(url_data, "http://", 7) != 0)) {
			no_proto = 0;
		} else {
			no_proto = 1;
			if ((size > 5) && !strncasecmp("http:", url_data, 5))
				url_data += 5, size -= 5;
		}
		if (!no_proto) {
			for (i = 0; i < (int) ARRAY_SIZE(sup_protos); i++) {
				if (!strncasecmp
					(sup_protos[i].name, url_data, MINVAL((int) strlen(sup_protos[i].name), size)))
					break;
			}
			if (i == (int) ARRAY_SIZE(sup_protos)) {
				free(needs_freeing);
				continue;
			}
		}
		size = (size > UL_MAX_URL_LEN - 1) ? (UL_MAX_URL_LEN - 1) : size;
		strncpy(link_info[count].url, url_data, size);
		(link_info[count].url)[size] = '\0';
		if (++count >= num) {
			free(needs_freeing);
			break;
		}
		free(needs_freeing);
	}
	return count;
}

int wget_exlink(char *url, char *page, int pagesize, link_info_t * link_info, int num)
{
	int linknum;
	char domain[UL_MAX_SITE_LEN], port[UL_MAX_PORT_LEN], dir[UL_MAX_PATH_LEN];
	char staticpart_url[UL_MAX_URL_LEN];
	char *p;

	//  assert(is_url(url));
	if (!is_url(url)) {
		ul_writelog(UL_LOG_DEBUG, "%s not url", url);
		return -1;
	}

	ul_get_static_part(url, staticpart_url);
	ul_parse_url(staticpart_url, domain, port, dir);
	p = strrchr(dir, '/');
	if (p==NULL)
	{
		return -1;
	}

	*(p + 1) = '\0';

	linknum = wget_exrellink(page, pagesize, link_info, num);

	for (int i = 0; i < linknum; i++) {
		if (!is_pathurl(link_info[i].url)) {
			ul_writelog(UL_LOG_DEBUG, "is_pathurl failed %s", link_info[i].url);
			memset(&link_info[i], 0, sizeof(link_info[i]));
			continue;
			//  assert(is_pathurl(link_info[i].url));
		}
		//absolute path
		if (is_abs_path(link_info[i].url)) {
			//too long path
			if (strlen(link_info[i].url) >= UL_MAX_PATH_LEN) {
				ul_writelog(UL_LOG_DEBUG, "too long path:%s", link_info[i].url);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}
			//single path
			strcpy(link_info[i].path, link_info[i].url);
			if (!ul_single_path(link_info[i].path)) {
				ul_writelog(UL_LOG_DEBUG, "error path:%s", link_info[i].path);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}
			//normalize path
			ul_normalize_path(link_info[i].path);
			//domain and port   
			strcpy(link_info[i].domain, domain);
			strcpy(link_info[i].port, port);

			//relative path
		} else if (is_rel_path(link_info[i].url)) {
			//too long path
			if (strlen(link_info[i].url) + strlen(dir) >= UL_MAX_PATH_LEN) {
				ul_writelog(UL_LOG_DEBUG, "too long path:%s%s", dir, link_info[i].url);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}
			//script
			if (strncasecmp(link_info[i].url, "javascript:", 11) == 0) {
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}
			//single path
			strcpy(link_info[i].path, dir);
			strcat(link_info[i].path, link_info[i].url);
			if (!ul_single_path(link_info[i].path)) {
				ul_writelog(UL_LOG_DEBUG, "error path:%s", link_info[i].path);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}
			//normalize path
			ul_normalize_path(link_info[i].path);

			//domain and port
			strcpy(link_info[i].domain, domain);
			strcpy(link_info[i].port, port);

			//url
		} else if (is_url(link_info[i].url)) {
			if (0 ==
				ul_parse_url(link_info[i].url, link_info[i].domain, link_info[i].port,
							 link_info[i].path)) {
				ul_writelog(UL_LOG_DEBUG, "ul_parse_urll error, url=%s", link_info[i].url);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}
			ul_tolowerstr_singlebuf((unsigned char *) link_info[i].domain);
			//single path
			if (!ul_single_path(link_info[i].path)) {
				ul_writelog(UL_LOG_DEBUG, "error path:%s", link_info[i].path);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}
			//normalize path
			ul_normalize_path(link_info[i].path);

		} else {
			ul_writelog(UL_LOG_DEBUG, "parse error");
			return -1;
			//assert(0);
		}

	}
	return linknum;				//add by liumh
}



//#include "wget.h"
#ifndef HTML_PARSE_H
#define HTML_PARSE_H

struct attr_pair {
	char *name;					/* attribute name */
	char *value;				/* attribute value */

	/* Needed for URL conversion; the places where the value begins and
	 * ends, including the quotes and everything. */
	const char *value_raw_beginning;
	int value_raw_size;

	/* Used internally by map_html_tags. */
	int name_pool_index, value_pool_index;
};

struct taginfo {
	char *name;					/* tag name */
	int end_tag_p;				/* whether this is an end-tag */
	int nattrs;					/* number of attributes */
	struct attr_pair *attrs;	/* attributes */

	const char *start_position;	/* start position of tag */
	const char *end_position;	/* end position of tag */
};

int map_html_tags(const char *text, int size, const char **allowed_tag_names,
				  const char **allowed_attribute_names, link_info_t * link_info, int num);

#endif /* HTML_PARSE_H */

struct pool {
	char *contents;				/* pointer to the contents. */
	int size;					/* size of the pool. */
	int index;					/* next unoccupied position in
								 * contents. */

	int alloca_p;				/* whether contents was allocated
								 * using alloca(). */
	char *orig_contents;		/* orig_contents, allocated by
								 * alloca().  this is used by
								 * POOL_FREE to restore the pool to
								 * the "initial" state. */
	int orig_size;
};

#define xmalloc  malloc
#define xrealloc realloc
#define xstrdup  strdup
#define xfree    free

#define ALLOCA_ARRAY(type, len) ((type *) alloca ((len) * sizeof (type)))
#define XREALLOC_ARRAY(ptr, type, len)					\
	((void) (ptr = (type *) xrealloc (ptr, (len) * sizeof (type))))

#define DO_REALLOC_FROM_ALLOCA(basevar, sizevar, needed_size, allocap, type) do	\
{										\
	/* Avoid side-effectualness.  */						\
	long do_realloc_needed_size = (needed_size);					\
	long do_realloc_newsize = (sizevar);						\
	while (do_realloc_newsize < do_realloc_needed_size) {				\
		do_realloc_newsize <<= 1;							\
		if (do_realloc_newsize < 16)						\
		do_realloc_newsize = 16;							\
	}										\
	if (do_realloc_newsize != (sizevar))						\
	{										\
		if (!allocap)								\
		XREALLOC_ARRAY (basevar, type, do_realloc_newsize);			\
		else									\
		{									\
			void *drfa_new_basevar = xmalloc (do_realloc_newsize);		\
			memcpy (drfa_new_basevar, basevar, (sizevar));			\
			(basevar) = (type *)drfa_new_basevar;						\
			allocap = 0;								\
		}									\
		(sizevar) = do_realloc_newsize;						\
	}										\
} while (0)


/* Initialize the pool to hold INITIAL_SIZE bytes of storage. */

#define POOL_INIT(pool, initial_size) do {		\
	(pool).size = (initial_size);				\
	(pool).contents = ALLOCA_ARRAY (char, (pool).size);	\
	(pool).index = 0;					\
	(pool).alloca_p = 1;					\
	(pool).orig_contents = (pool).contents;		\
	(pool).orig_size = (pool).size;			\
} while (0)

/* Grow the pool to accomodate at least SIZE new bytes.  If the pool
   already has room to accomodate SIZE bytes of data, this is a no-op.  */

#define POOL_GROW(pool, increase) do {					\
	int PG_newsize = (pool).index + increase;				\
	DO_REALLOC_FROM_ALLOCA ((pool).contents, (pool).size, PG_newsize,	\
			(pool).alloca_p, char);			\
} while (0)

/* Append text in the range [beg, end) to POOL.  No zero-termination
   is done.  */

#define POOL_APPEND(pool, beg, end) do {			\
	const char *PA_beg = beg;					\
	int PA_size = end - PA_beg;					\
	POOL_GROW (pool, PA_size);					\
	memcpy ((pool).contents + (pool).index, PA_beg, PA_size);	\
	(pool).index += PA_size;					\
} while (0)

/* The same as the above, but with zero termination. */

#define POOL_APPEND_ZT(pool, beg, end) do {			\
	const char *PA_beg = beg;					\
	int PA_size = end - PA_beg;					\
	POOL_GROW (pool, PA_size + 1);				\
	memcpy ((pool).contents + (pool).index, PA_beg, PA_size);	\
	(pool).contents[(pool).index + PA_size] = '\0';		\
	(pool).index += PA_size + 1;					\
} while (0)

/* Forget old pool contents.  The allocated memory is not freed. */
#define POOL_REWIND(pool) pool.index = 0

/* Free heap-allocated memory for contents of POOL.  This calls
   xfree() if the memory was allocated through malloc.  It also
   restores `contents' and `size' to their original, pre-malloc
   values.  That way after POOL_FREE, the pool is fully usable, just
   as if it were freshly initialized with POOL_INIT.  */

#define POOL_FREE(pool) do {			\
	if (!(pool).alloca_p)				\
	xfree ((pool).contents);			\
	(pool).contents = (pool).orig_contents;	\
	(pool).size = (pool).orig_size;		\
	(pool).index = 0;				\
	(pool).alloca_p = 1;				\
} while (0)


#define AP_DOWNCASE		1
#define AP_PROCESS_ENTITIES	2
#define AP_SKIP_BLANKS		4

static void convert_and_copy(struct pool *pool, const char *beg, const char *end, int flags)
{
	int old_index = pool->index;
	int size;

	/* First, skip blanks if required.  We must do this before entities
	 * are processed, so that blanks can still be inserted as, for
	 * instance, `&#32;'.  */
	if (flags & AP_SKIP_BLANKS) {
		while (beg < end && ISSPACE(*beg))
			++beg;
		while (end > beg && ISSPACE(end[-1]))
			--end;
	}
	size = end - beg;

	if (flags & AP_PROCESS_ENTITIES) {
		/* Stack-allocate a copy of text, process entities and copy it
		 * to the pool.  */
		char *local_copy = (char *) alloca(size + 1);
		const char *from = beg;
		char *to = local_copy;

		while (from < end) {
			if (*from != '&')
				*to++ = *from++;
			else {
				const char *save = from;
				int remain;

				if (++from == end)
					goto lose;
				remain = end - from;

				if (*from == '#') {
					int numeric;
					++from;
					if (from == end || !ISDIGIT(*from))
						goto lose;
					for (numeric = 0; from < end && ISDIGIT(*from); from++)
						numeric = 10 * numeric + (*from) - '0';
					if (from < end && ISALPHA(*from))
						goto lose;
					numeric &= 0xff;
					*to++ = numeric;
				}
#define FROB(x) (remain >= (int)(sizeof (x) - 1)			\
		&& !memcmp (from, x, sizeof (x) - 1)		\
		&& (*(from + sizeof (x) - 1) == ';'		\
			|| remain == sizeof (x) - 1		\
			|| !ISALNUM (*(from + sizeof (x) - 1))))
				else if (FROB("lt"))
					*to++ = '<', from += 2;
				else if (FROB("gt"))
					*to++ = '>', from += 2;
				else if (FROB("amp"))
					*to++ = '&', from += 3;
				else if (FROB("quot"))
					*to++ = '\"', from += 4;
				/* We don't implement the proposed "Added Latin 1"
				 * entities (except for nbsp), because it is unnecessary
				 * in the context of Wget, and would require hashing to
				 * work efficiently.  */
				else if (FROB("nbsp"))
					*to++ = 160, from += 4;
				else
					goto lose;
#undef FROB
				/* If the entity was followed by `;', we step over the
				 * `;'.  Otherwise, it was followed by either a
				 * non-alphanumeric or EOB, in which case we do nothing.    */
				if (from < end && *from == ';')
					++from;
				continue;

			  lose:
				/* This was not an entity after all.  Back out.  */
				from = save;
				*to++ = *from++;
			}
		}
		*to++ = '\0';
		POOL_APPEND(*pool, local_copy, to);
	} else {
		/* Just copy the text to the pool.  */
		POOL_APPEND_ZT(*pool, beg, end);
	}

	if (flags & AP_DOWNCASE) {
		char *p = pool->contents + old_index;
		for (; *p; p++)
			*p = TOLOWER(*p);
	}
}

/* Check whether the contents of [POS, POS+LENGTH) match any of the
   strings in the ARRAY.  */
static int array_allowed(const char **array, const char *beg, const char *end)
{
	int length = end - beg;
	if (array) {
		for (; *array; array++)
			if (length >= (int) strlen(*array)
				&& !strncasecmp(*array, beg, length))
				break;
		if (!*array)
			return 0;
	}
	return 1;
}

/* RFC1866: name [of attribute or tag] consists of letters, digits,
   periods, or hyphens.  We also allow _, for compatibility with
   brain-damaged generators.  */
#define NAME_CHAR_P(x) (ISALNUM (x) || (x) == '.' || (x) == '-' || (x) == '_')

/* States while advancing through comments. */
#define AC_S_DONE	0
#define AC_S_BACKOUT	1
#define AC_S_BANG	2
#define AC_S_DEFAULT	3
#define AC_S_DCLNAME	4
#define AC_S_DASH1	5
#define AC_S_DASH2	6
#define AC_S_COMMENT	7
#define AC_S_DASH3	8
#define AC_S_DASH4	9
#define AC_S_QUOTE1	10
#define AC_S_IN_QUOTE	11
#define AC_S_QUOTE2	12

#ifdef STANDALONE
static int comment_backout_count;
#endif

static const char *advance_declaration(const char *beg, const char *end)
{
	const char *p = beg;
	char quote_char = '\0';		/* shut up, gcc! */
	char ch;
	int state = AC_S_BANG;

	if (beg == end)
		return beg;
	ch = *p++;

	/* It looked like a good idea to write this as a state machine, but
	 * now I wonder...  */

	while (state != AC_S_DONE && state != AC_S_BACKOUT) {
		if (p == end)
			state = AC_S_BACKOUT;
		switch (state) {
		case AC_S_DONE:
		case AC_S_BACKOUT:
			break;
		case AC_S_BANG:
			if (ch == '!') {
				ch = *p++;
				state = AC_S_DEFAULT;
			} else
				state = AC_S_BACKOUT;
			break;
		case AC_S_DEFAULT:
			switch (ch) {
			case '-':
				state = AC_S_DASH1;
				break;
			case ' ':
			case '\t':
			case '\r':
			case '\n':
				ch = *p++;
				break;
			case '>':
				state = AC_S_DONE;
				break;
			case '\'':
			case '\"':
				state = AC_S_QUOTE1;
				break;
			default:
				if (NAME_CHAR_P(ch))
					state = AC_S_DCLNAME;
				else
					state = AC_S_BACKOUT;
				break;
			}
			break;
		case AC_S_DCLNAME:
			if (NAME_CHAR_P(ch))
				ch = *p++;
			else if (ch == '-')
				state = AC_S_DASH1;
			else
				state = AC_S_DEFAULT;
			break;
		case AC_S_QUOTE1:
			/* We must use 0x22 because broken assert macros choke on
			 * '"' and '\"'.  */
	//		assert(ch == '\'' || ch == 0x22);
			quote_char = ch;	/* cheating -- I really don't feel like
								 * introducing more different states for
								 * different quote characters. */
			ch = *p++;
			state = AC_S_IN_QUOTE;
			break;
		case AC_S_IN_QUOTE:
			if (ch == quote_char)
				state = AC_S_QUOTE2;
			else
				ch = *p++;
			break;
		case AC_S_QUOTE2:
			//assert(ch == quote_char);
			ch = *p++;
			state = AC_S_DEFAULT;
			break;
		case AC_S_DASH1:
			//assert(ch == '-');
			ch = *p++;
			state = AC_S_DASH2;
			break;
		case AC_S_DASH2:
			switch (ch) {
			case '-':
				ch = *p++;
				state = AC_S_COMMENT;
				break;
			default:
				state = AC_S_BACKOUT;
			}
			break;
		case AC_S_COMMENT:
			switch (ch) {
			case '-':
				state = AC_S_DASH3;
				break;
			default:
				ch = *p++;
				break;
			}
			break;
		case AC_S_DASH3:
			//assert(ch == '-');
			ch = *p++;
			state = AC_S_DASH4;
			break;
		case AC_S_DASH4:
			switch (ch) {
			case '-':
				ch = *p++;
				state = AC_S_DEFAULT;
				break;
			default:
				state = AC_S_COMMENT;
				break;
			}
			break;
		}
	}

	if (state == AC_S_BACKOUT) {
#ifdef STANDALONE
		++comment_backout_count;
#endif
		return beg + 1;
	}
	return p;
}

/* Advance P (a char pointer), with the explicit intent of being able
   to read the next character.  If this is not possible, go to finish.  */

#define ADVANCE(p) do {				\
	++p;						\
	if (p >= end)					\
	goto finish;				\
} while (0)

/* Skip whitespace, if any. */

#define SKIP_WS(p) do {				\
	while (ISSPACE (*p)) {			\
		ADVANCE (p);				\
	}						\
} while (0)

/* Skip non-whitespace, if any. */

#define SKIP_NON_WS(p) do {			\
	while (!ISSPACE (*p)) {			\
		ADVANCE (p);				\
	}						\
} while (0)

//added by xhy in 12/9
int get_url_in_content(char *attr_value)
{
	char buffer[512];
	char *p;
	if (strlen(attr_value) >= 512)
		return 0;
	strcpy(buffer, attr_value);
	ul_tolowerstr_singlebuf((unsigned char *) buffer);
	p = strstr(buffer, "url=");
	if (p == NULL)
		return 0;
	p += 4;
	p = attr_value + (p - buffer);

	//move url to attr_value
	while (*p != '"' && *p != '\'' && *p != ' ' && *p != '\0')
		*attr_value++ = *p++;
	*attr_value = '\0';
	return 1;

}

//add end


int insert_taginfo(struct taginfo *tag, link_info_t * link_info)
{
	int i;
	/*
	 * fprintf(stderr,"***************************%d*******************************\n%s%s",
	 * tag->nattrs,tag->end_tag_p ? "/" : "", tag->name);
	 * for (i = 0; i < tag->nattrs; i++)
	 * printf (" %s=%s", tag->attrs[i].name, tag->attrs[i].value);
	 * fprintf(stderr,"\n----------------------------------------------------------\n");
	 */


	for (i = 0; i < tag->nattrs; i++) {
		if ((!strcasecmp(tag->attrs[i].name, "href")) || (!strcasecmp(tag->attrs[i].name, "src")))
			break;
		//added by xhy in 12/9
		if (!strcasecmp(tag->attrs[i].name, "content") && get_url_in_content(tag->attrs[i].value))
			break;
		//add end
		/*  else
		 * {
		 * printf (" not interest tag extracted:%s=%s", tag->attrs[i].name, tag->attrs[i].value);
		 * }
		 */
	}
	if (i < tag->nattrs) {
		int size = strlen(tag->attrs[i].value);
		size = (size > UL_MAX_URL_LEN - 1) ? (UL_MAX_URL_LEN - 1) : size;
		strncpy(link_info->url, tag->attrs[i].value, size);
		link_info->url[size] = 0;
		return 0;
	}
	return -1;
}

/*void GetAnchorText1_8(const char *buf, char *text, int bufsize)
{
	int errflag = 0, i = 0;
	for (; bufsize && *buf != '>'; ++buf, --bufsize);
	if (!bufsize)
		return;
	buf++;
	bufsize--;

	while (1) {
		if (*buf != '<') {
			if ((*buf != '\r') && (*buf != '\n')) {
				text[i++] = *buf;
				buf++;
			} else {
				text[i++] = ' ';
				buf++;
			}
		} else {
			for (; bufsize && *buf != '>'; ++buf, --bufsize) {
				if (*buf == '/' && (*(buf + 1) == 'a' || *(buf + 1) == 'A')) {
					goto end;
				}
			}
			if (!bufsize) {
				errflag = 1;
				break;
			}
			if (*buf == '>') {
				buf++;
				--bufsize;
			}
		}
		if (i >= UL_MAX_TEXT_LEN) {
			errflag = 1;
			break;
		}
	}
  end:
	if (errflag) {
		text[i] = 0;
		ul_writelog(UL_LOG_DEBUG, "GetAnchorText1_8 err %s", text);
		memset(text, 0, UL_MAX_TEXT_LEN);
	} else {
		text[i] = 0;
		//      ul_writelog(UL_LOG_DEBUG,"GetAnchorText1_8 success %s",text);
	}
}*/

int map_html_tags(const char *text, int size, const char **allowed_tag_names,
			  const char **allowed_attribute_names, link_info_t * link_info, int num)
{
	const char *p = text;
	const char *end = text + size;

	int attr_pair_count = 100;
	int attr_pair_alloca_p = 1;
	struct attr_pair *pairs = ALLOCA_ARRAY(struct attr_pair, attr_pair_count);
	struct pool pool;
	int count = 0;

	if (!size)
		return count;

	POOL_INIT(pool, 256);

	{
		int nattrs, end_tag;
		const char *tag_name_begin, *tag_name_end;
		const char *tag_start_position;
		int uninteresting_tag;

	  look_for_tag:
		POOL_REWIND(pool);

		nattrs = 0;
		end_tag = 0;

		/* Find beginning of tag.  We use memchr() instead of the usual
		 * looping with ADVANCE() for speed. */
		p = (char *) memchr(p, '<', end - p);
		if (!p)
			goto finish;

		tag_start_position = p;
		ADVANCE(p);

		/* Establish the type of the tag (start-tag, end-tag or
		 * declaration).  */
		if (*p == '!') {
			/* This is an SGML declaration -- just skip it.  */
			p = advance_declaration(p, end);
			if (p == end)
				goto finish;
			goto look_for_tag;
		} else if (*p == '/') {
			end_tag = 1;
			ADVANCE(p);
		}
		tag_name_begin = p;
		while (NAME_CHAR_P(*p))
			ADVANCE(p);
		if (p == tag_name_begin)
			goto look_for_tag;
		tag_name_end = p;
		SKIP_WS(p);
		if (end_tag && *p != '>')
			goto backout_tag;

		if (!array_allowed(allowed_tag_names, tag_name_begin, tag_name_end))
			/* We can't just say "goto look_for_tag" here because we need
			 * the loop below to properly advance over the tag's attributes.  */
			uninteresting_tag = 1;
		else {
			uninteresting_tag = 0;
			convert_and_copy(&pool, tag_name_begin, tag_name_end, AP_DOWNCASE);
		}

		/* Find the attributes. */
		while (1) {
			const char *attr_name_begin, *attr_name_end;
			const char *attr_value_begin, *attr_value_end;
			const char *attr_raw_value_begin, *attr_raw_value_end;
			int operation = AP_DOWNCASE;	/* stupid compiler. */

			SKIP_WS(p);

			if (*p == '/') {
				/* A slash at this point means the tag is about to be
				 * closed.  This is legal in XML and has been popularized
				 * in HTML via XHTML.  */
				/* <foo a=b c=d /> */
				/*              ^  */
				ADVANCE(p);
				SKIP_WS(p);
				if (*p != '>')
					goto backout_tag;
			}

			/* Check for end of tag definition. */
			if (*p == '>')
				break;

			/* Establish bounds of attribute name. */
			attr_name_begin = p;	/* <foo bar ...> */
			/*      ^        */
			while (NAME_CHAR_P(*p))
				ADVANCE(p);
			attr_name_end = p;	/* <foo bar ...> */
			/*         ^     */
			if (attr_name_begin == attr_name_end)
				goto backout_tag;

			/* Establish bounds of attribute value. */
			SKIP_WS(p);
			if (NAME_CHAR_P(*p) || *p == '/' || *p == '>') {
				/* Minimized attribute syntax allows `=' to be omitted.
				 * For example, <UL COMPACT> is a valid shorthand for <UL
				 * COMPACT="compact">.  Even if such attributes are not
				 * useful to Wget, we need to support them, so that the
				 * tags containing them can be parsed correctly. */
				attr_raw_value_begin = attr_value_begin = attr_name_begin;
				attr_raw_value_end = attr_value_end = attr_name_end;
			} else if (*p == '=') {
				ADVANCE(p);
				SKIP_WS(p);
				if (*p == '\"' || *p == '\'') {
					int newline_seen = 0;
					char quote_char = *p;
					attr_raw_value_begin = p;
					ADVANCE(p);
					attr_value_begin = p;	/* <foo bar="baz"> */
					/*           ^     */
					while (*p != quote_char) {
						if (!newline_seen && *p == '\n') {
							/* If a newline is seen within the quotes, it
							 * is most likely that someone forgot to close
							 * the quote.  In that case, we back out to
							 * the value beginning, and terminate the tag
							 * at either `>' or the delimiter, whichever
							 * comes first.  Such a tag terminated at `>'
							 * is discarded.  */
							p = attr_value_begin;
							newline_seen = 1;
							continue;
						} else if (newline_seen && *p == '>')
							break;
						ADVANCE(p);
					}
					attr_value_end = p;	/* <foo bar="baz"> */
					/*              ^  */
					if (*p == quote_char)
						ADVANCE(p);
					else
						goto look_for_tag;
					attr_raw_value_end = p;	/* <foo bar="baz"> */
					/*               ^ */
					/* The AP_SKIP_BLANKS part is not entirely correct,
					 * because we don't want to skip blanks for all the
					 * attribute values.  */
					operation = AP_PROCESS_ENTITIES | AP_SKIP_BLANKS;
				} else {
					attr_value_begin = p;	/* <foo bar=baz> */
					/*          ^    */
					/* According to SGML, a name token should consist only
					 * of alphanumerics, . and -.  However, this is often
					 * violated by, for instance, `%' in `width=75%'.
					 * We'll be liberal and allow just about anything as
					 * an attribute value.  */
					while (!ISSPACE(*p) && *p != '>')
						ADVANCE(p);
					attr_value_end = p;	/* <foo bar=baz qux=quix> */
					/*             ^          */
					if (attr_value_begin == attr_value_end)
						/* <foo bar=> */
						/*          ^ */
						goto backout_tag;
					attr_raw_value_begin = attr_value_begin;
					attr_raw_value_end = attr_value_end;
					operation = AP_PROCESS_ENTITIES;
				}
			} else {
				/* We skipped the whitespace and found something that is
				 * neither `=' nor the beginning of the next attribute's
				 * name.  Back out.  */
				goto backout_tag;	/* <foo bar [... */
				/*          ^    */
			}

			/* If we're not interested in the tag, don't bother with any
			 * of the attributes.  */
			if (uninteresting_tag)
				continue;

			/* If we aren't interested in the attribute, skip it.  We
			 * cannot do this test any sooner, because our text pointer
			 * needs to correctly advance over the attribute.  */
			if (allowed_attribute_names
				&& !array_allowed(allowed_attribute_names, attr_name_begin, attr_name_end))
				continue;

			DO_REALLOC_FROM_ALLOCA(pairs, attr_pair_count, nattrs + 1,
								   attr_pair_alloca_p, struct attr_pair);

			pairs[nattrs].name_pool_index = pool.index;
			convert_and_copy(&pool, attr_name_begin, attr_name_end, AP_DOWNCASE);

			pairs[nattrs].value_pool_index = pool.index;
			convert_and_copy(&pool, attr_value_begin, attr_value_end, operation);
			pairs[nattrs].value_raw_beginning = attr_raw_value_begin;
			pairs[nattrs].value_raw_size = (attr_raw_value_end - attr_raw_value_begin);
			++nattrs;
			/* add by chen jingkai */
			if (nattrs > 99)
				nattrs--;
		}

		if (uninteresting_tag) {
			ADVANCE(p);
			goto look_for_tag;
		}

		/* By now, we have a valid tag with a name and zero or more
		 * attributes.  Fill in the data and call the mapper function.  */
		{
			int i;
			struct taginfo taginfo;

			taginfo.name = pool.contents;
			taginfo.end_tag_p = end_tag;
			taginfo.nattrs = nattrs;
			/* We fill in the char pointers only now, when pool can no
			 * longer get realloc'ed.  If we did that above, we could get
			 * hosed by reallocation.  Obviously, after this point, the pool
			 * may no longer be grown.  */
			for (i = 0; i < nattrs; i++) {
				pairs[i].name = pool.contents + pairs[i].name_pool_index;
				pairs[i].value = pool.contents + pairs[i].value_pool_index;
			}
			taginfo.attrs = pairs;
			taginfo.start_position = tag_start_position;
			taginfo.end_position = p + 1;
			/* Ta-dam! */
			//      (*mapfun) (&taginfo, closure);
			if (!insert_taginfo(&taginfo, &link_info[count])) {	//tag->end_tag_p ? "/" : "", tag->name
				link_info[count].text[0] = '\0';
				if (!taginfo.end_tag_p && !strcasecmp(taginfo.name, "a")) {
					GetAnchorText(p, link_info[count].text, end - p);
				}
				count++;
				if (count >= num)
					goto finish;
			}
			ADVANCE(p);
		}
		goto look_for_tag;

	  backout_tag:

		/* The tag wasn't really a tag.  Treat its contents as ordinary
		 * data characters. */
		p = tag_start_position + 1;
		goto look_for_tag;
	}

  finish:
	POOL_FREE(pool);
	if (!attr_pair_alloca_p)
		xfree(pairs);
	return count;
}

#undef ADVANCE
#undef SKIP_WS
#undef SKIP_NON_WS

int wget1_8_exrellink(char *page, int pagesize, link_info_t * link_info, int num)
{
	int count = 0;
	static const char *tag[] = {
		"frame",
		"iframe",
		"a",
		"area",
		"link",
		"meta",
		"embed",
		NULL
	};
	static const char *attr[] = {
		"src",
		"href",
		"content",
		NULL
	};
	count = map_html_tags(page, pagesize, tag, attr, link_info, num);
	return count;
}

int wget1_8_exlink(char *url, char *page, int pagesize, link_info_t * link_info, int num)
{
	int linknum;
	int ret;
	char domain[UL_MAX_SITE_LEN], port[UL_MAX_PORT_LEN], dir[UL_MAX_PATH_LEN];
	char staticpart_url[UL_MAX_URL_LEN];
	char *p;

	//  assert(is_url(url));
	if (!is_url(url)) {
		ul_writelog(UL_LOG_DEBUG, "%s is not url", url);
		return -1;
	}

	ul_get_static_part(url, staticpart_url);
	ret = ul_parse_url(staticpart_url, domain, port, dir);
	if (ret == 0)
	{
		ul_writelog(UL_LOG_DEBUG, "%s invalid url",url);
		return -1;
	}
	//  fprintf(stderr,"<<<url=%s %s%s%s>>>\n",url,domain, port, dir);
	p = strrchr(dir, '/');
	*(p + 1) = '\0';

	linknum = wget1_8_exrellink(page, pagesize, link_info, num);
	for (int i = 0; i < linknum; i++) {
		if (!is_pathurl(link_info[i].url)) {
			ul_writelog(UL_LOG_DEBUG, "is_pathurl failed %s", link_info[i].url);
			link_info[i].domain[0] = 0;
			continue;
			//assert(is_pathurl(link_info[i].url));
		}
		//absolute path
		if (is_abs_path(link_info[i].url)) {
			//too long path
			if (strlen(link_info[i].url) >= UL_MAX_PATH_LEN) {
				ul_writelog(UL_LOG_DEBUG, "too long path:%s", link_info[i].url);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}
			//single path
			strcpy(link_info[i].path, link_info[i].url);
			if (!ul_single_path(link_info[i].path)) {
				ul_writelog(UL_LOG_DEBUG, "error path:%s", link_info[i].path);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}
			//normalize path
			ul_normalize_path(link_info[i].path);
			//domain and port   
			strcpy(link_info[i].domain, domain);
			strcpy(link_info[i].port, port);

			//relative path
		} else if (is_rel_path(link_info[i].url)) {
			//too long path
			if (strlen(link_info[i].url) + strlen(dir) >= UL_MAX_PATH_LEN) {
				ul_writelog(UL_LOG_DEBUG, "too long path:%s%s", dir, link_info[i].url);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}
			//script
			if (strncasecmp(link_info[i].url, "javascript:", 11) == 0) {
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}
			//single path
			strcpy(link_info[i].path, dir);
			strcat(link_info[i].path, link_info[i].url);
			if (!ul_single_path(link_info[i].path)) {
				ul_writelog(UL_LOG_DEBUG, "error path:%s", link_info[i].path);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}
			//normalize path
			ul_normalize_path(link_info[i].path);

			//domain and port
			strcpy(link_info[i].domain, domain);
			strcpy(link_info[i].port, port);

			//url
		} else if (is_url(link_info[i].url)) {
			if (0 ==
				ul_parse_url(link_info[i].url, link_info[i].domain, link_info[i].port,
							 link_info[i].path)) {
				ul_writelog(UL_LOG_DEBUG, "ul_parse_urll error, url=%s", link_info[i].url);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}
			ul_tolowerstr_singlebuf((unsigned char *) link_info[i].domain);
			//single path
			if (!ul_single_path(link_info[i].path)) {
				ul_writelog(UL_LOG_DEBUG, "error path:%s", link_info[i].path);
				memset(&link_info[i], 0, sizeof(link_info[i]));
				continue;
			}
			//normalize path
			ul_normalize_path(link_info[i].path);

		} else {
			ul_writelog(UL_LOG_DEBUG, "parse error!\n");
			return -1;
			//assert(0);
		}

	}
	return linknum;				//add by liumh

}

int extract_link(char *url, char *page, int pagesize, link_info_t * link_info, int num, int method)
{
	if (url == NULL || page == NULL || link_info == NULL)
	{
		ul_writelog(UL_LOG_DEBUG, "invalid argument at extract_link");
		return -1;
	}

	if (pagesize > MAX_PAGE_SIZE  || pagesize <=0 || num <=0) 
	{
		ul_writelog(UL_LOG_DEBUG, "invalid pagesize:%d or num:%d",pagesize, num);
		return -1;
	}

	int count = 0;
	if (method == UL_METHOD_IXINTUI)
		count = exlink(url, page, link_info, num);
	else if (method == UL_METHOD_WGET16)
		count = wget_exlink(url, page, pagesize, link_info, num);
	else if (method == UL_METHOD_WGET18)
		count = wget1_8_exlink(url, page, pagesize, link_info, num);
	else
		return -1;
	return count;
}

int ul_absolute_url(char *url, char *relurl, char *absurl)
{

	char domain[UL_MAX_SITE_LEN], port[UL_MAX_PORT_LEN], dir[UL_MAX_PATH_LEN];
	char reldomain[UL_MAX_SITE_LEN], relport[UL_MAX_PORT_LEN], relpath[UL_MAX_PATH_LEN];
	char *p;
	char *where = "absolute_url";

	if (url==NULL || relurl==NULL || absurl==NULL)
	{
		ul_writelog(UL_LOG_DEBUG,"param error!");
		return 0;
	}

	absurl[0] = '\0';

	if (!ul_parse_url(url, domain, port, dir)) {
		ul_writelog(UL_LOG_DEBUG, "%s:error url:%s", where, url);
		return 0;
	}

	p = strrchr(dir, '/');
	if (p == NULL) {
		ul_writelog(UL_LOG_DEBUG, "%s:invalid url:%s", where, url);
		return 0;
	}
	*(p + 1) = '\0';

	if (!is_pathurl(relurl)) {
		ul_writelog(UL_LOG_DEBUG, "%s:error relative url:%s", where, relurl);
		return 0;
	}
	if (is_abs_path(relurl)) {
		//too long path
		if (strlen(relurl) >= UL_MAX_PATH_LEN) {
			ul_writelog(UL_LOG_DEBUG, "%s:too long path:%s", where, relurl);
			return 0;
		}
		//single path
		strcpy(relpath, relurl);
		//domain and port   
		strcpy(reldomain, domain);
		strcpy(relport, port);

		//relative path
	} else if (is_rel_path(relurl)) {
		//too long path
		if (strlen(relurl) + strlen(dir) >= UL_MAX_PATH_LEN) {
			ul_writelog(UL_LOG_DEBUG, "too long path:%s%s", dir, relurl);
			return 0;
		}
		//script
		if (strncasecmp(relurl, "javascript:", 11) == 0) {
			ul_writelog(UL_LOG_DEBUG, "javascript:%s%s", dir, relurl);
			return 0;
		}
		//single path
		strcpy(relpath, dir);
		strcat(relpath, relurl);

		//domain and port
		strcpy(reldomain, domain);
		strcpy(relport, port);

		//url
	} else if (is_url(relurl)) {
		if (0 == ul_parse_url(relurl, reldomain, relport, relpath)) {
			ul_writelog(UL_LOG_DEBUG, "ul_parse_url error, url=%s", relurl);
			return 0;
		}
		ul_tolowerstr_singlebuf((unsigned char *) reldomain);

	} else {
		ul_writelog(UL_LOG_DEBUG, "parse error!\n");
		return 0;
		//assert(0);
	}

	//single_path
	if (!ul_single_path(relpath)) {
		ul_writelog(UL_LOG_DEBUG, "error path:%s", relpath);
		return 0;
	}
	//normalize path
	ul_normalize_path(relpath);
	if (relport[0] == '\0')
		sprintf(absurl, "%s%s", reldomain, relpath);
	else
		sprintf(absurl, "%s:%s%s", reldomain, relport, relpath);
	return 1;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
