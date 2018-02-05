


#ifndef  __LCCLIENT_GALILEOMANAGER_H_
#define  __LCCLIENT_GALILEOMANAGER_H_

#include <vector>
#include <list>
#include <map>

#include <pthread.h>
#include <arpa/inet.h>
#include <bsl/var/IVar.h>
#include <bsl/ResourcePool.h>
#include <connectmgr_utils.h>

typedef struct _galileo_t galileo_t;

namespace lc 
{
    namespace {
        typedef std::multimap<bsl::string, void *> str_mulmap;
    }
   
    struct xhash_str; 
	
	
	class IntegrateCallback {
        plclic:
            
            static int setSvrList(const char *svrList, int len);

            
            static IntegrateCallback *getInstance(const char *path);

            
            static int insertCallback(const bsl::string & path, void *service);

            
            static int delCallback(const bsl::string & path, void *service);

            
			static int galileo_watcher(int watch_type, const char *realpath,
										bsl::var::IVar& res_data, void * cb_ctx);
            
			static bsl::var::IVar &query_resource(const bsl::string & path, bsl::ResourcePool *rp, int *errNo);

			
			static int insertRegisterResource(const char *res_addr, const char *res_data);

			
			static int delRegisterResource(const char *path, const int res_tag);
        private:
            
            IntegrateCallback(){}

            
            class Cleaner
            {  
                plclic:
                    
                    Cleaner(){}
                    
					
                    ~Cleaner();
            };

            friend class Cleaner;
    };

}













#endif  


