
#ifndef  __LCCLIENT_CONNECTION_H_
#define  __LCCLIENT_CONNECTION_H_

#include <vector>

#include <bsl/containers/string/bsl_string.h>
#include <connection.h>
#include <bsl/var/implement.h>
#include <bsl/ResourcePool.h>
#include <socketpool.h>
#include <Configure.h>
#include <bsl/var/IVar.h>

#include "lcclient_struct.h"
#include "lcclient_nshead.h"

namespace lc {

    
    class LcClientConnection : plclic SocketConnection {
        plclic:
            
            LcClientConnection() : _last_connect(-1), _tid(0), _finish_no(-1), _curr_no(-1),
                    _read_status(0){
                pthread_mutex_init(&_mutex, NULL);
                pthread_cond_init(&_cond, NULL);
                _in_event = new (std::nothrow)LcClientInDuSepEvent;
            }
            
            
            ~LcClientConnection(){
                pthread_mutex_destroy(&_mutex);
                pthread_cond_destroy(&_cond);
                _in_event->release();
            }

            
            int connect(comcm::Server *);

            
            int setHandle(int fd);

            int getIntFD();
           
            
            int getCurrIP();

            
            void setCurrIP(int num) {_last_connect = num;}

            
            void setTID(pthread_t id){_tid = id;}

            
            pthread_t getTID(){return _tid;}

            LcClientInDuSepEvent *getInEvent(){return _in_event;}
        protected:
            int _last_connect;        
            pthread_t _tid;       

            lc::MLock _lock;
            long long _finish_no;
            long long _curr_no;

            pthread_mutex_t _mutex;
            pthread_cond_t _cond;

            int _read_status;

            
            LcClientInDuSepEvent *_in_event;
    };


}







#endif  


