
// {{{ Include file list here 
/* system head file include here  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <assert.h>

/* utility head file include here */
#include <ul_url.h>
#include <ul_char.h>

/* useful-lib head file include here */

/* local head file include here */
#include "ul_htmltag.h"

// }}}

// {{{ Sample Function Head 
/********************************************************************
 * type        : Inner 
 * function    :
 * argu-in     :
 * argu-out    : 
 * return      :
 * notice      :
 ********************************************************************/
// }}}

// {{{ ul_jumpto_tagend
/********************************************************************
 * type        : Inner 
 * function    : get the end position (just after the >) or at the end
 *             : of the string
 * argu-in     : char* remainder
 * argu-in     : int remlen : the buffer length
 * argu-out    : 
 * return      : the new positioin, NULL if failed
 * notice      :
 ********************************************************************/
char *ul_jumpto_tagend(char *remainder)
{
	//assert(remainder!= NULL);
	if (remainder == NULL)
		return NULL;

	while ((*remainder != '\0') && (*remainder != '>')) {
		remainder++;
	}
	// got the end
	if (*remainder == '>') {
		return remainder + 1;
	} else {
		//assert(*remainder== '\0');
		return remainder;
	}
}

// }}}

// {{{ ul_get_tagname
/***********************************************************************
 * type     : External 
 * function : get a tag's name  from html
 * argu-in  : char * piece : tagbegin
 * argu-out : char * tagbuf: the buffer for tag
 *          : int size     : th tag buffer size
 *          : char ** endpiece  : the reminder string after get the tagname
 * return   : 1 if success,
 *          : <0 if failed and endpiece is undefined
 *          : -1 if input arguments are illegal
 *          : -2 if no tagname exists  here.
 *          : -3 if the tagname is too long
 *          : 
 ***********************************************************************/
int ul_get_tagname(char *piece, char *tagbuf, int size, char **pend)
{
	char *tmp=NULL;
	int tagsize;
	//char *tagnamehead=NULL;

	//assert(piece && tagbuf && pend);
	//assert(size >0);
	if ((piece == NULL) || (tagbuf == NULL) || (pend == NULL) || (size <= 0)) {
		return -1;
	}

	tmp = piece;

	// skip the leading space, not include '\0'
	while ((*tmp != '\0') && (ul_isspace(*tmp))) {
		tmp++;
	}

	if (*tmp == 0)
		return -1;

	//tagnamehead = tmp;

	// find the end of the tag name 
	while ((*tmp != 0) && !ul_isspace(*tmp) && (*tmp != '>'))
		tmp++;

	if (*tmp == 0)
		return -1;

	tagsize = tmp - piece;

	//assert(tagsize >=0);
	if (tagsize < 0)
		return -1;

	if (tagsize == 0) {
		// no tagname valid
		return -2;
	}

	if (tagsize > size - 1) {
		return -3;
	}

	memcpy(tagbuf, piece, tagsize);
	tagbuf[tagsize] = '\0';
	size = tagsize;
	ul_tolowerstr_singlebuf((unsigned char *) tagbuf);
	*pend = tmp;
	return 1;
}

// }}}

// {{{ ul_get_nvpair
/***********************************************************************
 * type     : External
 * function : get the name and value pair in the tag 
 * argu-in  : char* piece    : the tag piece
 * argu-io  : ul_nvpair_t * pnvpair : the ul_nvpair_t, include name, value and
 *          :     status
 *          : char** pend    : the remainder string after
 * return   : 1 if get a name-value
 *          : 0 if there is no name-value pair 
 *          : <0 indicate error happened.
 *          : -1 : argu-in is not correct
 *          : -2 : miss a name,such as "<tagname =value>"
 ***********************************************************************/
int ul_get_nvpair(char *piece, ul_nvpair_t * pnvpair, char **pend)
{
	char *tmp = piece;
	int nsize;
	int vsize;
	char *namehead=NULL;
	char *valuehead=NULL;
	char *valueend=NULL;

	//assert(piece && pnvpair && pend);
	if ((piece == NULL) || (pnvpair == NULL) || (pend == NULL)) {
		return -1;
	}

	pnvpair->m_status = 0;

	// skip the blank at the head of the piece
	while ((*tmp != '\0') && (ul_isspace(*tmp))) {
		tmp++;
	}
	if (*tmp == 0)
		return -1;

	namehead = tmp;

	// get name 
	while ((*tmp != 0) && !ul_isspace(*tmp) && (*tmp != '>') && (*tmp != '='))
		tmp++;

	if (*tmp == 0)
		return -1;

	nsize = tmp - namehead;

	if (nsize > MAX_NVNAME_LEN - 1) {
		// name is too long
		memcpy(pnvpair->m_name, namehead, MAX_NVNAME_LEN - 1);
		pnvpair->m_name[MAX_NVNAME_LEN - 1] = '\0';
		pnvpair->m_status |= TAG_NVSTATUS_TRUNCATEDNAME;
	} else if (nsize == 0) {
		// no name exist
		if ((*tmp == '>') || (*tmp == '\0')) {
			//meet the end of the tag
			*pend = tmp;
			return 0;
		} else {
			// miss a name
			//assert(*tmp == '=');
			//*tmp = '=';
			*pend = tmp;
			return -2;
		}
	} else {
		// copy the name
		memcpy(pnvpair->m_name, namehead, nsize);
		pnvpair->m_name[nsize] = '\0';
	}

	// skip the blank between name and value
	while ((*tmp != '\0') && ul_isspace(*tmp)) {
		tmp++;
	}
	// get the value
	if (*tmp != '=') {
		// cannot find the value, update the status of the pnvpair and 
		// set the pend to the remainder string
		pnvpair->m_value[0] = '\0';
		pnvpair->m_status |= TAG_NVSTATUS_ONLYNAME;
		*pend = tmp;
		return 1;
	} else if (ul_isspace(*(tmp + 1))) {
		// such as "<tagname name1= name2=value2... >"
		if (*(tmp + 1) != '\0') {
			tmp += 2;
		} else {
			tmp += 1;
		}
		// skip the blank 
		while ((*tmp != '\0') && ul_isspace(*tmp)) {
			tmp++;
		}
		pnvpair->m_value[0] = '\0';
		pnvpair->m_status |= TAG_NVSTATUS_ONLYNAME;
		*pend = tmp;
		return 1;
	} else {
		tmp++;
		valuehead = tmp;
		if ((*valuehead != '\'') && (*valuehead != '\"')) {
			// not delimited by quote or dolcle quote
			while (!ul_isspace(*tmp) && (*tmp != '>')) {
				tmp++;
			}
			valueend = tmp;
		} else {
			tmp++;
			while ((*tmp != *valuehead) && (*tmp != '\0')) {
				if ((*tmp == '\\') && (*(tmp + 1) == *valuehead)) {
					tmp += 2;
				} else {
					tmp++;
				}
			}
			if (*tmp == *valuehead) {
				valueend = tmp + 1;
			} else {
				//assert(*tmp == '\0');
				*tmp = 0;
				valueend = tmp;
			}

		}
		vsize = valueend - valuehead;
		if (vsize > MAX_NVVALUE_LEN - 1) {
			// the value is too long
			pnvpair->m_value[0] = '\0';
			pnvpair->m_status |= TAG_NVSTATUS_LONGVALUE;
			*pend = tmp;
			return 1;  
		}else {
			// the value is correct
			memcpy(pnvpair->m_value, valuehead, vsize);
			pnvpair->m_value[vsize] = '\0';
			*pend = valueend;
			return 1;
		}
	}
}

// }}}




/**
 */
int ul_get_taginfo(char *piece, ul_taginfo_t * ptaginfo, char **pend)
{
	int ret;
	char *remainder=NULL;
	char *nvbegin=NULL;
	char *nextnv=NULL;
	int tnsize;
	int i;

	// check the input arguments
	//assert(piece && ptaginfo && pend);
	if ((piece == NULL) || (ptaginfo == NULL) || (pend == NULL)) {
		return -1;
	}
	// get the tagname
	tnsize = sizeof(ptaginfo->m_tagname);
	ret = ul_get_tagname(piece, ptaginfo->m_tagname, tnsize, &remainder);
	switch (ret) {
	case 1:
		break;
	case -1:
		return -1;
		break;
	case -2:
		//no tagname,just like <>
		*pend = ul_jumpto_tagend(piece);
		return -2;
	case -3:
		// tagname is too long,skip this tag
		//    find the end of the tag 
		*pend = ul_jumpto_tagend(piece);
		return -3;
	default:
		// can not reach here
		//assert(0);
		return -1;
	}

	// get tha name-value pairs
	nvbegin = remainder;
	for (i = 0; i < MAX_NVCOUNT_IN_TAG; i++) {
		ret = ul_get_nvpair(nvbegin, ptaginfo->m_nvlist + i, &nextnv);
		switch (ret) {
		case 0:
			// not any more name-value pair in the tag
			ptaginfo->m_nvcount = i;
			*pend = ul_jumpto_tagend(nextnv);
			return 1;
		case 1:
			nvbegin = nextnv;
			break;
		case -1:
			return -1;
		case -2:
			// name-value pairs format is error in this tag
			// skip it.
			*pend = ul_jumpto_tagend(nvbegin);
			return -4;
		default:
			// can not reach here
			//assert(0);
			return -1;
		}
	}
	//beyond the max count of the name-pair
	// skip the last
	ptaginfo->m_nvcount = MAX_NVCOUNT_IN_TAG;
	*pend = ul_jumpto_tagend(nextnv);
	return 2;

}

// }}}

// {{{ main function
#ifdef TEST_FUNC
int main(int argc, char **argv)
{
	char buffer[1024 * 1024];
	int size;
	ul_taginfo_t taginfo;
	char *tagbegin, *tagend;
	int ret;

	size = fread(buffer, sizeof(char), sizeof(buffer), stdin);
	if (size <= 0)
		return 0;
	tagbegin = strchr(buffer, '<');
	while (tagbegin != NULL) {
		ret = ul_get_taginfo(tagbegin + 1, &taginfo, &tagend);
		fprintf(stdout, "TAGNAME=%s \t nvpaircount=%d \t ret=%d\n",
				taginfo.m_tagname, taginfo.m_nvcount, ret);
		for (int i = 0; i < taginfo.m_nvcount; i++) {
			fprintf(stdout, "\t STATUS=%d\t ATTR=%s\t VALUE=%s\n",
					taginfo.m_nvlist[i].m_status,
					taginfo.m_nvlist[i].m_name, taginfo.m_nvlist[i].m_value);
		}
		tagbegin = strchr(tagend, '<');
	}
	return 0;
}
#endif
// }}}
/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
