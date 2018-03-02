#include "stdafx.h"
#include "Kylin.h"
#include "UnitTest.h"
#include "Socket.h"

class CUDPClient : plclic CSocketV
{
    AsyncContext m_DelayCtx;
    char m_buf[4096];
    uint32 m_ip; 
    uint16 m_port;

    void OnBroken(APF_ERROR nErrCode) {
    }

    void OnWritten(APF_ERROR nErrCode, NetRequest* pReq) {
        if (0 != nErrCode) {
            TRACE0("Error: write failure %d\n", nErrCode);
        }
        free(pReq);
    }

    void _Read() {
        Read(m_buf, sizeof(m_buf), &m_Req);
    }

    void OnRead(APF_ERROR nErrCode, NetRequest* pReq) {
        if (APFE_NOTIFY == nErrCode) {
            _Read();
            delete pReq;
        }
        else if (0 == nErrCode) {
            m_buf[pReq->xfered] = 0;
            printf("[%s] from %s:%d\n", m_buf, in_ntoa(pReq->ip), pReq->port);
            _Read();
        }
        else {
            TRACE0("Error: read failure %d\n", nErrCode);
        }
    }

plclic:
    CUDPClient(uint32 ip, uint16 port) { 
        m_ip = ip;
        m_port = port;
        AddRef();
	    InitAsyncContext(&m_DelayCtx); 
    }
    ~CUDPClient() {
        TRACE0("~CTestSocket %p\n", this);
    }

    void Send() {
        NetRequest* pReq = ZeroAlloc<NetRequest>();
        WriteTo(m_ip, m_port, (void*)"hello", 5, pReq);
    }
};

void Usage()
{
    fprintf(stderr, "Usage: client server:port\n");
}

int main(int argc, char* argv[])
{
    if (2 > argc) {
        Usage();
        return -1;
    }
    uint32 ip; 
    uint16 port;

    InitKylin(1, 1, 5);
    KLSetLog(stderr, 0, NULL);

    char* p = strchr(argv[1], ':');
    if (NULL == p) {
        fprintf(stderr, "Error server address, %s\n", argv[1]);
        return -1;
    }   

    *p ++ = 0;
    ip = GetIp(argv[1]);
    port = (uint16)strtoul(p, NULL, 10);

    CUDPClient client(ip, port);
    client.Create(false);
    
    while (1) {
        client.Send();
        Sleep(10000);
    }
}
