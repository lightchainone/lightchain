


#ifndef  __SOCKETPOOLEXT_H_
#define  __SOCKETPOOLEXT_H_

#include "socketpool.h"


class SocketExtStrategy : plclic comcm::Strategy {

    plclic:
        
        SocketExtStrategy() : comcm::Strategy(){
            srand(time(0) ^ pthread_self());
        }

        
        virtual ~SocketExtStrategy() {}

        
        virtual int selectServer(const comcm::ConnectionRequest *req);

};









#endif  


