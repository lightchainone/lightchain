#include <time.h>

#include <lc_log.h>

#include "lcclient_eventpool.h"

namespace lc {

    LcClientEventPool :: LcClientEventPool()
    {
        _createTag = 0;
        _event_pool = NULL;
    }

    LcClientEventPool :: ~LcClientEventPool()
    {
        if (1 == _createTag) {
            
            _event_map.destroy();
            _res_queue.destroy();
        }

        if (NULL != _event_pool) {
            delete _event_pool;
            _event_pool = NULL;
        }
    }

    int LcClientEventPool :: init()
    {
        if (NULL == _event_pool) {
            _event_pool = new (std::nothrow)lc::EventMonitorPool;
            if (NULL == _event_pool) {
                LC_LOG_WARNING("new EventMonitorPool error");
                return -1;
            }
        }

        if (0 == _createTag) {
            int ret = _event_map.create(LCCLIENT_HASHMAP_SIZE);
            if (0 != ret) {
                LC_LOG_WARNING("_event_map[hashmap] create error [%d] [%m]", ret);
                if (NULL != _event_pool) {
                    delete _event_pool;
                    _event_pool = NULL;
                }
                return -1;
            }
            ret = _res_queue.create();
            if (0 != ret) {
                LC_LOG_WARNING("_res_queue create error [%d] [%m]", ret);
                if (NULL != _event_pool) {
                    delete _event_pool;
                    _event_pool = NULL;
                }
                _event_map.destroy();
                return -1;
            }

            _createTag = 1;
        }

        return 0;
    }

    int LcClientEventPool :: pollAll()
    {
        if (0 == _event_map.size()) {
            return _res_queue.size();
        }
        const int waittime = 100;
        while (1) {
            poll_tv(waittime, 1);
            if (0 == _event_map.size()) {
                return _res_queue.size();
            }
        }

        return 0;
    }

    int LcClientEventPool :: cancelAll()
    {
        if (0 == _event_map.size()) {
            lcclient_talkwith_t *currTalk = fetchFirstReadyTalkAll();
            while (NULL != currTalk) {
                if (LCCLIENT_NSHEAD == currTalk->type) {
                    nshead_talkwith_t *talk = dynamic_cast<nshead_talkwith_t*>(currTalk);
                    if (NULL != talk) {
                        if (0 == talk->success) {
                            if (NULL != talk->freecallback && NULL != talk->resbuf) {
                                lc_callback_alloc_t allocarg;
                                allocarg.buff = talk->resbuf;
                                allocarg.size = talk->reshead.body_len;
                                allocarg.arg = talk->allocarg;
                                talk->freecallback(&allocarg);
                                talk->resbuf = NULL;
                            }
                        }
                    }
                }
                currTalk = fetchFirstReadyTalkAll();
            }
            return 0;
        } 
        
        prt_prt_map :: iterator it1;
        int ret = 0;
        for(it1=_event_map.begin(); it1!=_event_map.end(); it1++) {
            lc::SockEvent *ev = (*it1).first;
            if (NULL != ev && NULL != ev->reactor()) {
                ret = ev->reactor()->cancel(ev);
            }

        }

        while (1) {
            LC_LOG_DEBUG("poll");
            
            ret = poll_tv(LCCLIENT_MANAGER_WAITMSTIME);
            
            lcclient_talkwith_t *currTalk = NULL;
            nshead_talkwith_t *talk = NULL;
            if (ret > 0) {
                int i = 0;
                for(i=0; i<ret; i++) {
                    currTalk = fetchFirstReadyTalkAll();
                    if (NULL != currTalk) {
                        talk = dynamic_cast<nshead_talkwith_t*>(currTalk);
                        if (NULL == talk) {
                            continue;
                        } else if (0 == talk->success) {
                            if (NULL != talk->freecallback && NULL != talk->resbuf) {
                                lc_callback_alloc_t allocarg;
                                allocarg.buff = talk->resbuf;
                                allocarg.size = talk->reshead.body_len;
                                allocarg.arg = talk->allocarg;
                                talk->freecallback(&allocarg);
                                talk->resbuf = NULL;
                            }
                        }
                    } else {
                        break;
                    }
                }
            }
            if (0 == _event_map.size()) {
                while (!_res_queue.empty()) {
                    currTalk = fetchFirstReadyTalkAll();
                    if (NULL != currTalk) {
                        talk = dynamic_cast<nshead_talkwith_t*>(currTalk);
                        if (NULL == talk) {
                            continue;
                        } else if (0 == talk->success) {
                            if (NULL != talk->freecallback && NULL != talk->resbuf) {
                                lc_callback_alloc_t allocarg;
                                allocarg.buff = talk->resbuf;
                                allocarg.size = talk->reshead.body_len;
                                allocarg.arg = talk->allocarg;
                                talk->freecallback(&allocarg);
                                talk->resbuf = NULL;
                            }
                        }
                    } else {
                        break;
                    }
                }
                return 0;
            }

            timeval tm;
            tm.tv_sec = 0;
            tm.tv_usec = 100;
            select(0, NULL, NULL, NULL, &tm);
        }

        return 0;
    }
    int LcClientEventPool :: callEventMonitorCallBack(lc::SockEvent *event)
    {
        if (NULL == _event_pool || NULL == event) {
            LC_LOG_WARNING("input NULL");
            return -1;
        }
        
        
        lc::EventMonitorPool::callback(event, _event_pool);
        LC_LOG_DEBUG("eventpool callback over");
        return 0;
    }

    lc::SockEvent* LcClientEventPool :: setTalk(lcclient_talkwith_t *talk)
    {
        if (NULL == talk) {
            return NULL;
        }

        
        lc::SockEvent *event = new (std::nothrow)lc::SockEvent;
        if (NULL == event) {
            LC_LOG_WARNING("new SockEvent error");
            return NULL;
        }
        if (-1 == _event_map.set(event, talk)) {
            LC_LOG_WARNING("set event to map error");
            event->release();
            return NULL;
        }

        return event;
    }

    
    int LcClientEventPool :: poll_tv(int ms_secs, int tag)
    {
        if (0 == tag) {
            if (!_res_queue.empty()) {
                return _res_queue.size();
            }
        }

        if (_event_map.size() == 0 && _res_queue.empty()) {
            return lc::LC_WAIT_NO_MSG;       
        }

        const int eventNum = 8;
        lc::IEvent * eventPool[eventNum];
        if (NULL == eventPool) {
            LC_LOG_WARNING("new IEvent Vector error");
            return -1;
        }

        int ret = 0;
        if (ms_secs >= 0) {
            timeval tm;
            tvABS(&tm, ms_secs);
            ret = _event_pool->poll_tv(eventPool, eventNum, &tm);
            if (ret < 0) {
                return -1;
            } else if (ret == 0) {
                return 0;
            }
        } else {
            do {
                timeval tm;
                int temp_secs = 10;
                tvABS(&tm, temp_secs);
                ret = _event_pool->poll_tv(eventPool, eventNum, &tm);
            } while (ret <= 0);
        }

        getReadyTalk(ret, eventPool);

        ret = _res_queue.size();
        return ret;   
    }

    lcclient_talkwith_t *LcClientEventPool :: fetchFirstReadyTalkAll()
    {
        if (_res_queue.empty()) {
            return NULL;
        }

        lcclient_talkwith_t *resultTalk = _res_queue.front();
        _res_queue.pop_front();

        return resultTalk;
    }

    
    nshead_talkwith_t *LcClientEventPool :: fetchFirstReadyTalk()
    {
        if (_res_queue.empty()) {
            return NULL;
        }

        

        nshead_talkwith_t *resultTalk = NULL;
        talk_list :: iterator it1 = _res_queue.begin();
        for ( ;it1 != _res_queue.end(); it1++) {
            if ((*it1)->type == LCCLIENT_NSHEAD) {
                resultTalk = dynamic_cast<nshead_talkwith_t*>(*it1);
                _res_queue.erase(it1);
                break;
            }
        }

        return resultTalk;
    }

    common_talkwith_t *LcClientEventPool :: fetchFirstReadyTalkForCommon()
    {
        if (_res_queue.empty()) {
            return NULL;
        }
        
        common_talkwith_t *resultTalk = NULL;
        talk_list :: iterator it1 = _res_queue.begin();
        for ( ;it1 != _res_queue.end(); it1++) {
            if ((*it1)->type == LCCLIENT_COMMON) {
                resultTalk = dynamic_cast<common_talkwith_t*>(*it1);
                _res_queue.erase(it1);
                break;
            }
        }

        return resultTalk;
    }

    int LcClientEventPool :: poll_single(const lcclient_talkwith_t *talk, int ms_secs)
    {
        if (NULL == talk) {
            return -1;
        }

        if (!_res_queue.empty()) {
            talk_list :: iterator it1;
            for (it1=_res_queue.begin(); it1!=_res_queue.end(); it1++) {
                if (talk == (*it1)) {
                    _res_queue.erase(it1);
                    return 1;
                }
            }
        } else if (0 == _event_map.size()) {
            LC_LOG_DEBUG("no msg al");
            return lc::LC_WAIT_NO_MSG;
        }

        timeval tm;
        gettimeofday(&tm, NULL);
        int currTime = tm.tv_sec*1000 + tm.tv_usec/1000;
        int startTime = currTime;

        int lastTime;
        const int defaultWaitTime = 10; 
        if (ms_secs >= 0) {
            lastTime = ms_secs - (currTime-startTime);
        } else {
            lastTime = defaultWaitTime;
        }

        const int eventNum = 8;
        lc::IEvent *eventPool[eventNum];
        
        int ret = 0;
        int i = 0;
        timeval tm1;
        int findTag = 0;
        do {
            tvABS(&tm1, lastTime);
    
            ret = _event_pool->poll_tv(eventPool, eventNum, &tm1);
            if (ret <= 0) {
                gettimeofday(&tm, NULL);
                if (ms_secs >= 0) {
                    lastTime -= ((tm.tv_sec*1000+tm.tv_usec/1000) - (currTime));
                    currTime = tm.tv_sec*1000 + tm.tv_usec/1000;
                    continue;
                } else {
                    if (_event_map.size() == 0) {
                        return lc::LC_WAIT_NO_MSG;
                    }
                    lastTime = defaultWaitTime;
                    continue;
                }
            }
            
            for(i=0; i<ret; i++) {
                
                lcclient_talkwith_t *currTalk;
                if (_event_map.get(dynamic_cast<lc::SockEvent*>(eventPool[i]), &currTalk) 
                                                                            != bsl::HASH_EXIST) {
                    continue;
                }
                _event_map.erase(dynamic_cast<lc::SockEvent*>(eventPool[i]));
                eventPool[i]->release();
                
                if (NULL == currTalk) {
                    continue;
                }
                if (talk == currTalk) {
                    LC_LOG_DEBUG("find talk in map");
                    findTag = 1;
                } else {
                    _res_queue.push_back(currTalk);
                }
            }
            if (1 == findTag) {
                return 1;
            }
            gettimeofday(&tm, NULL);
            if (ms_secs >= 0) {
                lastTime -= ((tm.tv_sec*1000+tm.tv_usec/1000) - (currTime));
                currTime = tm.tv_sec*1000 + tm.tv_usec/1000;
            } else {
                lastTime = defaultWaitTime;
            }
        } while (lastTime > 0); 

        return 0;
    }

    int LcClientEventPool :: isTalkIn(const lcclient_talkwith_t *talk)
    {
        if (_res_queue.empty() && _event_map.size() == 0) {
            return 0;
        }

        talk_list :: iterator it1;
        for (it1=_res_queue.begin(); it1!=_res_queue.end(); it1++) {
            if (talk == (*it1)) {
                return 1;
            }
        }

        prt_prt_map :: iterator it2;
        for(it2=_event_map.begin(); it2!=_event_map.end(); it2++) {
            if (talk == (*it2).second) {
                return 1;
            }
        }

        return 0;
    }

    void LcClientEventPool :: getReadyTalk(int ret, lc::IEvent **eventPool)
    {
        int i = 0;
        lcclient_talkwith_t *currTalk;
        for(i=0; i<ret; i++) {
            if (_event_map.get(dynamic_cast<lc::SockEvent *>(eventPool[i]), &currTalk) != bsl::HASH_EXIST) {
                LC_LOG_WARNING(" poll(time) event not exist");
                continue;
            }
            _event_map.erase(dynamic_cast<lc::SockEvent *>(eventPool[i]));
            eventPool[i]->release();
            if (NULL == currTalk) {
                continue;       
            }
            
            _res_queue.push_back(currTalk);
        }

        return;
    }

    int LcClientEventPool :: setTalk(lc::SockEvent *event, lcclient_talkwith_t *talk)
    {
        if (-1 == _event_map.set(event, talk)) {
            LC_LOG_WARNING("set event to map error");
            return -1;
        }
        return 0;
    }

    int LcClientEventPool :: delTalk(lc::SockEvent *event)
    {
        _event_map.erase(event);
        return 0;
    }
}


