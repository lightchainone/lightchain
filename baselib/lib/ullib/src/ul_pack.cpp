
#include "ul_pack.h"

#define BUF_START "~BUF! "					/**< buff开始标志       */
const size_t BUF_START_SIZE = sizeof(BUF_START)-1;
#define BUF_EOF "~EOF!"						/**< buff结束标志       */
const size_t BUF_EOF_SIZE = sizeof(BUF_EOF)-1;

ul_package_t *ul_pack_init()		 
{
	ul_package_t *pp;
	pp = (ul_package_t *)ul_calloc(1, sizeof(ul_package_t));
	return pp;
}

int ul_pack_free(ul_package_t *ppack)
{
	ul_free(ppack);
	return 0;
}

int ul_pack_putbuf(ul_package_t *ppack, char *buffer, int size)
{
	if (NULL == ppack) {
		return 0;
	}
	ppack->buf = buffer;
	ppack->bufsize = size;
	return 0;
}

char *ul_pack_getbuf(const ul_package_t *ppack)
{
	if (NULL == ppack) {
		return NULL;
	}
	return ppack->buf;
}

//限制缓冲区长度版本
int ul_pack_getvalue_ex(ul_package_t *ppack, const char *pname, char *pvalue, size_t size)
{
	int i;
	if (NULL == ppack || NULL == pname || NULL == pvalue) {
		return 0;
	}
	pvalue[0] = '\0';

	for (i = 0; i < ppack->pairnum; i++) {
		if (strcmp(ppack->name[i], pname) == 0) {
			if (snprintf(pvalue, size, "%s", ppack->value[i]) > (ssize_t)size) {
				ul_writelog(UL_LOG_WARNING, "%s ppack item length too long [size:%zu]", __func__, size);
			}
			return 1;
		}
	}
	return 0;
}

/**
 *
 * -1 删除失败 输入错误或key值不存在
 * 0 删除成功
**/
int ul_pack_delvalue(ul_package_t *ppack, const char *pname)
{
	int i;
	int j;

	if (NULL == ppack || NULL == pname) {
		return -1;
	}

	for (i = 0; i < ppack->pairnum; i++) {
		if (strcmp(ppack->name[i], pname) == 0) {
			for (j=i+1; j<ppack->pairnum; j++) {
				snprintf(ppack->name[j-1], sizeof(ppack->name[j-1]), "%s", ppack->name[j]);
				snprintf(ppack->value[j-1], sizeof(ppack->value[j-1]), "%s", ppack->value[j]);
			}

			break;
		}
	}
	if (i == ppack->pairnum) {
		return -1;
	}
	ppack->pairnum --;

	return 0;
}


int ul_pack_putvalue(ul_package_t *ppack, const char *pname, const char *pvalue)
{
	int i;

	if (NULL == ppack || NULL == pname || NULL == pvalue) {
		return -1;
	}

	if (strlen(pvalue) >= MAX_ITEM_SIZE || strlen(pname) >= MAX_ITEM_SIZE) {
		return -1;
	}

	if (strstr(pname, "\r\n") || strstr(pname, " : ") ||
		strstr(pvalue, "\r\n") || strstr(pvalue, " : ")) {
		return -1;
	}

	for (i = 0; i < ppack->pairnum; i++) {
		if (strcmp(ppack->name[i], pname) == 0) {
			snprintf(ppack->value[i], sizeof(ppack->value[i]), "%s", pvalue);
			return 1;
		}
	}

	if (ppack->pairnum < MAX_PACK_ITEM_NUM) {
		snprintf(ppack->name[ppack->pairnum], sizeof(ppack->name[ppack->pairnum]), "%s", pname);
		snprintf(ppack->value[ppack->pairnum], sizeof(ppack->value[ppack->pairnum]), "%s", pvalue);
		ppack->pairnum++;
		return 0;
	} else {
		return -2;
	}
}

int ul_pack_reado_ms(int sockfd, ul_package_t *ppack, int msec)
{
	int   size;
	int   headsize = -1;
	int   bodysize = -1;   
	int   i = 0;
	char  sizebuf[SIZE_NUM];
	char  headbuf[MAX_HEAD_SIZE];
	char  *ph = headbuf;
	char  *q;
	char  tmp[10], eofstr[BUF_EOF_SIZE+1];

	if (NULL == ppack) {
		return -2;
	}
	memset(sizebuf, 0, sizeof(sizebuf));
	size = ul_pack_readallo_ms(sockfd, sizebuf, sizeof(sizebuf), msec);
	if (-1 == size) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_reado: overtime or socket error");
		return -2;
	} else if(size != SIZE_NUM || strncmp(sizebuf, BUF_START, BUF_START_SIZE) != 0) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: size number error.[%d:%d || %s].", size, SIZE_NUM, sizebuf);
		if(ul_pack_readendo_ms(sockfd, msec) < 0) {
			return -2;
		}
		return -1;
	}

	sscanf(sizebuf, "%s%d%d", tmp, &headsize, &bodysize);
	if (headsize < 0 || bodysize < 0) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: size are negative [%d : %d]",
				headsize, bodysize); 
		if(ul_pack_readendo_ms(sockfd, msec) < 0) {
			return -2;
		}
		return -1;
	}

	if (bodysize > ppack->bufsize || headsize >= MAX_HEAD_SIZE) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: buffer size not enough [%d : %d]",
				ppack->bufsize, bodysize); 
		if (ul_pack_readendo_ms(sockfd, msec) < 0) {
			return -2;
		}
		return -1;
	}

	size = ul_pack_readallo_ms(sockfd, headbuf, (size_t)headsize, msec); 
	if (-1 == size) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: socket error");
		return -2;
	}else if(size != headsize) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: read head error.size[%d] not equal bodysize[%d]",
				size, headsize);
		if(ul_pack_readendo_ms(sockfd, msec) < 0) {
			return -2;
		}
		return -1;
	}
	headbuf[headsize] = '\0';                                                                                                                       
	size = ul_pack_readallo_ms(sockfd, ppack->buf, (size_t)bodysize, msec);
	if (-1 == size) {   
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: socket error");
		return -2; 
	}

	if (size != bodysize) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: read body error");
		if (ul_pack_readendo_ms(sockfd, msec) < 0) { 
			return -2;
		}
		return -1;
	}

	size = ul_pack_readallo_ms(sockfd, eofstr, BUF_EOF_SIZE, msec);
	eofstr[BUF_EOF_SIZE] = '\0';
	if (-1 == size) {   
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: socket error");
		return -2; 
	}

	if ((size_t)size != BUF_EOF_SIZE || strncmp(eofstr, BUF_EOF, BUF_EOF_SIZE) != 0 ) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: read EOF error,%d,%s", size, eofstr);
		if(ul_pack_readendo_ms(sockfd, msec) < 0) { 
			return -2;
		}
		return -1;
	}

	while (*ph != '\0' && i < MAX_PACK_ITEM_NUM) {
		q = strstr(ph, "\r\n");
		if (NULL == q || q == ph)
			break;
		*q = '\0';
		ppack->name[i][0]  = '\0';
		ppack->value[i][0]  = '\0';
		char *p ;
		p = strstr(ph , " : ");
		if (NULL == p || p-ph >= MAX_ITEM_SIZE || q-(p+3) >= MAX_ITEM_SIZE) {
			ul_writelog(UL_LOG_WARNING, "ul_pack_read: head data error error [%s]", ph);
			if(ul_pack_readendo_ms(sockfd, msec) < 0) {
				return -2;
			}
			return -1;
		}
		snprintf(ppack->value[i], sizeof(ppack->value[i]), "%s", p+3);

		sscanf(ph, "%s%s", ppack->name[i], tmp );
		if (':' == tmp[0]) {
			i++;
		}

		ph = q + 2;
	}

	int leftTag = 0;
	if ((*ph != '\0') && (i == MAX_PACK_ITEM_NUM)) {
		q = strstr(ph, "\r\n");
		if (NULL == q || q == ph) {
			leftTag = 0;
		} else {
			*q = '\0';
			char *p;
			p = strstr(ph, " : ");
			if (NULL == p) {
				leftTag = 0;						
			} else {
				leftTag = 1;
			}
		}
	}
	if (1 == leftTag) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: left key-value pairs in head, curr recv[%d]", i);
	}

	ppack->pairnum = i;

	return bodysize;    
}

int ul_pack_writeo_ms(int sockfd, const ul_package_t *ppack, int msec)
{
	char  sizebuf[SIZE_NUM];
	char  headbuf[MAX_HEAD_SIZE];
	int   headsize = 0;
	int   bodysize = 0;
	int   size;
	int   i;
	if (NULL == ppack) {
		return -1;
	}
	headbuf[0] = '\0';

	for (i = 0;i < ppack->pairnum; i++) {
		if (ppack->name[i][0] != '\0') {
			headsize += snprintf(headbuf + headsize, sizeof(headbuf) - headsize, 
					"%s : %s\r\n", ppack->name[i], ppack->value[i]);
		}
		if (headsize > MAX_HEAD_SIZE) {
			ul_writelog(UL_LOG_WARNING, "ul_pack_writeo_ms: head size over default max head .%d.",
headsize);
			return -2;
		}
	}
	headsize += snprintf(headbuf + headsize, sizeof(headbuf) - headsize, "%s", "\r\n");

	if (headsize > MAX_HEAD_SIZE) {
		ul_writelog(UL_LOG_WARNING,"ul_pack_writeo_ms: head size over default max head size.%d.",
headsize);
		return -2;
	}

	bodysize = ppack->bufsize;
	size = snprintf(sizebuf, sizeof(sizebuf), "%s%d %d", BUF_START, headsize, bodysize);

	if (size < SIZE_NUM - 1) {
		sizebuf[size] = ' ';
		for (i = size + 1; i < SIZE_NUM - 1; i++)
		sizebuf[SIZE_NUM-1] = '\0';
	}

	size = ul_pack_writeallo_ms(sockfd, sizebuf, SIZE_NUM, msec);
	if (size != SIZE_NUM) {
		ul_writelog(UL_LOG_WARNING,"ul_pack_writeo_ms: write size error %d:%d.", size, SIZE_NUM);
		return -1;
	}

	size = ul_pack_writeallo_ms(sockfd, headbuf, (size_t)headsize, msec);
	if (size != headsize) {
		ul_writelog(UL_LOG_WARNING,"ul_pack_writeo_ms: write head error.size is %d:%d.", size, headsize);
		return -1;
	}

	size = ul_pack_writeallo_ms(sockfd,ppack->buf, (size_t)bodysize, msec);
	if (size != bodysize) {
		ul_writelog(UL_LOG_WARNING,"ul_pack_writeo_ms: write body error.size is %d:%d.", size, bodysize);
		return -1;
	}
	size = ul_pack_writeallo_ms(sockfd, BUF_EOF, BUF_EOF_SIZE, msec);
	if ((size_t)size != BUF_EOF_SIZE) {
		ul_writelog(UL_LOG_WARNING,"ul_pack_writeo_ms: write eof error.%d.",size);
		return -1;
	}

	return 0;
}

int ul_pack_cleanhead(ul_package_t* ppack)
{
	if (NULL == ppack) {
		return -1;
	} else {
		ppack->pairnum = 0;
		return 1;
	}
}

int ul_pack_cleanall(ul_package_t* ppack)
{
	if (NULL == ppack) {
		return -1;
	} else {
		ppack->pairnum = 0;
		ppack->bufsize = 0;
		ppack->buf = NULL;
		return 1;
	}
}

int ul_pack_write_mo_ms(int sockfd, ul_package_t *ppack, int msec)
{

	int  headsize = 0;
	int  bodysize = 0;
	int  size;
	int  i;
	char *tmpbuf = NULL;
	int  ret;
	int  pairlen;
	size_t  curpos = 0;
	int maxLen;

	if (NULL == ppack) {
		return -1;
	}
	tmpbuf = (char*)malloc(SIZE_NUM+MAX_HEAD_SIZE + ppack->bufsize + 20UL);
	if (NULL == tmpbuf) {
		ul_writelog(UL_LOG_WARNING,
				"[%s] malloc sendbuf(size=%d) for write ul_package_t failed."
				"using ul_pack_writeo_ms",
				__func__, ppack->bufsize);
		return ul_pack_writeo_ms(sockfd,ppack, msec);
	}

	maxLen = SIZE_NUM + MAX_HEAD_SIZE + ppack->bufsize + 20UL;
	curpos = SIZE_NUM;
	for (i = 0;i < ppack->pairnum; i++) {
		if (ppack->name[i][0] != '\0') {
			pairlen = snprintf(tmpbuf+curpos, maxLen - curpos, "%s : %s\r\n",
					ppack->name[i],ppack->value[i]);
			if (pairlen >= (int)(maxLen-curpos)) {
				ret = -1;
				goto end;
			}
			/*
			if (pairlen <= 0) {
				ret = -1;
				goto end;
			}
			*/
			curpos += pairlen;
			if ((curpos-SIZE_NUM) > MAX_HEAD_SIZE) {
				ret =-1;
				goto end;
			}
		}
	}

	curpos += snprintf(tmpbuf + curpos, maxLen - curpos, "%s", "\r\n"); 

	headsize = (int)(curpos - SIZE_NUM);

	if (headsize > MAX_HEAD_SIZE) {
		ret = -1;
		goto end;
	}

	bodysize = ppack->bufsize;

	if ((size = snprintf(tmpbuf, SIZE_NUM, "%s%d %d", BUF_START, headsize, bodysize)) >= SIZE_NUM) {
		ret = -1;
		goto end;
	}

	if (size < SIZE_NUM - 1) {
		tmpbuf[size] = ' ';
		for (i = size + 1; i < SIZE_NUM - 1; i++)
		tmpbuf[SIZE_NUM - 1] = '\0';
	} else {
		ret = -1;
		goto end;
	}

	if (curpos > SIZE_NUM+MAX_HEAD_SIZE) {
		ret = -1;
		goto end;
	}

	if (bodysize != 0) {
		if (NULL == ppack->buf) {
			ret = -1;
			goto end;
		}
		memcpy(tmpbuf + curpos, ppack->buf, (size_t)bodysize);
		curpos += bodysize;
	}

	memcpy(tmpbuf + curpos, BUF_EOF, BUF_EOF_SIZE);

	curpos += BUF_EOF_SIZE;
	size = ul_pack_writeallo_ms(sockfd, tmpbuf, curpos, msec);
	if ((size_t)size != curpos) {
		ul_writelog(UL_LOG_WARNING,"%s: write size error %d:%zu.",
				__func__, size, curpos);
		ret = -1;
	} else {
		ret = 0;
	}
end:
	free(tmpbuf);
	return ret;
}

int ul_pack_serialization(void *buf, int bufsize, ul_package_t *ppack)
{

	int headsize = 0;
	int bodysize = 0;
	int size, i, pairlen, maxlen;
	int ret = 0;
	size_t curpos = 0;
	char *tmpbuf;

	if (buf == NULL || bufsize <= 0 || ppack == NULL) {
		return -1;
	}

	maxlen = SIZE_NUM + MAX_HEAD_SIZE + ppack->bufsize + 20UL;

	tmpbuf = (char *)malloc(maxlen);
	if (tmpbuf == NULL) {
		ul_writelog(UL_LOG_WARNING,
				"[%s] malloc buf(size=%d) for serial ul_package_t failed.",
				__func__, ppack->bufsize);
		return -1;
	}

	curpos = SIZE_NUM;
	for (i = 0; i < ppack->pairnum; i++) {
		if (ppack->name[i][0] != '\0') {
			pairlen = snprintf(tmpbuf + curpos, maxlen - curpos, "%s : %s\r\n",
					ppack->name[i], ppack->value[i]);
			if (pairlen >= (int)(maxlen - curpos)) {
				ret = -1;
				goto end;
			}
			curpos += pairlen;
			if ((curpos - SIZE_NUM) > MAX_HEAD_SIZE) {
				ret = -1;
				goto end;
			}
		}
	}

	curpos += snprintf(tmpbuf + curpos, maxlen - curpos, "%s", "\r\n"); 

	headsize = (int)(curpos - SIZE_NUM);

	if (headsize > MAX_HEAD_SIZE) {
		ret = -1;
		goto end;
	}

	bodysize = ppack->bufsize;

	if ((size = snprintf(tmpbuf, SIZE_NUM, "%s%d %d", BUF_START, headsize, bodysize)) >= SIZE_NUM) {
		ret = -1;
		goto end;
	}

	if (size < SIZE_NUM - 1) {
		tmpbuf[size] = ' ';
		for (i = size + 1; i < SIZE_NUM - 1; i++)
		tmpbuf[SIZE_NUM - 1] = '\0';
	} else {
		ret = -1;
		goto end;
	}

	if (curpos > SIZE_NUM + MAX_HEAD_SIZE) {
		ret = -1;
		goto end;
	}

	if (bodysize != 0) {
		if (ppack->buf == NULL) {
			ret = -1;
			goto end;
		}
		memcpy(tmpbuf + curpos, ppack->buf, (size_t)bodysize);
		curpos += bodysize;
	}

	memcpy(tmpbuf + curpos, BUF_EOF, BUF_EOF_SIZE);
	curpos += BUF_EOF_SIZE;

	if (curpos >= (size_t)bufsize) {
		ret = -1;
	} else {
		memcpy(buf, tmpbuf, curpos);
		ret = curpos;
	}

end:
	free(tmpbuf);
	return ret;
}

int ul_pack_deserialization(void *buf, int bufsize, ul_package_t *ppack)
{
	int headsize = -1;
	int bodysize = -1;   
	int i = 0;
	int packsize;
	char sizebuf[SIZE_NUM];
	char headbuf[MAX_HEAD_SIZE];
	char tmp[10], eofstr[BUF_EOF_SIZE+1];
	char *ph = headbuf;
	char *q;

	if (buf == NULL || bufsize <= 0 || ppack == NULL) {
		return -1;
	}

	/* read STRART */
	memcpy(sizebuf, buf, SIZE_NUM);
	if(strncmp(sizebuf, BUF_START, BUF_START_SIZE) != 0) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_deserialization: start buf error.[%d : %s].", SIZE_NUM, sizebuf);
		return -1;
	}
	sscanf(sizebuf, "%s%d%d", tmp, &headsize, &bodysize);
	if (headsize < 0 || bodysize < 0) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_deserialization: size are negative [%d : %d]",
				headsize, bodysize); 
		return -1;
	}
	if (bodysize > ppack->bufsize || headsize >= MAX_HEAD_SIZE) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_deserialization: body buffer size not enough [%d : %d]",
				ppack->bufsize, bodysize); 
		return -1;
	}
	packsize = SIZE_NUM + headsize + bodysize + BUF_EOF_SIZE;
	if (bufsize < packsize) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_deserialization: total buffer size not enough [%d : %d]",
				bufsize, packsize); 
		return -1;
	}

	/* read HEAD */
	memcpy(headbuf, (char *)buf + SIZE_NUM, headsize);
	headbuf[headsize] = '\0';                                                                                                                       

	/* read BODY */
	memcpy(ppack->buf, (char *)buf + SIZE_NUM + headsize, bodysize);
	ppack->bufsize = bodysize;

	/* read END */
	memcpy(eofstr, (char *)buf + SIZE_NUM + headsize + bodysize, BUF_EOF_SIZE);
	eofstr[BUF_EOF_SIZE] = '\0';
	if (strncmp(eofstr, BUF_EOF, BUF_EOF_SIZE) != 0 ) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_deserialization: read EOF error.[%ld || %s].", BUF_EOF_SIZE, eofstr);
		return -1;
	}

	/* parse HEAD */
	while (*ph != '\0' && i < MAX_PACK_ITEM_NUM) {
		q = strstr(ph, "\r\n");
		if (NULL == q || q == ph)
			break;
		*q = '\0';
		ppack->name[i][0]  = '\0';
		ppack->value[i][0]  = '\0';
		char *p ;
		p = strstr(ph , " : ");
		if (NULL == p || p-ph >= MAX_ITEM_SIZE || q-(p+3) >= MAX_ITEM_SIZE) {
			ul_writelog(UL_LOG_WARNING, "ul_pack_deserialization: head data error error [%s]", ph);
			return -1;
		}
		snprintf(ppack->value[i], sizeof(ppack->value[i]), "%s", p+3);

		sscanf(ph, "%s%s", ppack->name[i], tmp );
		if (':' == tmp[0]) {
			i++;
		}

		ph = q + 2;
	}

	int leftTag = 0;
	if ((*ph != '\0') && (i == MAX_PACK_ITEM_NUM)) {
		q = strstr(ph, "\r\n");
		if (NULL == q || q == ph) {
			leftTag = 0;
		} else {
			*q = '\0';
			char *p;
			p = strstr(ph, " : ");
			if (NULL == p) {
				leftTag = 0;						
			} else {
				leftTag = 1;
			}
		}
	}
	if (1 == leftTag) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_deserialization: left key-value pairs in head, curr recv[%d]", i);
	}

	ppack->pairnum = i;

	return packsize;    
}
/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
