
#include "ul_pack.h"

#define BUF_START		  "~BUF! "					/**< buff开始标志       */
const size_t BUF_START_SIZE=sizeof(BUF_START)-1;
#define BUF_EOF			  "~EOF!"					/**< buff结束标志       */
const size_t BUF_EOF_SIZE=sizeof(BUF_EOF)-1;


int ul_pack_putvalue(ul_package_t *ppack, const char *pname, const char *pvalue);


char *ul_pack_getbuf(const ul_package_t *ppack);


int ul_pack_getvalue(ul_package_t *ppack, char *pname, char *pvalue)
{
	int i;
	if (NULL == ppack || NULL == pname || NULL == pvalue) {
		return 0;
	}
	pvalue[0] = '\0';

	for (i=0; i<ppack->pairnum; i++) {
		if (strncmp(ppack->name[i], pname, sizeof(ppack->name[i])) == 0) {
			snprintf(pvalue, sizeof(ppack->value[i]), "%s", ppack->value[i]);
			return 1;
		}
	}
	return 0;
}

int ul_pack_read(int sockfd, ul_package_t *ppack)
{
	int  size;
	int  headsize = -1;
	int  bodysize = -1;   
	int  i = 0;
	char sizebuf[SIZE_NUM];
	char headbuf[MAX_HEAD_SIZE];
	char *ph = headbuf;
	char *q;
	char tmp[10], eofstr[BUF_EOF_SIZE + 1];

	if (NULL == ppack) {
		return -2;
	}
	memset(sizebuf, 0, SIZE_NUM);
	size = ul_pack_readall(sockfd, sizebuf, SIZE_NUM);
	if(-1 == size) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: socket error\n");
		return -2;
	} else if(size != SIZE_NUM || strncmp(sizebuf, BUF_START, BUF_START_SIZE) != 0) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: size number error.[%d:%d || %20s].", size, SIZE_NUM, sizebuf);
		if (ul_pack_readend(sockfd) < 0)
			return -2;
		return -1;
	}

	sscanf(sizebuf,"%s%d%d", tmp, &headsize, &bodysize);
	if (headsize < 0 || bodysize < 0) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: size are negative [%d : %d]", headsize, bodysize); 
		if (ul_pack_readend(sockfd) < 0) {
			return -2;
		}
		return -1;
	}
	if (bodysize > ppack->bufsize || headsize >= MAX_HEAD_SIZE) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: buffer size not enough [%d : %d]", ppack->bufsize, bodysize); 
		if (ul_pack_readend(sockfd) < 0) {
			return -2;
		}
		return -1;
	} 

	size = ul_pack_readall(sockfd, headbuf, headsize); 
	if (-1 == size) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: socket error\n");
		return -2;
	} else if (size != headsize) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: read head error.size[%d] not equal bodysize[%d]", size, headsize);
		if (ul_pack_readend(sockfd) < 0) { 
			return -2;
		}
		return -1;
	}
	headbuf[headsize] = '\0';                                                                                                                       
	size = ul_pack_readall(sockfd, ppack->buf, bodysize);
	if (-1 == size) {   
		ul_writelog(UL_LOG_WARNING,"ul_pack_read: socket error\n");
		return -2; 
	}
	if (size != bodysize) {
		ul_writelog(UL_LOG_WARNING,"ul_pack_read: read body error\n");
		if (ul_pack_readend(sockfd) < 0) {
			return -2;
		}
		return -1;
	}

	size = ul_pack_readall(sockfd, eofstr, BUF_EOF_SIZE);
	eofstr[BUF_EOF_SIZE] = '\0';
	if (-1 == size) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: socket error\n");
		return -2; 
	}
	if ((size_t)size != BUF_EOF_SIZE || strncmp(eofstr, BUF_EOF, BUF_EOF_SIZE) != 0 ) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_read: read EOF error,%d,%5s\n", size, eofstr);
		if (ul_pack_readend(sockfd) < 0) { 
			return -2;
		}
		return -1;
	}

	while(*ph != '\0' && i < MAX_PACK_ITEM_NUM) {
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
			if (ul_pack_readend(sockfd) < 0) { 
				return -2;
			}
			return -1;
		}
		snprintf(ppack->value[i], sizeof(ppack->value[i]), "%s", p+3);

		sscanf(ph, "%s%s", ppack->name[i], tmp);
		if (':' == tmp[0]) {
			i++;
		}
		ph = q+2;
	}
	ppack->pairnum = i;

	return (bodysize);    
}       


int ul_pack_readall(int fd, void *ptr1, size_t nbytes)
{
	ssize_t nread, nleft = nbytes;
	char *ptr = (char*)ptr1;
	while (nleft > 0) {
again:
		nread = read(fd, ptr, nleft);
		if (nread < 0) {
			if (EINTR == errno) goto again;
			ul_writelog(UL_LOG_WARNING, "read from socket(%d,%zu) failed.error[%d] info is %s.",
					fd, nbytes, errno, strerror(errno));
			return -1;
		} else if (0 == nread) {
			break;
		}
		ptr += nread;
		nleft -= nread;
	}
	return (nbytes-nleft);
}


int ul_pack_readend(int sockfd)
{
	int    size;
	char   tmp[10];
	char   *pend = tmp;
	int    flag = 0;
	size_t count = 0;


	for (;;) {
		size = read(sockfd, pend, 1);
		if (size < 0) {
			if(EINTR == errno) {
				continue;
			} else {
				ul_writelog(UL_LOG_WARNING, "ul_pack_readend: read socket error.errno[%d] is %s.",
						errno, strerror(errno));
				return -1;
			}
		} else if (0 == size) {
			return -1;
		}

		if (flag) {
			if ('~' == *pend) {
				count = 1;
				tmp[0] = *pend;
				pend = tmp+1;
			} else {
				pend++;
				count++;
				if (BUF_EOF_SIZE == count) {
					tmp[count] = '\0';
					if (strncmp(tmp, BUF_EOF, BUF_EOF_SIZE) == 0) {
						return 0;
					} else {
						pend = tmp;
						flag = 0;
						count = 0;
					}
				}
			}   
		} else {
			if('~' == *pend) {
				flag = 1;
				count = 1;
				pend++;
			}
		} 
	}   
}


int ul_pack_write(int sockfd, ul_package_t *ppack)
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
	}
	headsize += snprintf(headbuf + headsize, sizeof(headbuf) - headsize, "%s", "\r\n");

	bodysize = ppack->bufsize;
	snprintf(sizebuf, sizeof(sizebuf), "%s%d %d", BUF_START, headsize, bodysize);
	size = strlen(sizebuf);

	if (size < SIZE_NUM - 1) {
		sizebuf[size] = ' ';
		for (i = size+1; i < SIZE_NUM-1; i++) {
		}
		sizebuf[SIZE_NUM-1] = '\0';
	}

	size = ul_pack_writeall(sockfd, sizebuf, SIZE_NUM);
	if (size != SIZE_NUM) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_write: write size error %d:%d.",size,SIZE_NUM);
		return -1;
	}

	size = ul_pack_writeall(sockfd, headbuf, headsize);
	if (size != headsize) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_write: write head error.size is %d:%d.",size,headsize);
		return -1;
	}

	size = ul_pack_writeall(sockfd, ppack->buf, bodysize);
	if (size != bodysize) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_write: write body error.size is %d:%d.",size,bodysize);
		return -1;
	}
	size = ul_pack_writeall(sockfd, BUF_EOF, BUF_EOF_SIZE);
	if ((size_t)size != BUF_EOF_SIZE) {
		ul_writelog(UL_LOG_WARNING, "ul_pack_write: write eof error.%d.",size);
		return -1;
	}   

	return 0;
}


int ul_pack_writeall(int sockfd, char *buf, int n)
{
	int   nleft = n;
	int   nbytes;
	char  *ptr = buf;

	for (; nleft>0; ) {
		nbytes = write(sockfd, ptr, nleft);
		if (nbytes <= 0) {
			if (EINTR == errno) {
				nbytes = 0;
			} else {
				ul_writelog(UL_LOG_WARNING, "ul_pack_writeall: write socket[%d] error.errno[%d] is %s.",
						sockfd, errno, strerror(errno));
				return -1;
			}
		}
		nleft -= nbytes;
		ptr += nbytes;
	}

	return n;
}        


int ul_pack_write_m(int sockfd, ul_package_t *ppack)
{

	int  headsize = 0;
	int  bodysize = 0;
	int  size;
	int  i;
	char *tmpbuf=NULL;
	int  tmpbuf_size = 0;
	int	 ret;
	int  pairlen = 0;
	int  curpos = 0;
	if (NULL == ppack) {
		return -1;
	}
	tmpbuf_size = SIZE_NUM+MAX_HEAD_SIZE + ppack->bufsize + 20;
	if ((tmpbuf = (char*)malloc(tmpbuf_size)) == NULL) {
		ul_writelog(UL_LOG_WARNING,
				"[%s] malloc sendbuf(size=%d) for write ul_package_t failed."
				"using ul_pack_write",
				__func__,ppack->bufsize);
		return ul_pack_write(sockfd, ppack);
	}

	curpos = SIZE_NUM;
	for (i = 0;i < ppack->pairnum; i++) {
		if (ppack->name[i][0] != '\0') {
			pairlen = snprintf(tmpbuf + curpos, sizeof(tmpbuf) - curpos, "%s : %s\r\n",
					ppack->name[i],ppack->value[i]);
			
			if (pairlen <= 0) {
				ret = -1;
				goto end;
			}
			curpos += pairlen;
		}
	}
	curpos += snprintf(tmpbuf + curpos, sizeof(tmpbuf) - curpos, "%s", "\r\n");

	headsize = curpos - SIZE_NUM;
	if (headsize >= MAX_HEAD_SIZE) {
		ret = -1;
		goto end;
	}

	bodysize = ppack->bufsize;

	if ((size = snprintf(tmpbuf, SIZE_NUM, "%s%d %d", BUF_START, headsize, bodysize)) > SIZE_NUM) {
		ret = -1;
		goto end;
	}

	if (size < SIZE_NUM - 1) {
		tmpbuf[size] = ' ';
		for (i = size + 1; i < SIZE_NUM - 1;i++) {
		}

		tmpbuf[SIZE_NUM-1] = '\0';
	} else {
		ret=-1;
		goto end;
	}

	if (curpos > SIZE_NUM + MAX_HEAD_SIZE) {
		ret = - 1;
		goto end;
	}
	if (bodysize != 0) {
		if (NULL == ppack->buf) {
			ret = -1;
			goto end;
		}
		memcpy(tmpbuf + curpos, ppack->buf, bodysize);
		curpos += bodysize;
	}

	memcpy(tmpbuf + curpos, BUF_EOF, BUF_EOF_SIZE);

	curpos += BUF_EOF_SIZE;

	if ((size = ul_pack_writeall(sockfd, tmpbuf, curpos)) != curpos) {
		ul_writelog(UL_LOG_WARNING,"%s: write size error %d:%d.",
				__func__,size,curpos);
		ret=-1;
	} else {
		ret=0;
	}
end:
	free(tmpbuf);
	return ret;
}

int ul_pack_reado(int sockfd, ul_package_t *ppack, int over_time)
{
	return ul_pack_reado_ms(sockfd, ppack, over_time * 1000);
}


int ul_pack_writeo(int sockfd, ul_package_t *ppack ,int sec)
{
	return ul_pack_writeo_ms(sockfd,ppack,sec * 1000);	
}


int ul_pack_writeallo(int sockfd, char *buf, int n, int sec)
{
	return ul_swriteo_ex(sockfd, buf, n, sec);	
}

int ul_pack_write_mo(int sockfd, ul_package_t *ppack, int sec)
{
	return ul_pack_write_mo_ms(sockfd, ppack, sec * 1000);
}


int ul_pack_readallo_ms(int fd, void *ptr1, size_t nbytes, int msec)
{
	ssize_t nread, nleft = (ssize_t)nbytes;
	char *ptr = (char*)ptr1;
	while (nleft > 0) {
again:
		nread = ul_reado_ms(fd, ptr, (size_t)nleft ,msec);
		if(nread < 0) {
			switch (nread) {
				case -1:
				case -3:
					if (EINTR == errno) goto again;
					break ;
				default:
					break;
			}
			ul_writelog(UL_LOG_WARNING, "read from socket(%d,%zu) failed.error[%d] info is %s.",
					fd, nbytes, errno, strerror(errno));
			return -1;
		} else if (0 == nread) {
			break;
		}
		ptr += nread;
		nleft -= nread;
	}
	return (int)((ssize_t)nbytes - nleft);
}

int ul_pack_readendo_ms(int sockfd, int msec)
{
	int   size;
	char  tmp[10];
	char  *pend = tmp;
	int   flag = 0;
	size_t   count = 0;

	for (;;) {
		size = ul_reado_ms(sockfd, pend, 1, msec);
		if (size < 0) {
			if (size != -2 && EINTR == errno) {
				continue;
			} else {
				ul_writelog(UL_LOG_WARNING,"ul_pack_readend: read socket error.errno[%d] is %s.",
						errno, strerror(errno));
				return -1;
			}
		} else if (0 == size) {
			return -1;
		}
		
		if (flag) {
			if ('~' == *pend) {
				count = 1;
				tmp[0] = *pend;
				pend = tmp+1;
			} else {
				pend++;
				count++;
				if (BUF_EOF_SIZE == count) {
					tmp[count] = '\0';
					if (strncmp(tmp, BUF_EOF, BUF_EOF_SIZE) == 0) {
						return 0;
					} else {
						pend = tmp;
						flag = 0;
						count = 0;
					}
				}
			}   
		} else {
			if ('~' == *pend) {
				flag = 1;
				count = 1;
				pend++;
			}
		}           
	}   
}


int ul_pack_writeallo_ms(int sockfd, char *buf, int n,int msec)
{
	return ul_swriteo_ms_ex(sockfd, buf, n, msec);
}

int ul_pack_writeo_ms(int sockfd, ul_package_t *ppack, int msec)
{
	return ul_pack_writeo_ms(sockfd, (const ul_package_t*)ppack, msec);
}

int ul_pack_putvalue(ul_package_t *ppack, char *pname, char *pvalue)
{
	return ul_pack_putvalue(ppack, (const char *)pname, (const char *)pvalue);
}

char *ul_pack_getbuf(ul_package_t *ppack)
{
	return ul_pack_getbuf((const ul_package_t*)ppack);
}

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
