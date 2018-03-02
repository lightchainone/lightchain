#ifndef _BUF_STREAM_H_
#define _BUF_STREAM_H_

#include "BufHandle.h"

_KYLIN_PACKAGE_BEGIN_

class CBufReadStream
{
	friend class CBufReadStream_test;
	BufHandle *m_pCur;
	const BufHandle *m_pGuard;
	char* m_pBuf;
	int m_nLen;
plclic:
	CBufReadStream(BufHandle *pBegin, const BufHandle *pNext) {
		m_pCur = pBegin;
		m_pGuard = pNext;
		if (pBegin) {
			m_pBuf = pBegin->pBuf;
			m_nLen = pBegin->nDataLen;
		}
		else {
			m_nLen = 0;
		}
	}
	~CBufReadStream() {}

	bool Seek(int n) {
		int x = n;

		while (m_pCur != m_pGuard) {
			if (m_nLen >= x) {
				m_pBuf += x;
				m_nLen -= x;
				return true;
			}

			x -= m_nLen;
			m_pCur = m_pCur->_next;
			if (m_pCur != m_pGuard) {
				m_pBuf = m_pCur->pBuf;
				m_nLen = m_pCur->nDataLen;
			}
            else {
                return false;
            }
		}
		return false;
	}

    bool SeekToZero(OPTIONAL OUT int *pnLen) {
        int n = 0;
        while (m_pCur != m_pGuard) {
            n += m_nLen;
            for (; m_nLen>0 && *m_pBuf; m_pBuf++, m_nLen--);
            
            if (m_nLen > 0) {
                m_nLen --;
                m_pBuf ++;

                if (pnLen)
                    *pnLen = n - m_nLen;
                return true;
            }
            
            m_pCur = m_pCur->_next;
            if (m_pCur != m_pGuard) {
                m_pBuf = m_pCur->pBuf;
                m_nLen = m_pCur->nDataLen;
            }
            else {
                return false;
            }
        }
        return false;
    }

	BufHandle* GetCurrentHdl() {
		if (m_pCur != m_pGuard) {
			if (m_nLen == 0) {
				m_pCur = m_pCur->_next;
				if (m_pCur != m_pGuard) {
					m_pBuf = m_pCur->pBuf;
					m_nLen = m_pCur->nDataLen;
				}
			}
			else if (m_nLen < m_pCur->nDataLen) {
				m_pCur = SplitBufHdl(m_pCur, m_pCur->nDataLen-m_nLen);
			}
#ifdef	_DEBUG
			else {
				ASSERT(m_nLen == m_pCur->nDataLen);
			}
#endif
		}
		return m_pCur;
	}

	bool GetPascalString(OUT char* pBuf, int nBufSize, OPTIONAL OUT int* pnLen=NULL) {
		int n = 4;
		if (4 == GetBuffer((char*)&n, 4) 
			&& n <= nBufSize
			&& n == GetBuffer(pBuf, n)
        ) {
            if (pnLen)
                *pnLen = n+4;
            return true;
        }
        return false;
	}

	bool GetPascalString(OUT BufHandle** ppHdl, OUT BufHandle** ppNext) {
		int n = 4;
		if (4!=GetBuffer((char*)&n, 4))
			return false;

		*ppHdl = GetCurrentHdl();
		if (Seek(n)) {
			*ppNext = GetCurrentHdl();
			return true;
		}
		return false;
	}

    template <typename T>
    bool GetObject(OUT T* p) {
        return sizeof(T) == GetBuffer((char*)p, sizeof(T));
    }

	int GetBuffer(OUT char* pBuf, int nLen) {
		int n = 0;

		for ( ; nLen>0 && m_pCur!=m_pGuard; ) {
			if (nLen <= m_nLen) {
				memcpy(pBuf+n, m_pBuf, nLen);
				m_nLen -= nLen;
				n += nLen;

				m_pBuf += nLen;
				return n;
			}

			memcpy(pBuf+n, m_pBuf, m_nLen);
			nLen -= m_nLen;
			n += m_nLen;

			m_pCur = m_pCur->_next;
			if (m_pCur != m_pGuard) {
				m_pBuf = m_pCur->pBuf;
				m_nLen = m_pCur->nDataLen;
			}	
		}
		return n;
	}

	bool SkipToken() {
		while (m_pCur!=m_pGuard) {
			while (m_nLen > 0)  {
				if (*m_pBuf==' ' || *m_pBuf=='\r') {
					m_pBuf ++;
					m_nLen --;
					return true;
				}

				m_pBuf ++;
				m_nLen --;
			}

			m_pCur = m_pCur->_next;
			if (m_pCur!=m_pGuard) {
				m_pBuf = m_pCur->pBuf;
				m_nLen = m_pCur->nDataLen;
			}
		}
		return false;
	}

	
	bool GetToken(OUT char* pBuf, INOUT int *pnLen) {
		int nLen = *pnLen;
		int n = 0;

		*pnLen = 0;
		while (n<nLen && m_pCur!=m_pGuard) {
			while (m_nLen > 0)  {
				char c = *m_pBuf;
				m_pBuf ++;
				m_nLen --;

				if (c!=' ' && c!='\r') {
					pBuf[n++] = c;
					if (n == nLen) {
						*pnLen = n;
						return false;
					}
				}
				else  {
					pBuf[n++] = 0;
					*pnLen = n;
					return true;
				}
			}

			m_pCur = m_pCur->_next;
			if (m_pCur) {
				m_pBuf = m_pCur->pBuf;
				m_nLen = m_pCur->nDataLen;
			}
		}
		*pnLen = n;
		return false;
	}

	bool SeekToken(char* pToken) {
		int n = (int)strlen(pToken);
		int x = 0;
		BufHandle *pSave;
		char* pSaveBuf;
		int nSaveLen;

		while (m_pCur != m_pGuard) {
			while (m_nLen > 0)  {
				char c = *m_pBuf;
				m_pBuf ++;
				m_nLen --;

				if (c == pToken[x]) {
					if (x == 0) {
						pSave = m_pCur;
						pSaveBuf = m_pBuf-1;
						nSaveLen = m_nLen+1;
					}
					if (++x == n) {
						m_pCur = pSave;
						m_pBuf = pSaveBuf;
						m_nLen = nSaveLen;
						return true;
					}
				}
				else {
					x = 0;
				}
			}
			m_pCur = m_pCur->_next;
			if (m_pCur) {
				m_pBuf = m_pCur->pBuf;
				m_nLen = m_pCur->nDataLen;
			}
		}
		return false;
	}

	
	bool GetLine(OUT char* pBuf, INOUT int *pnLen) {
		int nLen = *pnLen;
		int n = 0;
        char c;

		
		

		*pnLen = 0;
		while (n<nLen && m_pCur!=m_pGuard) {
			while (m_nLen > 0)  {
				c = *m_pBuf;
				m_pBuf ++;
				m_nLen --;

                if ('\n' == c) {
                    if (1<=n && '\r'==pBuf[n-1]) {
                        pBuf[--n] = 0;
                    }
                    else {
                        pBuf[n] = 0;
                    }
                    *pnLen = n;
                    return true;
                }
                else {
                    pBuf[n++] = c;
                    if (n == nLen) {
                        *pnLen = n;
                        return false;
                    }
                }
				
			}
			m_pCur = m_pCur->_next;
			if (m_pCur) {
				m_pBuf = m_pCur->pBuf;
				m_nLen = m_pCur->nDataLen;
			}
		}
		*pnLen = n;
		return false;
	}

	bool GetUInt32(OUT uint32 *pnInt) {
		uint32 n = 0;
		bool bFlag = false;
		while (m_pCur != m_pGuard) {
			while (m_nLen > 0)  {
				char c = *m_pBuf;
				m_pBuf ++;
				m_nLen --;

				if (c!= ' ' && c!='\r') {
					if (c<='9' && c>='0') {
						n = n*10 + (c - '0');
						bFlag = true;
					}
					else {
						m_pBuf --;
						m_nLen ++;

						*pnInt = n;
						return false;
					}
				}
				else if (bFlag) {
					*pnInt = n;
					return true;
				}
			}

			m_pCur = m_pCur->_next;
			if (m_pCur) {
				m_pBuf = m_pCur->pBuf;
				m_nLen = m_pCur->nDataLen;
			}
		}
		*pnInt = n;
		return false;
	}
};


class CBufWriteStream
{
	friend class CBufWriteStream_test;

	BufHandle *m_pCur;
	BufHandle *m_pBegin;
	BufHandle *m_pGuard;
	char* m_pBuf;
	int m_nLen;
plclic:
	CBufWriteStream(BufHandle* pBegin=NULL, BufHandle* pNext=NULL) {
		if (pBegin == NULL) {
			m_pBegin = m_pCur = AllocateHdl(true);
			m_pBuf = m_pCur->pBuf;
			m_nLen = m_pCur->nBufLen;
		}
		else {
			m_pBegin = pBegin;
			for (m_pCur=m_pBegin; m_pCur->_next!=pNext; m_pCur=m_pCur->_next) ;
			m_pBuf = m_pCur->pBuf + m_pCur->nDataLen;
			m_nLen = m_pCur->nBufLen - m_pCur->nDataLen;
			ASSERT(m_nLen >= 0);
		}
		m_pCur->_next = m_pGuard = pNext;
	}
	~CBufWriteStream() {
		if (m_pBegin != m_pGuard) {
			ChainFreeHdl(m_pBegin, m_pGuard);
		}
	}

	int PutStringAsPascal(IN char* pBuf) {
		ASSERT(NULL != pBuf);
		int n = (int)strlen(pBuf) + 1;
		PutBuffer((char*)&n, 4);
		PutBuffer(pBuf, n);
        return n+4;
	}

	int PutBuffer(IN const char* pBuf, int nLen) {
        UNLIKELY_IF (nLen <= 0) {
			return 0;
        }
        ASSERT(NULL != pBuf);
        int nSaveLen = nLen;
		int n = (nLen > m_nLen) ? m_nLen : nLen;
		memcpy(m_pBuf, pBuf, n);
		m_pCur->nDataLen += n;
		m_pBuf += n;
		m_nLen -= n;

		if (n < nLen) {
			nLen -= n;
			m_pCur->_next = AllocateHdl(true, (char*)pBuf+n, nLen, m_pGuard);
			for (m_pCur=m_pCur->_next; m_pCur->_next!=m_pGuard; m_pCur=m_pCur->_next);

			m_pBuf = m_pCur->pBuf + m_pCur->nDataLen;
			m_nLen = m_pCur->nBufLen - m_pCur->nDataLen;
		}
        return nSaveLen;
	}

    template <typename T>
    int PutObject(const T& t) {
        PutBuffer((const char*)&t, sizeof(T));
        return sizeof(T);
    }

	int BackSeek(int nLen) {			
		ASSERT(nLen >= 0); 
		int n = 0;
		BufHandle *pHdl;
		while (m_pCur->nDataLen < nLen) {
			nLen -= m_pCur->nDataLen;
			n += m_pCur->nDataLen;

			if (m_pCur == m_pBegin) {
				m_pCur->nDataLen = 0;
				m_pBuf = m_pCur->pBuf;
				m_nLen = m_pCur->nBufLen;
				return n;
			}
			for (pHdl=m_pBegin; pHdl->_next!=m_pCur; pHdl=pHdl->_next) ;
			pHdl->_next = m_pGuard;
			FreeHdl(m_pCur);

			m_pCur = pHdl;
			m_nLen = m_pCur->nBufLen - m_pCur->nDataLen;
			m_pBuf = m_pCur->pBuf+m_pCur->nDataLen;
		}

		m_pCur->nDataLen -= nLen;
		m_pBuf -= nLen;
		m_nLen += nLen;
		n += nLen;
		if (0==m_pCur->nDataLen && m_pCur!=m_pBegin) {
			for (pHdl=m_pBegin; pHdl->_next!=m_pCur; pHdl=pHdl->_next) ;
			pHdl->_next = m_pGuard;
			FreeHdl(m_pCur);

			m_pCur = pHdl;
			m_nLen = m_pCur->nBufLen-m_pCur->nDataLen;
			m_pBuf = m_pCur->pBuf+m_pCur->nDataLen;
		}
		return n;
	}

	
	int Write(IN const char *fmt, ...) {
		va_list	ap;
		char buf[8192], *p;
		int n, m;
 
		p = buf;
		m = sizeof(buf);

		for (int i = 0; i < 2; ++i) {
			va_start(ap, fmt);
			n = vsnprintf(
				p,
				m,
				fmt,
				ap
				);
			va_end(ap);

			if (n > 0 && n < m) {
				PutBuffer(p, n);
				if (i > 0)
					free(p);
				break;
			}
			else {
				m = n+1;
				p = (char*)malloc(m);
			}
		}
		return n;
	}

	int PutBuffer(IN const char* pBuf) {
		ASSERT(NULL != pBuf);
		return PutBuffer(pBuf, (int)strlen(pBuf));
	}

	int BufferCat(IN BufHandle* pHdl, IN BufHandle* pNext) {
		
		int n = 0;
		if (NULL == pHdl)
			return 0;
		m_pCur->_next = pHdl;
		for (m_pCur=pHdl; m_pCur->_next!=pNext; m_pCur=m_pCur->_next) 
			n += m_pCur->nDataLen;
		n += m_pCur->nDataLen;
		m_pCur->_next = m_pGuard;

		
		
		m_pBuf = m_pCur->pBuf + m_pCur->nDataLen;
		m_nLen = m_pCur->nBufLen - m_pCur->nDataLen;
		ASSERT(m_nLen >= 0);
		return n;
	}

	BufHandle* GetBegin() {
		if (0 != m_pBegin->nDataLen) {
			BufHandle* pHdl = m_pBegin;
			m_pBegin = m_pGuard;
			m_pCur = m_pBegin; 
			m_nLen = 0;
			return pHdl;
		}
		return NULL;
	}
};

_KYLIN_PACKAGE_END_
#endif
