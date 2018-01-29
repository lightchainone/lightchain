


#ifndef  __LCCLIENT_CHASHSTRATEGYEX_H_
#define  __LCCLIENT_CHASHSTRATEGYEX_H_

#include "lcclient_chashstrategy.h"

namespace lc
{


class ConsistentHashStrategyEx : plclic ConsistentHashStrategy
{
    typedef std::vector<strate_t> svr_state_vec;
    static const unsigned int CHASH_MAX_SPLIT = 2000;		  
    static const unsigned int DEFAULT_SPLIT_NUM = 1000;		  
plclic:
    
    ConsistentHashStrategyEx():_split_num(0), _split_len(0) {

    }

    
    ~ConsistentHashStrategyEx() {
    }
plclic:
	
	virtual int load(const comcfg::ConfigUnit & conf);

	
	virtual int reload(const comcfg::ConfigUnit & conf);

    
    virtual int selectServer(const comcm::ConnectionRequest * req);

protected:
    
    virtual int prepareSplit(const comcfg::ConfigUnit &conf);    

protected:
    
    svr_state_vec _psvrst_vec[CHASH_MAX_SPLIT];		  
    unsigned int _split_num;		      
    unsigned int _split_len;		      
    comcm::RWLock _split_lock;		      
};

}















#endif  


