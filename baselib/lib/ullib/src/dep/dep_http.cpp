#include "ul_net.h"
#include "ul_http.h"
#include "ul_log.h"
#include "ul_char.h"
#include "ul_url.h"

int zx_connecto(int sockfd, const struct sockaddr *saptr, socklen_t socklen, int secs)
{
	int flags;
	int n, error;
	socklen_t len;

	if (sockfd <= 0 || saptr == (struct sockaddr *) 0)
		return -1;
	if (secs < 0)
		secs = 0;
	error = 0;
	flags = fcntl(sockfd, F_GETFL, 0);
	fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

	if ((n = connect(sockfd, saptr, socklen)) < 0) {
		if (errno != EINPROGRESS) {
			ul_writelog(UL_LOG_DEBUG,
						"connect(%d,<%d,%d,%u>,%d) call failed.error[%d] info is %s.", sockfd,
						((struct sockaddr_in *) saptr)->sin_family,
						((struct sockaddr_in *) saptr)->sin_port,
						((struct sockaddr_in *) saptr)->sin_addr.s_addr, socklen, errno,
						strerror(errno));

			if (errno == EAGAIN) {
				ul_writelog(UL_LOG_DEBUG, "TmpResUNAV!!!!!! in zxconnecto");
				return (-10);
			}

			return (-1);
		}

	}
	if (n == 0)
		goto done;

	struct pollfd pfd;
	pfd.fd = sockfd;
	pfd.events = POLLIN | POLLOUT;

	if ( (n = ul_poll(&pfd, 1, secs ? secs * 1000 : -1)) == 0 ) {
		ul_sclose(sockfd);
		sockfd = -1;
		errno = ETIMEDOUT;
		return -1;
	}

	if ( (pfd.revents & POLLIN) || (pfd.revents & POLLOUT) ) {
		len = sizeof(error);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
			return -1;
		}
	} else {
		return -1;
	}
  done:
	fcntl(sockfd, F_SETFL, flags);
	if (error) {
		ul_sclose(sockfd);
		sockfd = -1;
		errno = error;
		return -1;
	}
	return 0;
}

int zx_sclose(int fd)
{
	//shutdown(fd, SHUT_RDWR);
	return ul_sclose(fd);
}

int zx_tcpconnecto(char *host, int port, int secs)
{
	int fd;
	struct sockaddr_in sin;
	char buf[8192];
	int ru;
	//struct linger lin;

	//lin.l_onoff=1;
	//lin.l_linger=1;

	if ((fd = ul_socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		return -1;
	}
	//ul_setsockopt(fd, SOL_SOCKET, SO_LINGER, &lin, sizeof(lin));

	// resolv host to sockaddr_in
	bzero(&sin, sizeof(sin));

	//try A.B.C.D numeric IP format address first
	if ((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE) {
		//not numeric IP format, try text format
		struct hostent he, *p;
		int err, ret;
		if ((ret = ul_gethostbyname_r(host, &he, buf, sizeof(buf), &p, &err)) < 0) {
			close(fd);
			fd = -1;
			return -1;			//Resolve failed! :(
		}
		memcpy(&sin.sin_addr.s_addr, he.h_addr, sizeof(sin.sin_addr.s_addr));
	}
	// here we assume we are always under IPv4 environment.
	// If one day shifted to other network environment, such as IPv6,
	// following code should also be under modification
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);
	//connect
	if ((ru = zx_connecto(fd, (struct sockaddr *) &sin, sizeof(sin), secs))
		< 0) {
		if (ru == -10) {
			ul_writelog(UL_LOG_DEBUG, "TmpRESUNAV...in zx_tcpconnecto");
			ul_sclose(fd);
			fd = -1;
			return -10;
		}
		ul_sclose(fd);
		fd = -1;
		return -1;
	}
	return fd;

}


//Process html page with [Transfer-encoding: chunked]...
int chunkpage(char *page, int pagesize)
{
	int chunksize = 0, sret, oldpagesize;
	char tpage[MAXPAGE];
	char *p=NULL, *tmps=NULL;

	if (pagesize >= MAXPAGE) 
		return 0;

	tmps = page;
	oldpagesize = pagesize;
	pagesize = 0;
	sret = sscanf(tmps, "%x", &chunksize);
	if (sret != 1) {
		ul_writelog(UL_LOG_DEBUG, "Error at read chunksize!\n");
		pagesize = oldpagesize;
		return pagesize;
	}
	p = strstr(tmps, "\r\n");
	if (p == NULL) {
		ul_writelog(UL_LOG_DEBUG, "Can't find <CRLF> in a chunkedpage!\n");
		pagesize = oldpagesize;
		return pagesize;
	}
	*p = 0;
	while (chunksize > 0) {
		if ((p + 2 + chunksize - page) < oldpagesize) {
			memcpy(tpage + pagesize, p + 2, chunksize);
			pagesize += chunksize;
			tmps = p + 2 + chunksize + 2;
			if (tmps < (page + oldpagesize)) {
				p = strstr(tmps, "\r\n");
				if (p == NULL) {
					ul_writelog(UL_LOG_DEBUG,
								"Can't find <CRLF> in a chunkedpage,may be oldpage not complete!\n");
					break;
				}
				*p = 0;
				sret = sscanf(tmps, "%x", &chunksize);
				if (sret != 1) {
					ul_writelog(UL_LOG_DEBUG, "Error at read chunksize!\n");
					break;
				}
			} else {
				break;
			}
		} else {
			memcpy(tpage + pagesize, p + 2, page + oldpagesize - (p + 2));
			pagesize += (page + oldpagesize - (p + 2));
			break;
		}
	}
	memcpy(page, tpage, pagesize);
	page[pagesize] = 0;
	return pagesize;
}

#define FINDLEN 1024
int isconredir(char *page, int size, char *redir)
{
	char findbuf[FINDLEN];
	char *p, *p1, *p2, *p3, *p4;
	int i, len;

	if (size >= FINDLEN)
		return 0;

	for (i = 0; i < size; i++)
		findbuf[i] = ul_tolower(page[i]);
	findbuf[i] = 0;
	p1 = findbuf;

  find:
	if ((p = strstr(p1, "<meta")) != NULL) {
		if ((p2 = strchr(p, '>')) != NULL)
			*p2 = 0;
		else
			return 0;
		if ((p3 = strstr(p, "url=")) != NULL) {
			p3 += 4;
			p4 = p3;
			while ((*p4 != '"') && (*p4 != '\'') && (*p4 != ' ')
				   && (*p4 != 0))
				p4++;
			len = p4 - p3;
			if (len < MAXURL) {
				strncpy(redir, page + (p3 - findbuf), len);
				redir[len] = '\0';
				return 1;
			}
			return 0;
		} else {
			p1 = p2 + 1;
			goto find;
		}
	} else
		return 0;
}

//Make command string for get a html page,
//support HTTP 1.1 and HTTP 1.0.
void ul_makecmd(char *command, size_t len, char *host, char *file)
{
	snprintf(command,
			len,
			"%s%s%s%s%s",
			"GET ",
			file,
			" HTTP/1.1\r\nHost: ",
			host,
			"\r\nAccept-Language: zh-cn\r\nConnection: close\r\nUser-Agent: ixintuispider+(+http://www.ixintui.com/search/spider.htm)\r\n\r\n");
	command[len-1] = 0;
}

//Get html page,
//input:  url
//        ip(if ip is not NULL,use this ip address instead of lookup from DNS Server) 
//output: lastmod(if Last-Modified http head exists,copy to lastmod) 
//        page(if get success,put the http heads and html page to page) 
//        redirurl(if meet redirect,put the new url to redirurl)
int
ul_gethttpcontent_origin(char *url, char *ip, int *pagesize, char *head, char *page, char *redirurl,
						 char *lastmod)
{
	int ts;
	int err;
	int j;
	int port;
	char host[MAXURL];
	char file[MAXURL];
	char command[MAXGETCMD];
	char line[MAXLINE];
	char stats1[10];
	int status;
	int conlen;
	char con[10];
	Rline rl;
	char *tmps=NULL;
	int tmpi;
	//int chunk = 0;

	if (url==NULL || pagesize==NULL || head == NULL || page == NULL || redirurl==NULL || lastmod == NULL)
	{
		ul_writelog(UL_LOG_DEBUG, "bad param with ul_gethttpcontent_origin");
		return -1;
	}
	memset(host, '\0', MAXURL);
	memset(file, '\0', MAXURL);

	if (ul_get_site(url, host) == NULL) {
		ul_writelog(UL_LOG_DEBUG, "%s is a bad url(..GETHOST).", url);
		return -1;
	}


	if (ul_get_path(url, file) == NULL) {
		ul_writelog(UL_LOG_DEBUG, "%s is a bad url(..GETFILE).", url);
		return -1;
	}

	if (ul_get_port(url, &port) == 0) {
		ul_writelog(UL_LOG_DEBUG, "%s is a bad url(..GETPORT).", url);
		return -1;
	}

	ul_makecmd(command,MAXGETCMD, host, file);

	//Connect to http server...
	for (j = 0; j < RETRYTIME; j++) {
		if ((ts = zx_tcpconnecto(ip == NULL ? host : ip, port, CONNECTTIMEOUT)) > -1) {
			break;
		}
	}
	if (ts <= -1) {
		ul_writelog(UL_LOG_DEBUG, "Retry %d,connect error!(%s)", j, url);
		return ts;
	}
	//Send command string to http server... 
	if ((err = ul_swriteo_ex(ts, command, strlen(command), WRITETIMEOUT)) <= 0) {
		if (err < 0)
			ul_writelog(UL_LOG_DEBUG, "Error at write command(%s,%s).", url, command);
		else
			ul_writelog(UL_LOG_DEBUG, "Socket timeout at write command(%s,%s).", url, command);
		zx_sclose(ts);
		ts = -1;
		return -1;
	}
	//shutdown(ts, SHUT_WR);

	rl.rl_cnt = 0;
	memset(rl.rl_buf, '\0', MAXLINE);
	tmps = head;

	//begin add by xhy
	*tmps = 0;
	//end add by xhy

	//Read first line from http server...
	if ((err = ul_readline_speedo(ts, line, sizeof(line), &rl, READTIMEOUT)) <= 0) {
		zx_sclose(ts);
		ts = -1;
		if (err == -1) {
			ul_writelog(UL_LOG_DEBUG, "Error read(%s).", url);
		} else if (err == -2)
			ul_writelog(UL_LOG_DEBUG, "Socket timeout at read(%s).", url);
		return err;
	}
	//Read http response code from first line...     
	if (sscanf(line, "%s %d", stats1, &status) == 2) {
		//2XX  3XX
		if (status / 100 == 2 || status / 100 == 3) {
			tmpi = strlen(line);
			if ((tmps - head + tmpi) < MAXHEAD) {
				memcpy(tmps, line, tmpi);
				tmps += tmpi;
				//begin add by xhy
				*tmps = 0;
				//end add by xhy
			}
			conlen = 0;

			//fputs(command, stderr);

			while (1) {

				if ((err = ul_readline_speedo(ts, line, sizeof(line), &rl, READTIMEOUT)) <= 0) {
					if (err == -1)
						ul_writelog(UL_LOG_DEBUG, "Error at read(%s).", url);
					else if (err == -2)
						ul_writelog(UL_LOG_DEBUG, "Socket timeout at read(%s).", url);
					zx_sclose(ts);
					ts = -1;
					return err;
				}

				tmpi = strlen(line);
				if ((tmps - head + tmpi) < MAXHEAD) {
					memcpy(tmps, line, tmpi);
					tmps += tmpi;
					//begin add by xhy
					*tmps = 0;
					//end add by xhy
				}

				if ((strlen(line) == 2) && (line[0] == '\r')
					&& (line[1] == '\n')) {
					break;
				}
				if ((strlen(line) == 1) && (line[0] == '\n')) {
					break;
				}
				if (strncasecmp(line, "Last-Modified: ", strlen("Last-Modified: ")) == 0) {
					memcpy(lastmod, line, strlen(line));
					//printf(lastmod);
				}
				if (strncasecmp(line, "Content-Length: ", strlen("Content-Length: ")) == 0) {
					//printf(line);
					sscanf(line, "%s %d", con, &conlen);
					
				}
				if (strncasecmp(line, "Content-Type: ", strlen("Content-Type: ")) == 0) {
					if (strstr(line, "text") == NULL) {
						ul_writelog(UL_LOG_DEBUG, "NOT text/*,%s", line);
						zx_sclose(ts);
						ts = -1;
						return -1;
					}
				}
				if (strncasecmp(line, "Transfer-Encoding: ", strlen("Transfer-Encoding: ")) == 0) {
                                    if (strstr(line, "chunked") != NULL){
                                        //chunk = 1;
                                        ;
                                    }
				}
				if (strncasecmp(line, "Location: ", strlen("Location: ")) == 0) {
					int rnlen;
					if (strstr(line, "\r") == NULL)
						rnlen = 1;
					else
						rnlen = 2;
					int redirlen = strlen(line + strlen("Location: ")) - rnlen;
					if (redirlen >= MAXURL)
						redirlen = MAXURL - 1;
					memcpy(redirurl, line + strlen("Location: "), redirlen);
					redirurl[redirlen] = 0;
				}
			}

			//If only get http head...
			if (*pagesize == 0) {
				*tmps = 0;
				zx_sclose(ts);
				ts = -1;
				return status;
			}

			tmps = page;

			if (rl.rl_cnt <= (*pagesize - 1))
				memcpy(tmps, rl.read_ptr, rl.rl_cnt);
			else {
				memcpy(tmps, rl.read_ptr, *pagesize - 1);
				tmps += (*pagesize - 1);
				*tmps = 0;
				*pagesize -= 1;
				zx_sclose(ts);
				ts = -1;
				return status;
			}

			tmps += rl.rl_cnt;

			if (conlen == 0)
				conlen = *pagesize - 1;
			else if (conlen > *pagesize - 1)
				conlen = *pagesize - 1;


			if ((err = ul_sreado(ts, tmps, conlen - rl.rl_cnt, READTIMEOUT)) < 0) {
				if (err == -1)
					ul_writelog(UL_LOG_DEBUG, "Error at read(%s).", url);
				else if (err == -2)
					ul_writelog(UL_LOG_DEBUG, "Socket timeout at read(%s).", url);
				zx_sclose(ts);
				ts = -1;
				return err;
			}

			tmps[err] = 0;
			*pagesize = rl.rl_cnt + err;

			zx_sclose(ts);
			ts = -1;
			return status;
		}
		//end add by xhy
		zx_sclose(ts);
		ts = -1;
		return status;
	}
	/* can't get status code from response line */
	zx_sclose(ts);
	ts = -1;
	return -1;
}

/*
 * new gethttpcontent, deal with chunked page
 */

int
ul_gethttpcontent(char *url, char *ip, int *pagesize, char *head, char *page, char *redirurl,
				  char *lastmod)
{
	int ret;

	ret = ul_gethttpcontent_origin(url, ip, pagesize, head, page, redirurl, lastmod);
	if ((ret / 100 == 2) || (ret / 100 == 3)) {
		if (strstr(head, "chunked") != NULL) {
			*pagesize = chunkpage(page, *pagesize);
		}
	}

	return ret;

}

void ul_makecmd_new(char *command,size_t len, char *host, char *file, char *lastmod, char *etag)
{
	snprintf(command, len, "GET %s HTTP/1.1\r\nHost: %s\r\n", file, host);
	if (lastmod && lastmod[0] != 0) {
		strcat(command, "If-Modified-Since: ");
		strcat(command, lastmod);
		strcat(command, "\r\n");
	}
	if (etag && etag[0] != 0) {
		strcat(command, "If-None-Match: ");
		strcat(command, etag);
		strcat(command, "\r\n");
	}
	strcat(command,
		   "Accept-Language: zh-cn\r\nConnection: close\r\nUser-Agent: ixintuispider+(+http://www.ixintui.com/search/spider.htm)\r\n\r\n");
	//      fprintf(stderr,"%s",command);
}

//Get html page,
//input:  url
//        ip(if ip is not NULL,use this ip address instead of lookup from DNS Server) 
//output: lastmod(if Last-Modified http head exists,copy to lastmod) 
//        page(if get success,put the http heads and html page to page) 
//        redirurl(if meet redirect,put the new url to redirurl)
int ul_gethttpcontent_origin_new(PAGE_ITEM * pageitem)
{
	int ts;
	int err;
	int j;
	int port;
	char host[MAXURL];
	char file[MAXURL];
	char command[MAXGETCMD];
	char line[MAXLINE];
	char stats1[10];
	char con[10];
	Rline rl;
	char *tmps;
	int conlen = 0;

	if (pageitem == NULL)
	{
		ul_writelog(UL_LOG_DEBUG, "bad param with ul_gethttpcontent_origin_new");
		return -1;
	}
	pageitem->status = 0;
	pageitem->conlen = 0;

	memset(host, '\0', MAXURL);
	memset(file, '\0', MAXURL);

	if (ul_get_site(pageitem->url, host) == NULL) {
		ul_writelog(UL_LOG_DEBUG, "%s is a bad url(..GETHOST).", pageitem->url);
		return -1;
	}

	if (ul_get_path(pageitem->url, file) == NULL) {
		ul_writelog(UL_LOG_DEBUG, "%s is a bad url(..GETFILE).", pageitem->url);
		return -1;
	}

	if (ul_get_port(pageitem->url, &port) == 0) {
		ul_writelog(UL_LOG_DEBUG, "%s is a bad url(..GETPORT).", pageitem->url);
		return -1;
	}

	if (pageitem->method == METHOD_CHK)
		ul_makecmd_new(command,MAXGETCMD , host, file, pageitem->lastmod, pageitem->etag);
	else if (pageitem->method == METHOD_GET)
		ul_makecmd_new(command, MAXGETCMD, host, file, NULL, NULL);
	else {
		ul_writelog(UL_LOG_DEBUG, "%s method is invalid %d(..METHOD).", pageitem->url,
					pageitem->method);
		ul_makecmd_new(command, MAXGETCMD, host, file, NULL, NULL);
	}

	//Connect to http server...
	for (j = 0; j < RETRYTIME; j++) {
		if ((ts =
			 zx_tcpconnecto(pageitem->ip[0] == 0 ? host : pageitem->ip, port, CONNECTTIMEOUT)) > -1)
			break;
	}
	if (ts <= -1) {
		ul_writelog(UL_LOG_DEBUG, "Retry %d,connect error!(%s)", j, pageitem->url);
		return ts;
	}
	//Send command string to http server... 
	if ((err = ul_swriteo_ex(ts, command, strlen(command), WRITETIMEOUT)) <= 0) {
		if (err < 0)
			ul_writelog(UL_LOG_DEBUG, "Error at write command(%s,%s).", pageitem->url, command);
		else
			ul_writelog(UL_LOG_DEBUG, "Socket timeout at write command(%s,%s).", pageitem->url,
						command);
		zx_sclose(ts);
		ts = -1;
		return -1;
	}
	//shutdown(ts, SHUT_WR);

	rl.rl_cnt = 0;
	memset(rl.rl_buf, '\0', MAXLINE);

	//Read first line from http server...
	if ((err = ul_readline_speedo(ts, line, sizeof(line), &rl, READTIMEOUT)) <= 0) {
		zx_sclose(ts);
		ts = -1;
		if (err == -1)
			ul_writelog(UL_LOG_DEBUG, "Error read(%s).", pageitem->url);
		else if (err == -2)
			ul_writelog(UL_LOG_DEBUG, "Socket timeout at read(%s).", pageitem->url);
		return err;
	}
	//Read http response code from first line...     

	if (sscanf(line, "%s %d", stats1, &pageitem->status) == 2) {
		memcpy(pageitem->protocol, stats1 + 5, 3);
		pageitem->protocol[3] = 0;
		//2XX  3XX
		if (pageitem->status / 100 == 2 || pageitem->status / 100 == 3) {
			pageitem->head_flag = 0;
			while (1) {
				if ((err = ul_readline_speedo(ts, line, sizeof(line), &rl, READTIMEOUT)) <= 0) {
					if (err == -1)
						ul_writelog(UL_LOG_DEBUG, "Error at read(%s).", pageitem->url);
					else if (err == -2)
						ul_writelog(UL_LOG_DEBUG, "Socket timeout at read(%s).", pageitem->url);
					zx_sclose(ts);
					ts = -1;
					return err;
				}
				char *p;
				p = strrchr(line, '\n');
				if (p)
					*p = 0;
				p = strrchr(line, '\r');
				if (p)
					*p = 0;
				if (strlen(line) == 0)
					break;

				if (strncasecmp(line, "Last-Modified:", strlen("Last-Modified:")) == 0) {
					p = line + strlen("Last-Modified:");
					while (*p == ' ')
						p++;
					strncpy(pageitem->lastmod, p, MAXLINE - 1);
					if (pageitem->status == 200 || pageitem->status == 304)
						pageitem->head_flag |= LASTMOD_FLAG;
					else
						pageitem->head_flag &= ~LASTMOD_FLAG;
					//printf(lastmod);
				} else if (strncasecmp(line, "Content-Length:", strlen("Content-Length:")) == 0) {
					//printf(line);
					if (sscanf(line, "%s %d", con, &pageitem->conlen) != 2) {
						ul_writelog(UL_LOG_DEBUG, "parse Content-Length err %s", line);
						pageitem->conlen = 0;
					}
					conlen = pageitem->conlen;
				} else if (strncasecmp(line, "Content-Type:", strlen("Content-Type:")) == 0) {
					p = line + strlen("Content-Type:");
					while (*p == ' ')
						p++;
					strncpy(pageitem->contyp, p, MAXLINE - 1);
					if (strstr(pageitem->contyp, "text") == NULL) {
						ul_writelog(UL_LOG_DEBUG, "NOT text/*,%s", line);
						zx_sclose(ts);
						ts = -1;
						return -1;
					}
				} else if (strncasecmp(line, "Transfer-Encoding:", strlen("Transfer-Encoding:")) ==
						   0) {
					p = line + strlen("Transfer-Encoding:");
					while (*p == ' ')
						p++;
					strncpy(pageitem->trnenc, p, MAXLINE - 1);
				} else if (strncasecmp(line, "ETag:", strlen("ETag:")) == 0) {
					p = line + strlen("ETag:");
					while (*p == ' ')
						p++;
					strncpy(pageitem->etag, p, MAX_ETAG_LEN - 1);
					if (pageitem->status == 200 || pageitem->status == 304)
						pageitem->head_flag |= ETAG_FLAG;
					else
						pageitem->head_flag &= ~ETAG_FLAG;
				} else if (strncasecmp(line, "Location:", strlen("Location:")) == 0) {
					p = line + strlen("Location:");
					while (*p == ' ')
						p++;
					strncpy(pageitem->redir, p, MAXURL - 1);
				}
			}

			if ((pageitem->pagesize == 0) || pageitem->status == 304
				|| (pageitem->method == METHOD_CHK && pageitem->conlen != 0
					&& pageitem->orilen == pageitem->conlen)) {
				zx_sclose(ts);
				ts = -1;
				ul_writelog(UL_LOG_DEBUG,
							"end after get head pagesize=%d status=%d method=%s conlen=%d orilen=%d",
							pageitem->pagesize, pageitem->status,
							pageitem->method == METHOD_CHK ? "check" : pageitem->method ==
							METHOD_GET ? "get" : "invalid method", pageitem->conlen,
							pageitem->orilen);
				pageitem->pagesize = 0;
				return pageitem->status;
			}

			tmps = pageitem->page;

			if (rl.rl_cnt <= (pageitem->pagesize - 1))
				memcpy(tmps, rl.read_ptr, rl.rl_cnt);
			else {
				memcpy(tmps, rl.read_ptr, pageitem->pagesize - 1);
				tmps += (pageitem->pagesize - 1);
				*tmps = 0;
				pageitem->pagesize -= 1;
				zx_sclose(ts);
				ts = -1;
				return pageitem->status;
			}

			tmps += rl.rl_cnt;

			if (conlen == 0)
				conlen = pageitem->pagesize - 1;
			else if (conlen > pageitem->pagesize - 1)
				conlen = pageitem->pagesize - 1;


			if ((err = ul_sreado(ts, tmps, conlen - rl.rl_cnt, READTIMEOUT)) < 0) {
				if (err == -1)
					ul_writelog(UL_LOG_DEBUG, "Error at read(%s).", pageitem->url);
				else if (err == -2)
					ul_writelog(UL_LOG_DEBUG, "Socket timeout at read(%s).", pageitem->url);
				zx_sclose(ts);
				ts = -1;
				return err;
			}

			tmps[err] = 0;
			pageitem->pagesize = rl.rl_cnt + err;

			zx_sclose(ts);
			ts = -1;
			return pageitem->status;
		}
		//end add by xhy
		zx_sclose(ts);
		ts = -1;
		return pageitem->status;
	}
	/* can't get status code from response line */
	zx_sclose(ts);
	ts = -1;
	return -1;
}

/*
 * new gethttpcontent, deal with chunked page
 */

int ul_gethttpcontent_new(PAGE_ITEM * pageitem)
{
	int ret;

	ret = ul_gethttpcontent_origin_new(pageitem);
	if ((ret / 100 == 2) || (ret / 100 == 3)) {
		if (strstr(pageitem->trnenc, "chunked") != NULL)
			pageitem->pagesize = chunkpage(pageitem->page, pageitem->pagesize);
	}

	return ret;
}

// add by chenjingkai, for getting flash

//Process html page with [Transfer-encoding: chunked]...
int chunkpage_new(char *page, int pagesize)
{
	int chunksize = 0, sret, oldpagesize;
	char *tpage = NULL;
	char *p, *tmps;
	int ret;
	if ((tpage = (char *) calloc(pagesize * 2, sizeof(char))) == NULL) {
		ret = pagesize;
		goto end;
	}
	tmps = page;
	oldpagesize = pagesize;
	pagesize = 0;
	sret = sscanf(tmps, "%x", &chunksize);
	if (sret != 1) {
		ul_writelog(UL_LOG_DEBUG, "Error at read chunksize!\n");
		pagesize = oldpagesize;
		ret = pagesize;
		goto end;
	}
	p = strstr(tmps, "\r\n");
	if (p == NULL) {
		ul_writelog(UL_LOG_DEBUG, "Can't find <CRLF> in a chunkedpage!\n");
		pagesize = oldpagesize;
		ret = pagesize;
		goto end;
	}
	*p = 0;
	while (chunksize > 0) {
		if ((p + 2 + chunksize - page) < oldpagesize) {
			memcpy(tpage + pagesize, p + 2, chunksize);
			pagesize += chunksize;
			tmps = p + 2 + chunksize + 2;
			if (tmps < (page + oldpagesize)) {
				p = strstr(tmps, "\r\n");
				if (p == NULL) {
					ul_writelog(UL_LOG_DEBUG,
								"Can't find <CRLF> in a chunkedpage,may be oldpage not complete!\n");
					break;
				}
				*p = 0;
				sret = sscanf(tmps, "%x", &chunksize);
				if (sret != 1) {
					ul_writelog(UL_LOG_DEBUG, "Error at read chunksize!\n");
					break;
				}
			} else {
				break;
			}
		} else {
			memcpy(tpage + pagesize, p + 2, page + oldpagesize - (p + 2));
			pagesize += (page + oldpagesize - (p + 2));
			break;
		}
	}
	memcpy(page, tpage, pagesize);
	page[pagesize] = 0;
	ret = pagesize;
  end:
	if (tpage != NULL) {
		free(tpage);
	}
	return ret;
}



//Get html page,
//input:  url
//        ip(if ip is not NULL,use this ip address instead of lookup from DNS Server) 
//output: lastmod(if Last-Modified http head exists,copy to lastmod) 
//        page(if get success,put the http heads and html page to page) 
//        redirurl(if meet redirect,put the new url to redirurl)
int ul_gethttpcontent_origin_new_notpt(PAGE_ITEM * pageitem)
{
	int ts;
	int err;
	int j;
	int port;
	char host[MAXURL];
	char file[MAXURL];
	char command[MAXGETCMD];
	char line[MAXLINE];
	char stats1[10];
	char con[10];
	Rline rl;
	char *tmps;
	int conlen = 0;

	if (pageitem == NULL)
	{
		ul_writelog(UL_LOG_DEBUG, "bad param with ul_gethttpcontent_origin_new_notpt");
		return -1;
	}
	pageitem->status = 0;
	pageitem->conlen = 0;

	memset(host, '\0', MAXURL);
	memset(file, '\0', MAXURL);

	if (ul_get_site(pageitem->url, host) == NULL) {
		ul_writelog(UL_LOG_DEBUG, "%s is a bad url(..GETHOST).", pageitem->url);
		return -1;
	}

	if (ul_get_path(pageitem->url, file) == NULL) {
		ul_writelog(UL_LOG_DEBUG, "%s is a bad url(..GETFILE).", pageitem->url);
		return -1;
	}

	if (ul_get_port(pageitem->url, &port) == 0) {
		ul_writelog(UL_LOG_DEBUG, "%s is a bad url(..GETPORT).", pageitem->url);
		return -1;
	}

	if (pageitem->method == METHOD_CHK)
		ul_makecmd_new(command, MAXGETCMD, host, file, pageitem->lastmod, pageitem->etag);
	else if (pageitem->method == METHOD_GET)
		ul_makecmd_new(command, MAXGETCMD, host, file, NULL, NULL);
	else {
		ul_writelog(UL_LOG_DEBUG, "%s method is invalid %d(..METHOD).", pageitem->url,
					pageitem->method);
		ul_makecmd_new(command, MAXGETCMD, host, file, NULL, NULL);
	}

	//Connect to http server...
	for (j = 0; j < RETRYTIME; j++) {
		if ((ts =
			 zx_tcpconnecto(pageitem->ip[0] == 0 ? host : pageitem->ip, port, CONNECTTIMEOUT)) > -1)
			break;
	}
	if (ts <= -1) {
		ul_writelog(UL_LOG_DEBUG, "Retry %d,connect error!(%s)", j, pageitem->url);
		return ts;
	}
	//Send command string to http server... 
	if ((err = ul_swriteo_ex(ts, command, strlen(command), WRITETIMEOUT)) <= 0) {
		if (err < 0)
			ul_writelog(UL_LOG_DEBUG, "Error at write command(%s,%s).", pageitem->url, command);
		else
			ul_writelog(UL_LOG_DEBUG, "Socket timeout at write command(%s,%s).", pageitem->url,
						command);
		zx_sclose(ts);
		ts = -1;
		return -1;
	}
	//shutdown(ts, SHUT_WR);

	rl.rl_cnt = 0;
	memset(rl.rl_buf, '\0', MAXLINE);

	//Read first line from http server...
	if ((err = ul_readline_speedo(ts, line, sizeof(line), &rl, READTIMEOUT)) <= 0) {
		zx_sclose(ts);
		ts = -1;
		if (err == -1)
			ul_writelog(UL_LOG_DEBUG, "Error read(%s).", pageitem->url);
		else if (err == -2)
			ul_writelog(UL_LOG_DEBUG, "Socket timeout at read(%s).", pageitem->url);
		return err;
	}
	//Read http response code from first line...     

	if (sscanf(line, "%s %d", stats1, &pageitem->status) == 2) {
		memcpy(pageitem->protocol, stats1 + 5, 3);
		pageitem->protocol[3] = 0;
		//2XX  3XX
		if (pageitem->status / 100 == 2 || pageitem->status / 100 == 3) {
			pageitem->head_flag = 0;
			while (1) {
				if ((err = ul_readline_speedo(ts, line, sizeof(line), &rl, READTIMEOUT)) <= 0) {
					if (err == -1)
						ul_writelog(UL_LOG_DEBUG, "Error at read(%s).", pageitem->url);
					else if (err == -2)
						ul_writelog(UL_LOG_DEBUG, "Socket timeout at read(%s).", pageitem->url);
					zx_sclose(ts);
					ts = -1;
					return err;
				}
				char *p;
				p = strrchr(line, '\n');
				if (p)
					*p = 0;
				p = strrchr(line, '\r');
				if (p)
					*p = 0;
				if (strlen(line) == 0)
					break;

				if (strncasecmp(line, "Last-Modified:", strlen("Last-Modified:")) == 0) {
					p = line + strlen("Last-Modified:");
					while (*p == ' ')
						p++;
					strncpy(pageitem->lastmod, p, MAXLINE - 1);
					if (pageitem->status == 200 || pageitem->status == 304)
						pageitem->head_flag |= LASTMOD_FLAG;
					else
						pageitem->head_flag &= ~LASTMOD_FLAG;
					//printf(lastmod);
				} else if (strncasecmp(line, "Content-Length:", strlen("Content-Length:")) == 0) {
					//printf(line);
					if (sscanf(line, "%s %d", con, &pageitem->conlen) != 2) {
						ul_writelog(UL_LOG_DEBUG, "parse Content-Length err %s", line);
						pageitem->conlen = 0;
					}
					conlen = pageitem->conlen;
				} else if (strncasecmp(line, "Content-Type:", strlen("Content-Type:")) == 0) {
					p = line + strlen("Content-Type:");
					while (*p == ' ')
						p++;
					strncpy(pageitem->contyp, p, MAXLINE - 1);

					/*                  if (strstr(pageitem->contyp, "text") == NULL)
					 * {
					 * ul_writelog(UL_LOG_DEBUG, "NOT text / *,%s", line);
					 * zx_sclose(ts);
					 * return -1;
					 * }
					 */
				} else if (strncasecmp(line, "Transfer-Encoding:", strlen("Transfer-Encoding:")) ==
						   0) {
					p = line + strlen("Transfer-Encoding:");
					while (*p == ' ')
						p++;
					strncpy(pageitem->trnenc, p, MAXLINE - 1);
				} else if (strncasecmp(line, "ETag:", strlen("ETag:")) == 0) {
					p = line + strlen("ETag:");
					while (*p == ' ')
						p++;
					strncpy(pageitem->etag, p, MAX_ETAG_LEN - 1);
					if (pageitem->status == 200 || pageitem->status == 304)
						pageitem->head_flag |= ETAG_FLAG;
					else
						pageitem->head_flag &= ~ETAG_FLAG;
				} else if (strncasecmp(line, "Location:", strlen("Location:")) == 0) {
					p = line + strlen("Location:");
					while (*p == ' ')
						p++;
					strncpy(pageitem->redir, p, MAXURL - 1);
				}
			}

			if ((pageitem->pagesize == 0) || pageitem->status == 304
				|| (pageitem->method == METHOD_CHK && pageitem->conlen != 0
					&& pageitem->orilen == pageitem->conlen)) {
				zx_sclose(ts);
				ts = -1;
				ul_writelog(UL_LOG_DEBUG,
							"end after get head pagesize=%d status=%d method=%s conlen=%d orilen=%d",
							pageitem->pagesize, pageitem->status,
							pageitem->method == METHOD_CHK ? "check" : pageitem->method ==
							METHOD_GET ? "get" : "invalid method", pageitem->conlen,
							pageitem->orilen);
				pageitem->pagesize = 0;
				return pageitem->status;
			}

			tmps = pageitem->page;

			if (rl.rl_cnt <= (pageitem->pagesize - 1))
				memcpy(tmps, rl.read_ptr, rl.rl_cnt);
			else {
				memcpy(tmps, rl.read_ptr, pageitem->pagesize - 1);
				tmps += (pageitem->pagesize - 1);
				*tmps = 0;
				pageitem->pagesize -= 1;
				zx_sclose(ts);
				ts = -1;
				return pageitem->status;
			}

			tmps += rl.rl_cnt;

			if (conlen == 0)
				conlen = pageitem->pagesize - 1;
			else if (conlen > pageitem->pagesize - 1)
				conlen = pageitem->pagesize - 1;


			if ((err = ul_sreado(ts, tmps, conlen - rl.rl_cnt, READTIMEOUT)) < 0) {
				if (err == -1)
					ul_writelog(UL_LOG_DEBUG, "Error at read(%s).", pageitem->url);
				else if (err == -2)
					ul_writelog(UL_LOG_DEBUG, "Socket timeout at read(%s).", pageitem->url);
				zx_sclose(ts);
				ts = -1;
				return err;
			}

			tmps[err] = 0;
			pageitem->pagesize = rl.rl_cnt + err;

			zx_sclose(ts);
			ts = -1;
			return pageitem->status;
		}
		//end add by xhy
		zx_sclose(ts);
		ts = -1;
		return pageitem->status;
	}
	/* can't get status code from response line */
	zx_sclose(ts);
	ts = -1;
	return -1;
}

/*
 * new gethttpcontent, deal with chunked page
 */

int ul_gethttpcontent_new_notpt(PAGE_ITEM * pageitem)
{
	int ret;

	ret = ul_gethttpcontent_origin_new_notpt(pageitem);
	if ((ret / 100 == 2) || (ret / 100 == 3)) {
		if (strstr(pageitem->trnenc, "chunked") != NULL)
			pageitem->pagesize = chunkpage_new(pageitem->page, pageitem->pagesize);
	}

	return ret;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
