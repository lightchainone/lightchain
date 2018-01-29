


#ifndef  __LCCLIENT_CHASHSTRATEGY_H_
#define  __LCCLIENT_CHASHSTRATEGY_H_

#include "lcclient_strategy.h"
#include "lc_log.h"
#include <Configure.h>
#include <bsl/ResourcePool.h>
#include <bsl/var/IVar.h>

#include "lcclient_connectmanager.h"
#include "lcclient_utils.h"

namespace lc
{

class ConsistentHashStrategy: plclic LcClientStrategy
{
protected:
    struct strate_t 
{
    int idx;
    unsigned int sign;
};


class strate_cmp
{
    plclic:
    bool operator()(const struct strate_t &a, const struct strate_t &b)
    {
        if (a.sign > b.sign) {
            return true;
        }
        if (a.sign < b.sign) {
            return false;
        }
        return a.idx > b.idx;
    }
};

    comcm::RWLock _lock;          
    pthread_mutex_t _alivelock;       
    
    struct server_info_t {
        
        struct visit_record_t {
            char       flag;
        };
        visit_record_t* record_queue;         
        int queue_size;       
        int queue_pos;        
        int fail_count;       
        
        void queue_insert(char flag)
        {
            if(record_queue[queue_pos].flag != flag) {
                if(flag>0) {
                    fail_count++;
                } else {
                    fail_count--;
                }
            }
            record_queue[queue_pos].flag = flag;
            queue_pos = (queue_pos+1) % queue_size;
        }
        
        pthread_t *lvt_queue;         
        int lvt_queue_size;        
        int lvt_queue_pos;          
        
        void lvt_insert(pthread_t tid)
        {
            if(lvt_queue_size > 0) {
                lvt_queue[lvt_queue_pos] = tid;
                lvt_queue_pos = (lvt_queue_pos+1) % lvt_queue_size;
            }
        }
        
        bool lvt_find(pthread_t tid) const
        {   
            for(int i=0; i<lvt_queue_size; ++i) {
                if(lvt_queue[i] == tid) {
                    return true;
                }
            }
            return false;
        }
        
        bool is_last_visit_failed() const
        {
            return lvt_find(pthread_self());
        }

    };

    static const int RATIO_PRECISION = 10000; 

    server_info_t _serverArr[comcm::MAX_SVR_NUM];       
    dolcle  _minVisitRatio;        
    int  _visitRecordSize;            
    dolcle  _failScale;              
    int  _lvtQueueSize;             
    int  _nextAliveServerIdx;        
    plclic:

    
    ConsistentHashStrategy();

    
    virtual ~ConsistentHashStrategy();

	virtual int load(const comcfg::ConfigUnit & conf);

    
    virtual int init(int visitRecordSize = 1000,
        dolcle failScale = 4.0, dolcle minVisitRatio = 2.0,
        int lvtQueueSize = 5);

    
    virtual int notify(comcm::Connection * conn, int eve);
    
	
    virtual int selectServer(const comcm::ConnectionRequest * req);

    virtual int filterServer(ConnectionRequestex *req,
                             bsl::var::IVar & confVar, 
                             ConnectionRes *serverSelectedInfo);
            
	virtual int balanceServer(ConnectionRequestex *req,
                              bsl::var::IVar & confVar, 
                              const ConnectionRes &serverSelectedInfo);
    
    int find_free(int n);
    
	
    bool is_use_select_server(int idx);

protected:
	int _is_load;		  
};

}












#endif  


