
#include "lcclient_galileomanager.h"

#include <galileo.h>
#include <lc_log.h>

#include "lcclient_manager.h"
#include "lcclient_define.h"

namespace lc
{
const int svrListLen = 1024;          

static IntegrateCallback *_instance = NULL;          
static comcm::RWLock _lock;                   
static str_mulmap _callbackMap;
static galileo_t *_galileo = NULL;                  
static char _svrList[svrListLen] = {'\0'};            

IntegrateCallback :: Cleaner :: ~Cleaner() {
	if(IntegrateCallback::getInstance("../log/zoo.pspui.log")) {
        delete IntegrateCallback::getInstance("../log/zoo.pspui.log");
        galileo_destroy(_galileo);
    }
}

static int parseRealPath(bsl::string realpath, bsl::string &addr)
{
	const char *c_path = realpath.c_str();
	if ('/' != c_path[0]) {
		LC_LOG_WARNING("realptah is error, realpath[%s]", c_path);
		return -1;
	}
	int ret = strrchr(c_path, '/') - c_path;
	if (0 == ret || 1 == ret || ret == ((int)realpath.size()-1)) {
		LC_LOG_WARNING("realpath is error, realpath[%s]", c_path);
		return -1;
	}
	char c_addr[ret+1];
	strncpy(c_addr, c_path, ret);
	c_addr[ret] = '\0';
	addr = c_addr;
    return 0;
}

int IntegrateCallback :: setSvrList(const char *svrList, int len)
{
    if (NULL == svrList) {
        return -1;
    }

    if (_svrList[0] != '\0') {
        LC_LOG_WARNING("setSvrList before, can't be set twice");
        return 0;
    }

    if (len >= svrListLen) {
        LC_LOG_WARNING("svrList len[%d] to big, max len is[%d]", len, svrListLen);
        return -1;
    }
    snprintf(_svrList, sizeof(_svrList), "%s", svrList);

    return 0;
}

IntegrateCallback *IntegrateCallback :: getInstance(const char *path)
{
    if (_svrList[0] == '\0') {
        return NULL;
    }
    if (NULL == _instance) {
        _instance = new (std::nothrow) IntegrateCallback;
        if (NULL == _instance) {
            LC_LOG_WARNING("new Instance error [%m]");
            return NULL;
        }
        _galileo = galileo_init(_svrList);
        if (NULL == _galileo) {
            LC_LOG_WARNING("galileo_init error, maybe ip is error:%s", path);
            delete _instance;
            _instance = NULL;
            return NULL;
        }
        if (ZOO_CONNECTED_STATE != zoo_state(_galileo->zh)) {
            sleep(1);
        }
        if (ZOO_CONNECTED_STATE != zoo_state(_galileo->zh)) {
            LC_LOG_WARNING("galiloe_init error, maybe zk server is not working");
            delete _instance;
            _instance = NULL;
            return NULL;
        }
        galileo_set_log(path, 4);
        static Cleaner cln;
    }

    return _instance;
}





int IntegrateCallback :: insertCallback(const bsl::string & path, void *service)
{
    LC_LOG_DEBUG("%s", __func__);

    if (NULL == service) {
        return -1;
    }
    comcm::AutoWrite autowrite(&_lock);

    str_mulmap ::iterator it1;
    for(it1=_callbackMap.begin(); it1!=_callbackMap.end(); it1++) {
        if ((*it1).first == path && ((*it1).second == service)) {
            LC_LOG_WARNING("[%s] [%p] is set callback", path.c_str(), service);
            return 0;
        }
    }
    int ret = galileo_watch_resource(_galileo, path.c_str(), galileo_watcher, NULL);
    if (GALILEO_E_OK != ret) {
        LC_LOG_WARNING("[%s] [%p] set watch to galileo error", path.c_str(), service);
        return -1;
    }
    std::pair<bsl::string, void*> tempPair(path, service);
    _callbackMap.insert(tempPair);
    return 0;
}


int IntegrateCallback :: delCallback(const bsl::string & path, void *service)
{
    if (NULL == service) {
        return -1;
    }

    comcm::AutoWrite autowrite(&_lock);
    str_mulmap :: iterator it1;
    for(it1=_callbackMap.begin(); it1!=_callbackMap.end(); it1++) {
        if ((*it1).first == path && ((*it1).second == service)) {
            _callbackMap.erase(it1);
            return 0;
        }
    } 
    return -1;
}

bsl::var::IVar &IntegrateCallback :: query_resource(const bsl::string & path,
											bsl::ResourcePool *rp, int *errNo)
{
    LC_LOG_DEBUG("%s", __func__);

    int tag = 0;
    if (NULL != _galileo) {
        while (ZOO_CONNECTED_STATE != zoo_state(_galileo->zh)) {
            tag ++;
            if (tag < GALILEO_QUERY_RETRY) {
                sleep(GALILEO_QUERY_SLEEP_S);
            } else {
                break;
            }
        }
        if (GALILEO_QUERY_RETRY == tag) {
            *errNo = -1;
            return bsl::var::Null::null;
        }
        
    }

	bsl::var::IVar &conf = rp->create<bsl::var::Dict>();
	int ret = galileo_query_resource(_galileo, path.c_str(), conf, *rp);
	if (GALILEO_E_OK != ret) {
		LC_LOG_WARNING("query path[%s] error", path.c_str());
		*errNo = -1;
        return bsl::var::Null::null;
	}

	*errNo = 0;
    return conf;
}

int IntegrateCallback :: galileo_watcher(int watch_type, const char *realpath,
										bsl::var::IVar& res_data, void * )
{
    

    if (NULL==realpath)   {
        return -1;
    }
    str_mulmap :: iterator it1;
	LcClientManager *currMgr = NULL;
    std::vector<LcClientManager *> mgr_vec;
    int ret = 0;
    int i = 0;
	int mgr_num = 0;
    bsl::ResourcePool rp;
    bsl::string addr;
    bsl::var::Ref service_conf;
    switch(watch_type){
        case GALILEO_ABSTRACT_CHANGED_EVENT:
            
            service_conf = IntegrateCallback::query_resource(realpath, &rp, &ret);
            if (0 != ret) {
                LC_LOG_WARNING("galileo query resource [%s] error", realpath);
                break;
            }
            _lock.read();
            for(it1=_callbackMap.begin(); it1!=_callbackMap.end(); it1++) {
                if (strcmp((*it1).first.c_str(), realpath) == 0) {
                    mgr_vec.push_back((LcClientManager*)((*it1).second));
                    
					
                }
            }
            _lock.unlock();

            mgr_num = mgr_vec.size();
            for (i=0; i<mgr_num; i++) {
                currMgr = mgr_vec[i];
				if (NULL != currMgr) {
					currMgr->reload_by_galileo_event(service_conf, realpath);
				} else {
					LC_LOG_WARNING("ENTITY_CHANGED_EVENT mgr is NULL; path[%s]", realpath);
				}
            }
            break;
        case GALILEO_ENTITY_CHANGED_EVENT:
            if (parseRealPath(realpath, addr) != 0) {
                LC_LOG_WARNING("parse %s to get addr error", realpath);
                break;
            }
            service_conf = IntegrateCallback::query_resource(addr, &rp, &ret);
            if (0 != ret) {
                LC_LOG_WARNING("galileo query resource [%s] error", addr.c_str());
                break;
            }
            _lock.read();
            for(it1=_callbackMap.begin(); it1!=_callbackMap.end(); it1++) {
                if (strcmp((*it1).first.c_str(), addr.c_str()) == 0) {
                    mgr_vec.push_back((LcClientManager*)((*it1).second));
                    
					
                }
            }
            _lock.unlock();

            mgr_num = mgr_vec.size();
            for (i=0; i<mgr_num; i++) {
                currMgr = mgr_vec[i];
				if (NULL != currMgr) {
					currMgr->reload_by_galileo_event(service_conf, addr.c_str());
				} else {
					LC_LOG_WARNING("ENTITY_CHANGED_EVENT mgr is NULL; path[%s]", realpath);
				}
            }
            break;
        case GALILEO_RESOURCE_UPDATE_EVENT:
            
            _lock.read();
            for(it1=_callbackMap.begin(); it1!=_callbackMap.end(); it1++) {
                if (strcmp((*it1).first.c_str(), realpath) == 0) {
                    mgr_vec.push_back((LcClientManager*)((*it1).second));
                    
					
                }
            }
            _lock.unlock();

            mgr_num = mgr_vec.size();
            for (i=0; i<mgr_num; i++) {
                currMgr = mgr_vec[i];
				if (NULL != currMgr) {
					currMgr->reload_by_galileo_event(res_data, realpath);
				} else {
					LC_LOG_WARNING("ENTITY_CHANGED_EVENT mgr is NULL; path[%s]", realpath);
				}
            }
            break;
        default:
            LC_LOG_WARNING("Get a invalid event [%d], path[%s]", watch_type, realpath);
            break;
    }

    return 0;
}

int galileo_callback(int watch_type, const char *realpath, 
	bsl::var::IVar& res_data, void *cb_ctx)
{

    

    if (NULL==realpath)   {
        return -1;
    }

    int ret = 0;
    int i = 0;

    bsl::ResourcePool rp;
    bsl::string addr;
    bsl::var::Ref service_conf;
	
	LcClientManager *lccm = (LcClientManager*)cb_ctx;

    switch(watch_type){
        case GALILEO_ABSTRACT_CHANGED_EVENT:
            
            service_conf = IntegrateCallback::query_resource(realpath, &rp, &ret);
            if (0 != ret) {
                LC_LOG_WARNING("galileo query resource [%s] error", realpath);
                break;
            }
			lccm->reload_by_galileo_event(service_conf, realpath);
            break;
			
        case GALILEO_ENTITY_CHANGED_EVENT:
            if (parseRealPath(realpath, addr) != 0) {
                LC_LOG_WARNING("parse %s to get addr error", realpath);
                break;
            }
            service_conf = IntegrateCallback::query_resource(addr, &rp, &ret);
            if (0 != ret) {
                LC_LOG_WARNING("galileo query resource [%s] error", addr.c_str());
                break;
            }
			lccm->reload_by_galileo_event(service_conf, realpath);
            break;
        case GALILEO_RESOURCE_UPDATE_EVENT:
			lccm->reload_by_galileo_event(service_conf, realpath);
            break;
        default:
            LC_LOG_WARNING("Get a invalid event [%d], path[%s]", watch_type, realpath);
            break;
    }

    return 0;
}





int IntegrateCallback :: insertRegisterResource(const char *res_addr, const char *res_data)
{
	if (NULL == res_addr || NULL == res_data || NULL == _galileo){
		LC_LOG_WARNING("[%s:%d] Failed to get a res_addr or a valid res_data",
				__FILE__, __LINE__);
		return -1;
	}

	int rc = 0;
	int grc = GALILEO_E_OK;

	bsl::ResourcePool rp;
	bsl::var::JsonDeserializer jd(rp);
	bsl::var::Dict json_var;

	try{
		json_var = jd.deserialize(res_data);

		if (!json_var.is_dict()){
			LC_LOG_WARNING("[%s:%d] Failed to get a valid dict", __FILE__, __LINE__);
			return -3;
		}

		
		if (json_var.get(GALILEO_CONF_GALILEO_IP).is_null()){
			char* tmpip = new char[LCCLIENT_IP_LEN];
			rc = get_local_conn_ip(tmpip, LCCLIENT_IP_LEN);
			if (0 == rc){
				bsl::var::IVar &ip_str = rp.create<bsl::var::String>(tmpip);
				json_var.set(GALILEO_CONF_GALILEO_IP, ip_str);
			}
			else{
				LC_LOG_WARNING("[%s:%d] Failed to get a valid host ip", 
							__FILE__, __LINE__);
			}
			delete [] tmpip;
		}

		
		if (json_var.get(GALILEO_CONF_GALILEO_HOST).is_null()){
			char* tmphostname = new char[LCCLIENT_HOSTNAME_STR_LEN];
			rc = gethostname(tmphostname, LCCLIENT_HOSTNAME_STR_LEN);
			if (0 == rc){
				bsl::var::IVar &hostname_str = rp.create<bsl::var::String>(tmphostname);
				json_var.set(GALILEO_CONF_GALILEO_HOST, hostname_str);
			}
			else{
				LC_LOG_WARNING("[%s:%d] Failed to get a valid host name",
							__FILE__, __LINE__);
			}
			delete [] tmphostname;
		}
	} catch(bsl::Exception &e) {
		LC_LOG_WARNING("JsonDeserializer error:%s", e.all());
		return -1;
	} catch(...){
		return -1;
	}

	rc = 0;
	int tag = 0;
	grc = galileo_register_resource(_galileo, res_addr, json_var, &tag);
	if (0 != grc){
		LC_LOG_WARNING("[%s:%d] Failed to register new resource at addr[%s], err[%d], msg[%s]",
					__FILE__, __LINE__, res_addr, grc, galileo_err2str(grc));
		rc = -3;
	}
	return (0 == rc)?tag:rc;
}

int IntegrateCallback :: delRegisterResource(const char *path, const int res_tag)
{
	if (NULL == path || 0 >= res_tag || NULL == _galileo){
    	LC_LOG_WARNING("Failed to get a valid res_addr, a valid res_tag");
    	return -1;
	}

	int rc = 0;
	int grc = galileo_delete_resource(_galileo, path, res_tag);
    if (0 != grc){
    	LC_LOG_WARNING("Failed to delete resource from resource server, err[%d], msg[%s]", 
                       grc, galileo_err2str(grc));
        rc = grc;
    }
	return rc;
}


}
















