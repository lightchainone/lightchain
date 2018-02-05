



#ifndef  __LCCLIENTEVENTPOOL_H_
#define  __LCCLIENTEVENTPOOL_H_

#include <lc/lc2.h>

#include "lcclient_struct.h"
#include "lcclient_manager.h"

namespace lc {

    namespace {
        typedef bsl::list<lcclient_talkwith_t*> talk_list;
        typedef bsl::hashmap<lc::SockEvent*, lcclient_talkwith_t*> prt_prt_map;
    }
    
    
    class LcClientEventPool {
        plclic:
            
            LcClientEventPool();
            
            
            ~LcClientEventPool();

            
            int init();

            
            int callEventMonitorCallBack(lc::SockEvent *event);    

            
            lc::SockEvent* setTalk(lcclient_talkwith_t *talk);

            
            int setTalk(lc::SockEvent *event, lcclient_talkwith_t *talk);

            
            int poll_tv(int ms_secs = 0, int tag = 0); 

            
            nshead_talkwith_t *fetchFirstReadyTalk();

            
            common_talkwith_t *fetchFirstReadyTalkForCommon();

            
            int poll_single(const lcclient_talkwith_t *talkresult, int ms_secs = 0);

            
            int pollAll();

            
            int cancelAll(); 

            
            int isTalkIn(const lcclient_talkwith_t *talk);

            
            int delTalk(lc::SockEvent *event);

            
            lcclient_talkwith_t *fetchFirstReadyTalkAll();
        protected:
            
            void getReadyTalk(int ret, lc::IEvent **eventPool);

        protected:
            int _createTag;                                 
            talk_list _res_queue;                         
            lc::EventMonitorPool *_event_pool;              
            prt_prt_map _event_map;                         
    };

}












#endif  


