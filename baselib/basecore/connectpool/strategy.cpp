
#include "strategy.h"
#include "connectmgr.h"

namespace comcm{

	Strategy :: Strategy(){
		_log = NULL;
		_mgr = NULL;
        _last_reload = 0;
	}

	Strategy :: ~Strategy(){
	}

	int Strategy :: selectServer(const ConnectionRequest * ){
		return 0;
	}

	int Strategy :: notify(Connection * , int ){
		return 0;
	}

    int Strategy :: update() {
        return 0;
    }

	int Strategy :: setManager(ConnectManager * mgr){ 
		if(NULL == mgr){
			return -1;
		}
		_mgr = mgr;
		_log = _mgr->getLogAdapter();
		return 0;
	}

	const ConnectManager * Strategy :: getManager(){
		return _mgr;
	}

	HealthyChecker :: HealthyChecker(){
		_mgr = NULL;
		_log = NULL;
		_single_thread = false;
        _worker.seconds = 0;
        _worker.checker = NULL;
        _worker.run = false;
	}

	HealthyChecker :: ~HealthyChecker(){
	}

	int HealthyChecker :: setManager(ConnectManager * mgr){ 
		if(NULL == mgr){
			return -1;
		}
		_mgr = mgr;
		_log = _mgr->getLogAdapter();
		return 0;
	}

	int HealthyChecker :: healthyCheck(){
		return 0;
	}
	
	void * thread_worker(void * pd){
        
        
        ul_logstat_t logstat;
        logstat.events = 16;
        logstat.to_syslog = 0;
        logstat.spec = 0;
        ul_openlog_r("HealthyChecker", &logstat);
		HealthyChecker :: AsyncWorker * p = (HealthyChecker :: AsyncWorker *)pd;
		int seconds = p->seconds;
		HealthyChecker * checker = p->checker;
		while(p->run){
			checker->healthyCheck();
			sleep(seconds);
		}
        ul_closelog_r(0);
		return NULL;
	}

	int HealthyChecker :: startSingleThread(int seconds){
		_log->debug("[%s:%d] Try to start a single thread to healthy check...",
                __FILE__, __LINE__);
        
        if (!_single_thread) {
            AsyncWorker * p = &_worker;
            p->seconds = seconds;
            p->checker = this;
            p->run = true;
            if ( 0 == pthread_create(&_tid, NULL, thread_worker, p) ){
                _single_thread = true;
                _log->trace("[%s:%d] Start new thread for HealthyChecker successful.",
                        __FILE__, __LINE__);
                return 0;
            }
        }
		return -1;
	}

	void HealthyChecker :: joinThread(){
        
		if(_single_thread){
			_worker.run = false;
		    pthread_join(_tid, NULL);
            _single_thread = false;
		}
	}

	const ConnectManager * HealthyChecker :: getManager(){
		return _mgr;
	}

}

















