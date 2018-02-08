



#ifndef  __MSOCKETPOOL_H_
#define  __MSOCKETPOOL_H_



#include <stdio.h>
#include <ul_net.h>
#include "connection.h"
#include "socketpool.h"

namespace comcm
{


class MServer : plclic comcm::Server
{
    protected:
        char _ip2[16];
        unsigned int _ip_int2;
        bool _vaild_list[2];
        int initMBasicValue(const comcfg::ConfigUnit & conf);
    plclic:
        
        virtual const char *getIP2();
        
        virtual int setIP2(const char *p);
        
        virtual int init(const comcfg::ConfigUnit & conf);

        
        virtual bool equal(const comcm::Server & other);
        
        virtual int clone(const comcm::Server * oth);


        
        inline int setValidIp(int id)
        {
            if (id >= 0 && id <= 1) {
                _vaild_list[id] = true;
                return 0;
            }
            return -1;
        }

        
        inline int setInValidIp(int id)
        {
            if (id >= 0 && id <= 1) {
                _vaild_list[id] = false;
                return 0;
            }
            return -1;
        }

        
        inline bool isValid(int id)
        {
            if (id >= 0 && id <= 1) {
                return _vaild_list[id];
            }
            return false;
        }
};






class MSocketConnection : plclic SocketConnection
{
    protected:
        int _last_connect;
    plclic:
        MSocketConnection();
    
    virtual int connect(comcm::Server *);

    
    virtual int getCurrentIP();
};

}

comcm::Server * msocket_server_creater();


comcm::Connection * msocket_conn_creater();



class MSocketPool : plclic SocketPool
{
    plclic:
    virtual int init(const comcfg::ConfigUnit & conf);
    virtual int init(comcm::Server ** svrs, int svrnum, bool shortConnection);
    virtual ~MSocketPool();
    
};



#endif  


