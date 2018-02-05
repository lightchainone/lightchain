#include "lcclient_manager.h"

#include <lc_log.h>

#include "lcclient_eventpool.h"
#include "lcclient_nshead.h"
#include "lcclient_strategy.h"
#include "lcclient_healthystrategy.h"
#include "lcclient_rangestrategy.h"
#include "lcclient_healthycheck.h"
#include "lcclient_ioc.h"
#include "lcclient_connectpool.h"
#include "lcclient_connectserver.h"

#include "lcclient_chashstrategy.h"
#include "lcclient_selfstrategy.h"
#include "lcclient_rstrategy.h"
#include "lcclient_simplechashstrategy.h"
#include "lcclient_exphashstrategy.h"
#include "lcclient_chashstrategyex.h"
#include "lcclient_filterchashstrategy.h"

#include "bsl/AutoBuffer.h"
#include "bsl/var/JsonSerializer.h"




namespace lc {

    typedef struct _client_monitor_info {
        LcClientManager *lcmgr;
        bsl::string service_name;
    } client_monitor_info;

    typedef std::pair<bsl::string, bsl::string> path_sev_pair;

    LCCLIENT_GEN_IOC_CREATE(LcClientStrategy);
    LCCLIENT_GEN_IOC_CREATE(LcClientHealthyChecker);
    LCCLIENT_GEN_IOC_CREATE(LcClientRangeStrategy);
    LCCLIENT_GEN_IOC_CREATE(LcClientStrategyWithHealthy);
    LCCLIENT_GEN_IOC_CREATE(ConsistentHashStrategy);
    LCCLIENT_GEN_IOC_CREATE(ConsistentHashStrategyEx);
    LCCLIENT_GEN_IOC_CREATE(LcClientSelfStrategy);
    LCCLIENT_GEN_IOC_CREATE(LcClientRStrategy);
    LCCLIENT_GEN_IOC_CREATE(SimpleConsistentHashStrategy);
    LCCLIENT_GEN_IOC_CREATE(FilterConsistentHashStrategy);
    LCCLIENT_GEN_IOC_CREATE(ExpHashStrategy);

    static ResourceCleaner g_rc;
    static strategy_ioc *g_LcClientStrategyIoc = NULL;              
    static healthychecker_ioc *g_LcClientHealthyCheckerIoc = NULL;  

    static int LcClient_Register_IOC() {
        strategy_ioc *strategyIoc = lcclient_get_strategyioc();
        healthychecker_ioc *healthCheckerIoc = lcclient_get_healthycheckerioc();
        if (NULL == strategyIoc) {
            LC_LOG_WARNING("create strategyIoc is NULL");
            return 0;
        }
        if (NULL == healthCheckerIoc) {
            LC_LOG_WARNING("create healthCheckerIoc is NULL");
            return 0;
        }

        LCCLIENT_ADD_IOC_CLASS(strategyIoc, LcClientStrategy);
        LCCLIENT_ADD_IOC_CLASS(healthCheckerIoc, LcClientHealthyChecker);

        LCCLIENT_ADD_IOC_CLASS(strategyIoc, LcClientRangeStrategy);
        LCCLIENT_ADD_IOC_CLASS(strategyIoc, LcClientStrategyWithHealthy);
        LCCLIENT_ADD_IOC_CLASS(strategyIoc, ConsistentHashStrategy);
        LCCLIENT_ADD_IOC_CLASS(strategyIoc, ConsistentHashStrategyEx);
        LCCLIENT_ADD_IOC_CLASS(strategyIoc, LcClientSelfStrategy);
        LCCLIENT_ADD_IOC_CLASS(strategyIoc, LcClientRStrategy);
        LCCLIENT_ADD_IOC_CLASS(strategyIoc, SimpleConsistentHashStrategy);
        LCCLIENT_ADD_IOC_CLASS(strategyIoc, FilterConsistentHashStrategy);
        LCCLIENT_ADD_IOC_CLASS(strategyIoc, ExpHashStrategy);
        return 0;
    }

    static void deleteKeyValue(void *param)
    {
        LC_LOG_DEBUG("delete key value");
        LcClientEventPool *eventpool = (LcClientEventPool *)(param);
        if (NULL != eventpool) {
            delete eventpool;
            eventpool = NULL;
        }
    }

    
    ResourceCleaner :: ResourceCleaner()
    {
        LcClient_Register_IOC();
    }

    
    ResourceCleaner :: ~ResourceCleaner()
    {
        if (NULL != g_LcClientStrategyIoc) {
            delete g_LcClientStrategyIoc;
            g_LcClientStrategyIoc = NULL;
        }

        if (NULL != g_LcClientHealthyCheckerIoc) {
            delete g_LcClientHealthyCheckerIoc;
            g_LcClientHealthyCheckerIoc = NULL;
        }
    }

#define RETURN_IF_KEY_ERROR(conf, k, ret) \
	if(conf[k].selfType() == comcfg::CONFIG_ERROR_TYPE) { \
		LC_LOG_WARNING("Get config (%s) failed.", k); \
		return ret; \
	}
	
#define RETURN_IF_NO_VALUE(conf, func, k, v, ret) \
	if(conf[k].func(&v) != 0) { \
		LC_LOG_WARNING("Get config (%s) failed.", k); \
		return ret; \
	}	

#define CONVERT_KEY(v1, v2, gk, ck, flag) \
	if (v2.get(gk).is_null()) { \
		if(flag) LC_LOG_WARNING("Get (%s) failed.", gk); \
	} else { \
		v1[ck] = v2[gk]; \
		v1.del(gk); \
	}
	
#define FAILED_IF_NULL_KEY(v, k) \
	if (v.get(k).is_null()) { \
		LC_LOG_WARNING("Get (%s) failed.", k); \
		goto FAILED; \
	}
	
#define FAILED_IF_NOT_DICT(v, k) \
	if (!v.is_dict()) { \
		LC_LOG_WARNING("(%s) isn't a dict.", k); \
		goto FAILED; \
	}



    LcClientManager :: LcClientManager(lc::NetReactor *reactor)
    {
        if (NULL != reactor) {
            _netreactor = reactor;
            _isOut = 1;
        } else {
            _netreactor = NULL;
            _isOut = 0;
        }

        _createTag = 0;
        _confType = LCCLIENT_CONFTYPE_LOCAL;

        memset(_galileoDir, 0, sizeof(_galileoDir));
        memset(_galileoFile, 0 ,sizeof(_galileoFile));

        _galileoRun = 1;
        _galileoConfSave = NULL;
        _galileoConfSaveLen = 0;

        _healthy_run = 0;
        _healthyCheckTime = 5000;

        _galileoBufferSize = GALILEO_BUFFER_SIZE;

        _monitor = NULL;
        _monitor_threadnum = 0;
        _monitor_info = bsl::var::Null::null;

		m_new_version = false;
    }

    LcClientManager :: ~LcClientManager()
    {
    	std::vector<cluster_wrap_t *>::iterator idx;
		for(idx = m_cluster_vector.begin(); idx != m_cluster_vector.end(); idx++) {
			cluster_wrap_t *wrap = (cluster_wrap_t*)*idx;
			
			
			free(wrap);
		}
		
        if (1 == _healthy_run) {
            stop_healthy_check();
        }

        if (NULL != _netreactor && 0 == _isOut) {
            close();
            delete _netreactor;
            _netreactor = NULL;
        }

        if (1 == _createTag) {
            exitThreadUsingInsidePool();
            pthread_key_delete(_poolkey);

            deleteConnMap();
            deleteServiceMap();
            deleteRegRes();       

            if (NULL != _galileoConfSave) {
                delete []_galileoConfSave;
                _galileoConfSave = NULL;
            } 
        }
    }

    int LcClientManager :: setReactor(lc::NetReactor *reactor)
    {
        if (NULL == reactor) {
            return -1;
        }

        if (1 == _isOut) {
            LC_LOG_WARNING("reactor has been set");
            return -1;
        } else if (NULL != _netreactor) {
            LC_LOG_WARNING("reactor has been inited");
            return -1;
        }

        _netreactor = reactor;
        _isOut = 1;

        return 0;
    }

    int LcClientManager :: init(const char *dir, const char *file)
    {
        if (NULL == dir || NULL == file) {
            LC_LOG_WARNING("dir or file is NULL");
            return -1;
        }

        int ret;
        
        if (0 == _createTag) {
            if (initResource() != 0) {
                LC_LOG_WARNING("create resource error");
                return -1;
            }            

            _service_rwlock.write();
            
            comcfg::Configure conf;
            ret = conf.load(dir, file);
            if (ret != 0) {
                LC_LOG_WARNING("main conf file load error: %s, %s", dir, file);
                goto Fail;
            }
            if (conf[LCCLIENT_CONF_CONFIG].selfType() == comcfg::CONFIG_ERROR_TYPE) {
                LC_LOG_WARNING("main conf file [%s/%s] not exist [%s]", dir, file, 
                                LCCLIENT_CONF_CONFIG);
                goto Fail;
            }
            if (0 != innerLoad(conf[LCCLIENT_CONF_CONFIG])) {
                goto Fail;
            }
            _createTag = 1;
            _service_rwlock.unlock();
            return 0;
        } else {
            return 0;
        }
    Fail:
        _createTag = 0;
        _service_rwlock.unlock();
        return -1;
    }

int LcClientManager::init_reload(
	bsl::var::IVar &conf, 
	const char *servname, bsl::ResourcePool &rp, 
	bsl::var::IVar &galileoConfVar, 
	const char *proxyname, const char *sname)
{
    int errNo = 0;
    const char *temp_name = NULL;
    if (strcmp(proxyname, "") == 0) {
        temp_name = sname;
    } else {
        temp_name = proxyname;
    }

    bsl::var::IVar & resultVar = convertMcpackToVar(conf, rp, temp_name, errNo);    

    if (0 != errNo) {
        LC_LOG_WARNING("convert galileo var to service var error : [%d]", errNo);
        return -1;
    }

    if (!resultVar.is_dict()) {
        LC_LOG_WARNING("to var, result is not dict");
        return -1;
    }
    LC_LOG_DEBUG("begin load conf");

    bsl::var::Dict currVar;
    try {
        currVar[LCCLIENT_CONF_SERVICE] = resultVar;
    } catch (bsl::Exception& e) {
        LC_LOG_WARNING("error [%s]", e.what());
        return -1;
    } catch (std::exception& e){
        LC_LOG_WARNING("error [%s]", e.what());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("error unknown");
        return -1;
    }
        
    comcfg::Configure currConf;
    int ret = currConf.loadIVar(currVar);
    if (0 != ret) {
        LC_LOG_WARNING("construct conf from var error : [%d]", ret);
        return -1;
    }

    LcClientPool *pool = NULL;
    if (0 != getService(servname, &pool)) {
        LC_LOG_WARNING("two services have the same name : %s", servname);
        return -1;
    }
    ret = pool->reload(currConf[LCCLIENT_CONF_SERVICE]);
    if (0 != ret) {
        LC_LOG_WARNING("init LcClientPool error");
        return -1;
    }
    try {
        int currSize = galileoConfVar.size();
        galileoConfVar.set(currSize, resultVar);
    } catch (bsl::Exception& e) {
        LC_LOG_WARNING("error [%s]", e.what());
        return -1;
    } catch (std::exception& e){
        LC_LOG_WARNING("error [%s]", e.what());
        return -1;
    } catch (...) {
        LC_LOG_WARNING("error unknown");
        return -1;
    }

    LC_LOG_DEBUG("init[pack] over");
    return 0;
}

void LcClientManager :: merge_galileo_local_conf(
	const comcfg::ConfigUnit &local_conf, 
	const char *type, bsl::ResourcePool &rp, 
	bsl::var::IVar &resultVar)
{
    if (local_conf[type].selfType() != comcfg::CONFIG_ERROR_TYPE) {
		try {
        	resultVar[type] = rp.create<bsl::var::Int32>(local_conf[type].to_int32());
		} catch (bsl::Exception& e) {
			LC_LOG_WARNING("error [%s]", e.what());
		} catch (std::exception& e){
			LC_LOG_WARNING("error [%s]", e.what());
		} catch (...) {
			LC_LOG_WARNING("error unknown");
		}
    }
}

int LcClientManager :: merge_galileo_local_conf2(
	const comcfg::ConfigUnit &local_conf, 
	const char *type, bsl::ResourcePool &rp, 
	bsl::var::IVar &resultVar) 
{
    if (local_conf[type].selfType() != comcfg::CONFIG_ERROR_TYPE) {
        int errcode = 0;
        bsl::var::IVar &local_strategy = local_conf[type].to_IVar(&rp, &errcode);
        if (errcode != 0) {
            LC_LOG_WARNING("convert %s Conf to var error:%d", type, errcode);
            return -1;
        }
        resultVar[type] = local_strategy;
    }
    return 0;
}





int LcClientManager::init(bsl::var::IVar &galileo_conf, const char *servname, bsl::ResourcePool &rp, 
                            bsl::var::IVar &galileo_conf_array, const comcfg::ConfigUnit &local_conf, int merge_flag,
                            const char *proxyname, const char *sname)
{
    int errNo = 0;
    const char *service_name = NULL;
    if (strcmp(proxyname, "") == 0) {
        service_name = sname;
    } else {
        service_name = proxyname;
    }


	
	
    bsl::var::IVar & resultVar = convertMcpackToVar(galileo_conf, rp, service_name, errNo);    

    if (0 != errNo) {
        LC_LOG_WARNING("convertMcpackToVar failed, %d", errNo);
        return -1;
    }

    if (!resultVar.is_dict()) {
        LC_LOG_WARNING("Resource isn't dict.");
        return -1;
    }

    if (merge_flag != 0) {
        
        
        if (local_conf.selfType() == comcfg::CONFIG_ERROR_TYPE ||
            local_conf[LCCLIENT_CONF_SERVICE].selfType() == comcfg::CONFIG_ERROR_TYPE) {
            LC_LOG_WARNING("Service in local conf is not exist");
        } else {
			
				
            int local_service_num = local_conf[LCCLIENT_CONF_SERVICE].size();
            for (int i=0; i<local_service_num; i++) {
                const comcfg::ConfigUnit & local_service_conf = local_conf[LCCLIENT_CONF_SERVICE][i];
                if (local_service_conf[LCCLIENT_CONF_SERVICE_NAME].selfType() == comcfg::CONFIG_ERROR_TYPE) {
                    LC_LOG_WARNING("Service Name not exist in local Service:%d", i);
                    return -1;
                }
                if (strcmp(local_service_conf[LCCLIENT_CONF_SERVICE_NAME].to_cstr(), service_name) == 0) {
                    
                    


					try {
                        merge_galileo_local_conf(local_service_conf, LCCLIENT_CONF_SERVICE_CONNECT, rp, resultVar);
                        merge_galileo_local_conf(local_service_conf, LCCLIENT_CONF_SERVICE_READ, rp, resultVar);
                        merge_galileo_local_conf(local_service_conf, LCCLIENT_CONF_SERVICE_WRITE, rp, resultVar);
                        merge_galileo_local_conf(local_service_conf, LCCLIENT_CONF_SERVICE_MAXCONNECT, rp, resultVar);
                        merge_galileo_local_conf(local_service_conf, LCCLIENT_CONF_SERVICE_RETRY, rp, resultVar);

                        if (local_service_conf[LCCLIENT_CONF_SERVICE_CONNECTTYPE].selfType() != comcfg::CONFIG_ERROR_TYPE) {
                            resultVar[LCCLIENT_CONF_SERVICE_CONNECTTYPE] = rp.create<bsl::var::String>(local_service_conf[LCCLIENT_CONF_SERVICE_CONNECTTYPE].to_cstr());
                        }

                        merge_galileo_local_conf(local_service_conf, LCCLIENT_CONF_SERVICE_LINGER, rp, resultVar);
                        merge_galileo_local_conf(local_service_conf, LCCLIENT_CONF_SERVICE_WAITNUM, rp, resultVar);
                        merge_galileo_local_conf(local_service_conf, LCCLIENT_CONF_SERVICE_CONNALL, rp, resultVar);
                        merge_galileo_local_conf(local_service_conf, LCCLIENT_CONF_SERVICE_FETCHCONNTIME, rp, resultVar);
                        merge_galileo_local_conf(local_service_conf, LCCLIENT_CONF_SERVICE_SENDBUF, rp, resultVar);
                        merge_galileo_local_conf(local_service_conf, LCCLIENT_CONF_SERVICE_RECVBUF, rp, resultVar);

                        
                        if (merge_galileo_local_conf2(local_service_conf, LCCLIENT_CONF_STRATEGY, rp, resultVar) != 0) {
                            return -1;
                        }
                        if (merge_galileo_local_conf2(local_service_conf, LCCLIENT_CONF_HEALTHY, rp, resultVar) != 0) {
                            return -1;
                        }
                    } catch (bsl::Exception& e) {
			            LC_LOG_WARNING("error [%s]", e.what());
			            return -1;
			        } catch (std::exception& e){
			            LC_LOG_WARNING("error [%s]", e.what());
			            return -1;
			        } catch (...) {
			            LC_LOG_WARNING("error unknown");
			            return -1;
			        }
                }
            }
        }
    }

	
    bsl::var::Dict currVar;
    try {
        currVar[LCCLIENT_CONF_SERVICE] = resultVar;
    } catch (...) {
        LC_LOG_WARNING("error unknown");
        return -1;
    }

	
    comcfg::Configure currConf;
    int ret = currConf.loadIVar(currVar);
    if (0 != ret) {
        LC_LOG_WARNING("Conver IVar to comcfg failed, %d", ret);
        return -1;
    }

	
    LcClientPool *pool = new (std::nothrow)LcClientPool;
    if (NULL == pool) {
        LC_LOG_WARNING("new LcClientPool error [%m]");
        return -1;
    }

    ret = pool->init(currConf[LCCLIENT_CONF_SERVICE]);
    if (0 != ret) {
        LC_LOG_WARNING("init LcClientPool error");
        delete pool;
        return -1;
    }

    if (0 == findService(servname)) {
        LC_LOG_WARNING("two services have the same name : %s", servname);
        delete pool;
        return -1;
    }
    if (addService(servname, pool, proxyname, sname, 1) != 0 ) {
        delete pool;
        return -1;
    }

    try {
		
        int currSize = galileo_conf_array.size();
        galileo_conf_array.set(currSize, resultVar);
    } catch (...) {
        LC_LOG_WARNING("error unknown");
        return -1;
    }
    return 0;
}

bsl::var::IVar & LcClientManager :: convertMcpackToVar(
	bsl::var::IVar &galileo_var, 
	bsl::ResourcePool & rp, 
	const char *servname, int &errNo)
{
	LC_LOG_DEBUG("Enter convertMcpackToVar");

    static bsl::var::Null null_var;
    if (NULL == servname) {
        LC_LOG_WARNING("input error");
		goto FAILED;
    }
	
    try {
		FAILED_IF_NULL_KEY(galileo_var, GALILEO_CONF_GALILEO_ABSTRACT);
	
        bsl::var::IVar & abstract_var = galileo_var[GALILEO_CONF_GALILEO_ABSTRACT];
		FAILED_IF_NULL_KEY(abstract_var, GALILEO_CONF_GALILEO_SERVICE);

        bsl::var::IVar & services_var = abstract_var[GALILEO_CONF_GALILEO_SERVICE];
		FAILED_IF_NULL_KEY(services_var, servname);
	
        bsl::var::IVar & service_var = services_var[servname];
        LC_LOG_DEBUG("get conf over, currVar type : [%d]", service_var.is_dict());

		FAILED_IF_NOT_DICT(service_var, servname);

        bsl::var::IVar & tempNameVar = rp.create<bsl::var::String>(servname);
        service_var[LCCLIENT_CONF_SERVICE_NAME] = tempNameVar;
        service_var.del(GALILEO_CONF_GALILEO_SERVICE_NAME);

		CONVERT_KEY(service_var, service_var, GALILEO_CONF_GALILEO_SERVICE_PORT, LCCLIENT_CONF_SERVICE_PORT, true);
		CONVERT_KEY(service_var, service_var, GALILEO_CONF_GALILEO_CONNTYPE,     LCCLIENT_CONF_SERVICE_CONNECTTYPE, true);
		CONVERT_KEY(service_var, service_var, GALILEO_CONF_GALILEO_CONNTO,       LCCLIENT_CONF_SERVICE_CONNECT, true);
		CONVERT_KEY(service_var, service_var, GALILEO_CONF_GALILEO_READTO,       LCCLIENT_CONF_SERVICE_READ, true);
		CONVERT_KEY(service_var, service_var, GALILEO_CONF_GALILEO_WRITETO,      LCCLIENT_CONF_SERVICE_WRITE, true);


        
        FAILED_IF_NULL_KEY(galileo_var, GALILEO_CONF_GALILEO_ENTITY);

        bsl::var::IVar & entity_var = galileo_var[GALILEO_CONF_GALILEO_ENTITY];
		FAILED_IF_NOT_DICT(entity_var, GALILEO_CONF_GALILEO_ENTITY);

        
        bsl::var::DictIterator idx(entity_var.dict_begin());
        int i = 0;
		
        bsl::var::IVar & server_array = rp.create<bsl::var::Array>();
		
        for(idx=entity_var.dict_begin();  idx!=entity_var.dict_end(); ++idx) {
                            
            bsl::var::IVar & entity_value_var = (*idx).value();
            bsl::string entity_name = (*idx).key();

            if (!entity_value_var.is_dict()) {   
                LC_LOG_WARNING("entity %d isn't a dict.", i);
                continue;
            }
            LC_LOG_DEBUG("enti[%d] name : [%s]", i, entity_name.c_str());

            if (entity_value_var.get(GALILEO_CONF_GALILEO_NAME).is_null()) {
                LC_LOG_WARNING("Get (%s) failed.", GALILEO_CONF_GALILEO_NAME);
            } else {
                entity_value_var.del(GALILEO_CONF_GALILEO_NAME);
            }

			CONVERT_KEY(entity_value_var, entity_value_var, GALILEO_CONF_GALILEO_IP,      LCCLIENT_CONF_SERVER_IP, true);
			CONVERT_KEY(entity_value_var, entity_value_var, GALILEO_CONF_GALILEO_VERSION, LCCLIENT_CONF_SERVER_VERSION, false);
			CONVERT_KEY(entity_value_var, entity_value_var, GALILEO_CONF_GALILEO_MASTER,  LCCLIENT_CONF_SERVER_MASTER, false);


            if (entity_value_var.get(GALILEO_CONF_GALILEO_RANGE).is_null()){
                
                ;
            } else {
                bsl::var::IVar & range_var = entity_value_var[GALILEO_CONF_GALILEO_RANGE];

                if (!range_var.is_dict()) {
                    LC_LOG_WARNING("enti[%d]'s range is not dict", i);
                    continue;
                }

				CONVERT_KEY(entity_value_var, range_var, GALILEO_CONF_GALILEO_RANGE_NAME, LCCLIENT_CONF_SERVER_RANGE, false);
            }

            server_array.set(i, entity_value_var);
            i++;
        }
        if (server_array.size() != 0) {
            service_var[LCCLIENT_CONF_SERVER] = server_array;
        }
        
        errNo = 0;
        return service_var;
		
    } catch (bsl::Exception& e) {
        LC_LOG_WARNING("convertMcpackToVar failed (%s)", e.what());
		goto FAILED;

    } catch (std::exception& e){
        LC_LOG_WARNING("convertMcpackToVar failed (%s)", e.what());
		goto FAILED;

    } catch (...) {
        LC_LOG_WARNING("convertMcpackToVar failed (unknown)");
		goto FAILED;
    }

FAILED:
    errNo = -1;
    return null_var;
}


static void add_service_if(bsl::var::Array& service_array, bsl::var::IVar& service_var)
{
	int i;
    int arraySize = service_array.size();


	
	

    try{
        for (i=0; i<arraySize; i++) {
            if (service_array[i][LCCLIENT_CONF_SERVICE_NAME].to_string() == 
                        service_var[LCCLIENT_CONF_SERVICE_NAME].to_string()) {
                LC_LOG_DEBUG("replace service (%s) in service array", 
						service_var[LCCLIENT_CONF_SERVICE_NAME].to_string().c_str());
                service_array[i] = service_var;
                break;
            }
        }
    } catch (bsl::Exception& e) {
        LC_LOG_WARNING("error [%s]", e.what());
        return;
    } catch (std::exception& e){
        LC_LOG_WARNING("error [%s]", e.what());
        return;
    } catch (...) {
        LC_LOG_WARNING("error unknown");
        return;
    }

    if (i == arraySize) {

		LC_LOG_DEBUG("add service (%s) to service array", 
						service_var[LCCLIENT_CONF_SERVICE_NAME].to_string().c_str());
		
        service_array.set(i, service_var);
    }
}


static int stream_to_conf(const char* confstream, int conflen, comcfg::Configure &conf) 
{
	char *buf = conf.getRebuildBuffer(conflen); 
	if (buf == NULL) {
		LC_LOG_WARNING("getRebuildBuffer return null");
		return -1;
	} else {
		snprintf(buf, conflen, "%s", confstream);
		return conf.rebuild();
	}	
}



static void merge_service_info(bsl::var::IVar &currvar, bsl::var::IVar &newvar)
{
	
	currvar.del(LCCLIENT_CONF_SERVER);

	
	if(!newvar.get(LCCLIENT_CONF_SERVER).is_null()) 
		currvar[LCCLIENT_CONF_SERVER] = newvar[LCCLIENT_CONF_SERVER];

	
	LC_LOG_DEBUG("=========DUMP service info==========");
	LC_LOG_DEBUG("%s",	currvar.dump(3).c_str());
	LC_LOG_DEBUG("====================================");
}



int LcClientManager::reload_by_galileo_event(
	bsl::var::IVar & gailieo_service_var, const char *path)
{
    if (NULL == path) {
        LC_LOG_WARNING("input error, path[%ld]", (long)path);
        return -1;
    }
    
    AutoMLock lock1(_depres_lock);
    if (0 == _galileoRun) {
        return -1;
    }
    if (0 == _createTag) {
        LC_LOG_WARNING("LcClientManager is in init, can't be reload");
        return -1;
    }
	
    bsl::ResourcePool rp;
	comcfg::Configure galileo_conf;
	bsl::var::Array service_array; 
	int ret;
	bool created = false;


	
	ret = stream_to_conf(_galileoConfSave, _galileoConfSaveLen, galileo_conf);
	if(ret == 0) {
		if(galileo_conf[LCCLIENT_CONF_LCCLIENT].selfType() != comcfg::CONFIG_ERROR_TYPE) {
			bsl::var::IVar& client_dict = galileo_conf[LCCLIENT_CONF_LCCLIENT].to_IVar(&rp, &ret);
			if(ret == 0) {
				service_array = client_dict.get(LCCLIENT_CONF_SERVICE);
				if(!service_array.is_null()) { 
					LC_LOG_DEBUG("found service from saved config");
					created = true;
				}
			}
		}
	} 
	if(!created) {
		LC_LOG_DEBUG("no service found from saved config");
		service_array = rp.create<bsl::var::Array>();
	}
	

	
	
	
	

    path_sev_map :: iterator it1;
    for (it1=_galileo_servmap.begin(); it1!=_galileo_servmap.end(); it1++) {
        if (0 != strcmp((*it1).first.c_str(), path)) {
            continue;
        }

		const char *service_name = (*it1).second.c_str();

        _service_rwlock.read();
    
		
        LcClientPool *service_pool = NULL;
        if (getService(service_name, &service_pool) != 0) {
            _service_rwlock.unlock();
            continue;
        }
        _service_rwlock.unlock();

		
        bsl::var::IVar & input_var = convertMcpackToVar(gailieo_service_var, rp, service_name, ret);
        if (0 != ret) {
            continue;
        }

		bsl::var::IVar& using_var = input_var;

		if(_confType == LCCLIENT_CONFTYPE_HYBRID2) {
		
			
			bsl::var::IVar &pool_var = service_pool->getConf(&rp, &ret);
			if(ret != 0)
				continue;

			
			merge_service_info(pool_var, input_var);
			using_var = pool_var;
		} 

		
        comcfg::Configure using_conf;
		if((ret = using_conf.loadIVar(using_var)) != 0) {
			LC_LOG_WARNING("used_conf loadIVar error [%d]", ret);
			continue;
		}

		
        ret = service_pool->reload(using_conf);
        if (0 != ret) {
            LC_LOG_WARNING("currPool reload(conf) error [%d]", ret);
            continue;
        }

		
		add_service_if(service_array, using_var);
    }

	
    saveGalileoCopy(service_array);
    return 0;
}		

int LcClientManager :: init(const comcfg::ConfigUnit & conf)
{
    LC_LOG_DEBUG("init(conf)");

    
    

    if (conf[LCCLIENT_CONF_SERVICE].selfType() == comcfg::CONFIG_ERROR_TYPE) {
        LC_LOG_WARNING("service[%s] not exist", LCCLIENT_CONF_SERVICE);
        return -1;
    }
    int i = 0;
    int serviceNum = conf[LCCLIENT_CONF_SERVICE].size();
    for (i=0; i<serviceNum; i++) {
        LcClientPool *pool = new (std::nothrow)LcClientPool;
        if (NULL == pool) {
            LC_LOG_WARNING("new LcClientPool error [%m]");
            return -1;
        }

        int ret = pool->init(conf[LCCLIENT_CONF_SERVICE][i]);
        if (ret != 0) {
            delete pool;
            pool = NULL;
            LC_LOG_WARNING("init pool error");
            
            return -1;
        }

        bsl::string serviceName;
        if (conf[LCCLIENT_CONF_SERVICE][i]
            [LCCLIENT_CONF_SERVICE_NAME].get_bsl_string(&serviceName) != 0) {
            
            LC_LOG_WARNING("service name not exist in conf");
            delete pool;
            pool = NULL;
            return -1;
        }

        if (0 == findService(serviceName.c_str())) {
            LC_LOG_WARNING("two services have the same name : %s", serviceName.c_str());
            delete pool;
            pool = NULL;
            return -1;
        }
        
        if (addService(serviceName.c_str(), pool, "", "", 0) != 0) {
            delete pool;
            pool = NULL;
            return -1;
        }
    }
    return 0;
}

int LcClientManager :: initAfterGalileo(const comcfg::ConfigUnit & conf)
{
    LC_LOG_DEBUG("init(conf)");

    
    
    if (conf[LCCLIENT_CONF_SERVICE].selfType() == comcfg::CONFIG_ERROR_TYPE) {
        LC_LOG_WARNING("service[%s] not exist", LCCLIENT_CONF_SERVICE);
        return -1;
    }
    int i = 0;
    int serviceNum = conf[LCCLIENT_CONF_SERVICE].size();
    for (i=0; i<serviceNum; i++) {
        bsl::string serviceName;
        if (conf[LCCLIENT_CONF_SERVICE][i]
            [LCCLIENT_CONF_SERVICE_NAME].get_bsl_string(&serviceName) != 0) {
            
            LC_LOG_WARNING("service name not exist in conf");
            return -1;
        }

        if (0 == findService(serviceName.c_str())) {
            continue;
        }

        LcClientPool *pool = new (std::nothrow)LcClientPool;
        if (NULL == pool) {
            LC_LOG_WARNING("new LcClientPool error [%m]");
            return -1;
        }

        int ret = pool->init(conf[LCCLIENT_CONF_SERVICE][i]);
        if (ret != 0) {
            delete pool;
            pool = NULL;
            LC_LOG_WARNING("init pool error");
            
            return -1;
        }
        
        if (addService(serviceName.c_str(), pool, "", "", 0) != 0) {
            delete pool;
            pool = NULL;
            return -1;
        }
    }
    return 0;
}

    int LcClientManager :: nshead_talk(const char *servicename, nshead_talkwith_t *talkmsg[], 
                                       int count, bsl::var::IVar & confVar)
    {
        LC_LOG_DEBUG("nshead_talk");
        if (NULL == servicename || NULL == talkmsg || count <= 0) {
            return -1;
        }

        
        comcm::AutoRead auto_read(&_service_rwlock);
        LcClientPool *currPool = getManagerPool(servicename);
        if (NULL == currPool) {
            return -1;
        }

        
        for (int i=0; i<count; i++) {
            talkmsg[i]->servicename = servicename;
            postSingleTalkBlock(talkmsg[i], confVar, currPool, servicename);
        }
        
        return 0;
    }

    int LcClientManager :: nshead_simple_singletalk(const char *servicename, int hash_key,
                                                    nshead_t *reqhead, const void *reqbuff, 
                                                    nshead_t *reshead, void *resbuff,
                                                    u_int maxreslen)
    {
        LC_LOG_DEBUG("%s", __func__);

        if (NULL == servicename || NULL == reqhead || NULL == reqbuff || NULL == reshead ||
            NULL == resbuff) {
            return -1;
        }
        
        comcm::AutoRead autolock(&_service_rwlock);
        LcClientPool *currPool = getManagerPool(servicename);
        if (NULL == currPool) {
            return -1;
        }

        int errNo = 0;
        bsl::var::Dict confVar;
        default_server_arg_t defaultserverarg;
        defaultserverarg.key = hash_key;

        struct timeval fetch_start;
        struct timeval fetch_end;
        gettimeofday(&fetch_start, NULL);

        LcClientConnection *conn = currPool->fetchConnection(defaultserverarg, confVar, &errNo, this);
        gettimeofday(&fetch_end, NULL);

        if (NULL == conn) {
            LC_LOG_WARNING("nshead_talk : fetchConnection error");
            if (-1 != errNo) {
                return errNo;
            } else {
                return lc::NSHEAD_LC_INIT_ERROR;
            }
        }

        conn->setTID(pthread_self());
        int *strFD = (int*)(conn->getHandle());
        if (NULL == strFD) {
            LC_LOG_WARNING("nshead_talk: socket fd error");
            conn->setErrno(lc::NSHEAD_TCP_CONNECT_ERROR);
            currPool->freeConnection(conn);
            return lc::NSHEAD_TCP_CONNECT_ERROR;
        }
        int currFD = *strFD;
        if (currFD < 0) {
            LC_LOG_WARNING("nshead_talk: socket fd error");
            conn->setErrno(lc::NSHEAD_TCP_CONNECT_ERROR);
            currPool->freeConnection(conn);
            return lc::NSHEAD_TCP_CONNECT_ERROR;
        }

        
        add_monitor_info_ipport("QUERY_CONNECT_NUMBER", currFD, servicename);
        int fetch_time = (fetch_end.tv_sec-fetch_start.tv_sec)*1000 +
                (fetch_end.tv_usec-fetch_start.tv_usec)/1000;
        add_monitor_info_ipport_ex("CONNECT_TIME", currFD, servicename, fetch_time);
        
        int handleError = 0;
        socklen_t len = 0;
        len = sizeof(handleError);

        int val = getsockopt(currFD, SOL_SOCKET, SO_ERROR, &handleError, &len);
        if (-1 == val) {
            LC_LOG_WARNING("nshead_talk : get sockFD [%d] error [%d]", handleError, handleError);
            conn->setErrno(lc::NSHEAD_TCP_CONNECT_ERROR);
            currPool->freeConnection(conn);
            return lc::NSHEAD_TCP_CONNECT_ERROR;
        };

        int serverID = conn->serverID();
        LcClientServer *ser = fetchServer(servicename, serverID);
        if (NULL == ser) {
            LC_LOG_WARNING("nshead_talk fetch server error");
            conn->setErrno(lc::NSHEAD_LC_FETCH_SERVER_ERROR);
            currPool->freeConnection(conn);
            return lc::NSHEAD_LC_FETCH_SERVER_ERROR;
        }
        int readtimeout = ser->getReadTimeout();
        int writetimeout = ser->getWriteTimeout();
    
        

        int ret = nshead_write_ex(currFD, reqhead, const_cast<void *>(reqbuff), writetimeout);
        if (ret != 0) {
            
            add_monitor_info_ipport("WRITE_ERROR_NUMBER", currFD, servicename);
            
            LC_LOG_WARNING("%s nshead_write_ex error %d", __func__, ret);
            conn->setErrno(lc::NSHEAD_WRITE_HEAD_ERROR);
            currPool->freeConnection(conn);
            return lc::NSHEAD_WRITE_HEAD_ERROR;
        }
        
        ret = nshead_read_ex(currFD, reshead, resbuff, maxreslen, readtimeout);
        if (ret != 0) {
            
            add_monitor_info_ipport("READ_ERROR_NUMBER", currFD, servicename);
            
            LC_LOG_WARNING("%s nshead_read_ex error %d", __func__, ret);
            if (ret == NSHEAD_RET_EBODYLEN) {
                conn->setErrno(lc::NSHEAD_READ_BODY_BUF_NO_ENOUGH);
                currPool->freeConnection(conn);
                return lc::NSHEAD_READ_BODY_BUF_NO_ENOUGH;
            } else {
                conn->setErrno(lc::NSHEAD_READ_HEAD_ERROR);
                currPool->freeConnection(conn);
                return lc::NSHEAD_READ_HEAD_ERROR;
            }
        }
        
        currPool->freeConnection(conn);
        return 0;
    }

    int LcClientManager :: nshead_singletalk(const char *servicename, nshead_talkwith_t *talkmsg, 
                                            bsl::var::IVar &confVar)
    {
        LC_LOG_DEBUG("nshead_signletalk");
        if (NULL == servicename || NULL == talkmsg) {
            LC_LOG_WARNING("input NULL");
            return -1;
        }

        talkmsg->servicename = servicename;
        
        _service_rwlock.read();
        LcClientPool *currPool = getManagerPool(servicename);
        if (NULL == currPool) {
            _service_rwlock.unlock();
            return -1;
        }
        
        int ret = postSingleTalkBlock(talkmsg, confVar, currPool, servicename);
        if (ret != 0) {
            _service_rwlock.unlock();
            return talkmsg->success;
        }
        _service_rwlock.unlock();

        return 0;
    }

    int LcClientManager :: lc_client_common_callback(void *talk)
    {
        common_talkwith_t *curr_talk = (common_talkwith_t*)talk;
        LcEvent *lcevent = dynamic_cast<LcEvent*>(curr_talk->event);
        if (NULL == lcevent) {
            LC_LOG_WARNING("event type is not LcEvent");
            return -1;
        }
        lcevent->callback();
        curr_talk->eventpool->callEventMonitorCallBack(lcevent);
        return 0;
    }

    int LcClientManager :: lc_client_common_callback_ex(void *talk)
    {
        common_talkwith_t *curr_talk = (common_talkwith_t*)talk;
        LcEvent *lcevent = dynamic_cast<LcEvent*>(curr_talk->event);
        if (NULL == lcevent) {
            LC_LOG_WARNING("event type is not LcEvent");
            return -1;
        }
        lcevent->callback();
        return 0;
    }

    void LcClientManager ::  lc_client_callback(lc::IEvent *e, void *param)
    {
        if (NULL == e || NULL == param) {
            LC_LOG_WARNING("callback param error");
            return;
        }

        lc::nshead_atalk_t *atalk = (lc::nshead_atalk_t *)param;
        lcclient_nshead_atalk_t *lcclient_talk = (lcclient_nshead_atalk_t *)(atalk->param);
        LcClientPool *currPool = NULL;       
        
        
        if (0 != lcclient_talk->slctalk.err) {
            LC_LOG_DEBUG("talk error [%d] log_id[%u] Server : %s:%d", lcclient_talk->slctalk.err,
                         lcclient_talk->talk->reqhead.log_id,
                         lcclient_talk->server->getIP(lcclient_talk->conn->getCurrIP()),
                         lcclient_talk->server->getPort());
            lcclient_talk->talk->success = lcclient_talk->slctalk.err;
            if (lcclient_talk->talk->success != NSHEAD_READ_BODY_BUF_NO_ENOUGH) {
                if (NULL != lcclient_talk->conn) {
                    LC_LOG_DEBUG("conn error log_id[%u]", lcclient_talk->talk->reqhead.log_id);
                    
                    lcclient_talk->conn->setErrno(lcclient_talk->talk->success);
                }
            }

            int fd = lcclient_talk->conn->getIntFD();
            if (fd >= 0) {
                
                if (lcclient_talk->talk->success == NSHEAD_READ_HEAD_ERROR ||
                        lcclient_talk->talk->success == NSHEAD_READ_BODY_ERROR ||
                        lcclient_talk->talk->success == NSHEAD_READ_BODY_BUF_NO_ENOUGH ||
                        lcclient_talk->talk->success == NSHEAD_POST_READ_ERROR || 
                        lcclient_talk->talk->success == NSHEAD_POST_READBODY_ERROR) {

                    lcclient_talk->mgr->add_monitor_info_ipport("READ_ERROR_NUMBER",
                            fd, lcclient_talk->talk->servicename.c_str());
                }
                
                if (lcclient_talk->talk->success == NSHEAD_WRITE_HEAD_ERROR ||
                        lcclient_talk->talk->success == NSHEAD_WRITE_BODY_ERROR ||
                        lcclient_talk->talk->success == NSHEAD_POST_WRITEBODY_ERROR) {

                    lcclient_talk->mgr->add_monitor_info_ipport("WRITE_ERROR_NUMBER",
                            fd, lcclient_talk->talk->servicename.c_str());
                }
            }

        } else {
            LC_LOG_DEBUG("talk success log_id[%u]", lcclient_talk->talk->reqhead.log_id);
            lcclient_talk->talk->success = 0;
            if (NULL != atalk->alloccallback) {
                lcclient_talk->talk->resbuf = atalk->resbuf;
            }
        }

        
        if (NULL == lcclient_talk->conn) {
            LC_LOG_WARNING("conn is NULL log_id[%u]", lcclient_talk->talk->reqhead.log_id);
        } else {
            currPool = lcclient_talk->pool;
            if (NULL != currPool) {
                int handleError = 0;
                socklen_t len = 0;
                len = sizeof(handleError);
                int *curr_handle = (int *)lcclient_talk->conn->getHandle();
                int curr_fd = *curr_handle;
                int val = getsockopt(curr_fd, SOL_SOCKET, SO_ERROR, &handleError, &len);
                if (-1 == val) {
                    LC_LOG_DEBUG("nshead_talk : get sockFD [%d] error [%d] log_id[%u]", 
                                handleError, handleError, lcclient_talk->talk->reqhead.log_id);
                    
                    lcclient_talk->conn->setErrno(lc::LC_CONN_FD_ERROR);
                };
                
                
                if (lcclient_talk->talk->rpccallback1 != NULL && lcclient_talk->conn->getErrno() == 0) {
                    int ret = lcclient_talk->talk->rpccallback1((void *)lcclient_talk->talk);
                    if (ret == 1) {
                        
                        lcclient_talk->conn->setErrno(lc::LC_RPC_CLOSE_CONN);
                    }
                }
                currPool->freeConnection(lcclient_talk->conn);
                lcclient_talk->conn = NULL;
            }
        }
        
        
        server_talk_returninfo_t currReturnInfo;
        currReturnInfo.realreadtime = lcclient_talk->slctalk.realReadTime;
        currReturnInfo.realwritetime = lcclient_talk->slctalk.realWriteTime;
        
        lcclient_talk->talk->returninfo = currReturnInfo;
        lcclient_talk->talk->resbuf = lcclient_talk->slctalk.resbuf;

        LcClientServer *server = lcclient_talk->server;
        LcClientManager *mgr = lcclient_talk->mgr;
        if (NULL == server || NULL == mgr) {
            LC_LOG_WARNING("something wrong of talk for Server or Manager log_id[%u]",
                            lcclient_talk->talk->reqhead.log_id);
            goto End;
        }

        if (NULL != currPool) {
            LC_LOG_DEBUG("set server arg log_id[%u]", lcclient_talk->talk->reqhead.log_id);
            currPool->setServerArg(server, lcclient_talk->talk);
        }
    End:
        LcClientEventPool *eventpool = lcclient_talk->eventpool;
        if (NULL == eventpool) {
            LC_LOG_WARNING("event's pool is NULL log_id[%u]", lcclient_talk->talk->reqhead.log_id);
            return;
        }
        lc::SockEvent *sockEvent = dynamic_cast<lc::SockEvent *>(e);
        eventpool->callEventMonitorCallBack(sockEvent);

        return;
    }

    int LcClientManager :: nshead_nonblock_talk(const char *servicename, 
                                                nshead_talkwith_t *talkmsg[], 
                                                int count, bsl::var::IVar & confVar, 
                                                LcClientEventPool *pool)
    {
        LC_LOG_DEBUG("nshead_nonblock_talk");
        if (NULL == servicename || NULL == talkmsg || count <= 0) {
            return -1;
        }

        _service_rwlock.read();
        LcClientPool *currPool = getManagerPool(servicename);
        if (NULL == currPool) {
            _service_rwlock.unlock();
            return -1;
        }

        LcClientEventPool *currEventPool = getLocalEventPool(pool);
        if (NULL == currEventPool) {
             _service_rwlock.unlock();
             return -1;
        }

        int i = 0;
        int ret = 0;
        
        for (i=0; i<count; i++) {
            talkmsg[i]->servicename = servicename;
            ret = postSingleTalk(talkmsg[i], confVar, currPool, currEventPool, servicename);
            if (0 != ret){
                LC_LOG_DEBUG("post talk [%d] error", i);
            }
        }

        _service_rwlock.unlock();  
        
        return 0;
    }

    int LcClientManager :: nshead_nonblock_singletalk(const char *servicename, 
                                                      nshead_talkwith_t *talkmsg, 
                                                      bsl::var::IVar & confVar,
                                                      LcClientEventPool *pool)
    {
        LC_LOG_DEBUG("nshead_nonblock_signletalk");
        if (NULL == servicename || NULL == talkmsg) {
            return -1;
        }

        talkmsg->servicename = servicename;
        
        _service_rwlock.read();
        LcClientPool *currPool = getManagerPool(servicename);
        if (NULL == currPool) {
            _service_rwlock.unlock();
            return -1;
        }

        LcClientEventPool *currEventPool = getLocalEventPool(pool);
        if (NULL == currEventPool) {
             _service_rwlock.unlock();
             return -1;
        }
        postSingleTalk(talkmsg, confVar, currPool, currEventPool, servicename);
        _service_rwlock.unlock();

        if (isNsheadTalkInProcessing(talkmsg, currEventPool) == 0) {
            return talkmsg->success;
        } 

        return 0;
    }

    
    
    
    int LcClientManager :: wait(nshead_talkwith_t **return_talkmsg_vec, int num, int ms_timeout,
                                LcClientEventPool *pool)
    {
        LC_LOG_DEBUG("wait(nshead_talkwith_t **)");
        if ((NULL == return_talkmsg_vec) || (num <= 0)) {
            LC_LOG_WARNING("input error");
            return -1;
        }

        LcClientEventPool *currPool = getLocalEventPool1(pool);
        if (NULL == currPool) {
            return -1;
        }

        if (ms_timeout < 0 && -1 != ms_timeout) {
            LC_LOG_WARNING("input ms_timeout[%d] < 0", ms_timeout);
            ms_timeout = 0;
        }
        int readyNum = currPool->poll_tv(ms_timeout);
        if (readyNum <= 0) {
            return readyNum;
        }
        if (readyNum > num) {
            readyNum = num;
        }
        int i = 0;
        for (i=0; i<readyNum; i++) {
            return_talkmsg_vec[i] = currPool->fetchFirstReadyTalk();
            if (NULL == return_talkmsg_vec[i]) {
                return i-1;
            }
        }

        return i;
    }

    
    int LcClientManager :: wait(nshead_talkwith_t *req_talkmsg, int ms_timeout,
                                LcClientEventPool *pool)
    {
        LC_LOG_DEBUG("wait(nshead_talkwith_t*)");
        if (NULL == req_talkmsg) {
            return -1;
        }

        LcClientEventPool *currPool = getLocalEventPool1(pool);
        if (NULL == currPool) {
                return -1;
        }
        if (ms_timeout < 0 && -1 != ms_timeout) {
            LC_LOG_WARNING("input ms_timeout[%d] < 0", ms_timeout);
            ms_timeout = 0;
        }
        return currPool->poll_single(req_talkmsg, ms_timeout);
    }

    
    int LcClientManager :: wait(int ms_timeout, LcClientEventPool *pool)
    {
        LC_LOG_DEBUG("wait(ms_timeout)");

        LcClientEventPool *currPool = getLocalEventPool1(pool);
        if (NULL == currPool) {
            return -1;
        }

        if (ms_timeout < 0 && -1 != ms_timeout) {
            LC_LOG_WARNING("input ms_timeout[%d] < 0", ms_timeout);
            ms_timeout = 0;
        }
        return currPool->poll_tv(ms_timeout); 
    }

    
    int LcClientManager :: waitAll(LcClientEventPool *pool) 
    {
        LC_LOG_DEBUG("waitAll(LcClientEventPool*)");
        LcClientEventPool *currPool = getLocalEventPool1(pool);
        if (NULL == currPool) {
                return -1;
        }

        return currPool->pollAll();
    }

    nshead_talkwith_t *LcClientManager :: fetchFirstReadyReq(LcClientEventPool *pool)
    {
        LC_LOG_DEBUG("fetchFirstReadyReq()");
        LcClientEventPool *currPool = getLocalEventPool1(pool);
        if (NULL == currPool) {
            return NULL;
        }

        return currPool->fetchFirstReadyTalk();
    }

    int LcClientManager :: cancelAll(LcClientEventPool *pool)
    {
        LC_LOG_DEBUG("cancelAll()");

        LcClientEventPool *currPool = getLocalEventPool1(pool);
        if (NULL == currPool) {
            return -1;
        }
        currPool->cancelAll();

        return 0;
    }

    LcClientConnection * LcClientManager :: fetchConnection(const char *servicename, int *errNo,
                                                            default_server_arg_t *serverArg,
                                                            bsl::var::IVar & info)
    {
        LC_LOG_DEBUG("%s", __func__);

        if (NULL == errNo) {
            return NULL;
        }

        if (NULL == servicename) {
            *errNo = -1;
            return NULL;
        }
        default_server_arg_t defaultServerArg;
        defaultServerArg.key = -1;
        if (NULL == serverArg) {
            serverArg = &defaultServerArg;
        }
        _service_rwlock.read();
        LcClientPool *currPool = getManagerPool(servicename);
        if (NULL == currPool) {
            _service_rwlock.unlock();
            *errNo = -1;
            return NULL;
        }

        LcClientConnection *conn = currPool->fetchConnection(*serverArg, info, errNo, this);
        if (NULL == conn) {
            _service_rwlock.unlock();
            LC_LOG_WARNING("fetchConnection error");
            return NULL;
        }
        if (-1 == _conn_map.set(reinterpret_cast<long>(conn), servicename)){
            _service_rwlock.unlock();
            LC_LOG_WARNING("set connection error");
            *errNo = -1;
            return NULL;
        }
        _service_rwlock.unlock();
        
        *errNo = 0;
        conn->setTID(pthread_self());
        return conn;
    }

    int LcClientManager :: freeConnection(LcClientConnection* conn)
    {
        LC_LOG_DEBUG("%s", __func__);

        if (NULL == conn) {
            return -1;
        }
        
        _service_rwlock.read();
        bsl::string serviceName;
        if (_conn_map.get(reinterpret_cast<long>(conn), &serviceName)!= bsl::HASH_EXIST) {
            LC_LOG_WARNING("freeConnection error");
            _service_rwlock.unlock();
            return -1;
        }
        LcClientPool *pool = getManagerAllPool(serviceName.c_str());
        if (NULL == pool) {
            LC_LOG_WARNING("serviceName [%s]'s pool is NULL", serviceName.c_str());
            _service_rwlock.unlock();
            return -1;
        }
        int ret = pool->freeConnection(conn);
        _conn_map.set(reinterpret_cast<long>(conn), "");
        _service_rwlock.unlock();

        return ret;
    }

    int LcClientManager :: startHealthyCheck(u_int msSleepTime)
    {
        LC_LOG_DEBUG("%s", __func__);

        _healthyCheckTime = msSleepTime;

        if (0 == _healthy_run) {
            _healthy_run = 1;
            int ret = pthread_create(&_healthy_check_t, NULL, start_healthy_check, this);
            if (0 != ret) {
                _healthy_run = 0;
                return ret;
            } else {
                return 0;
            }
        }

        return 0;
    }

    void *LcClientManager :: start_healthy_check(void *param)
    {
        if (NULL == param) {
            return NULL;
        }
        LcClientManager *mgr = (LcClientManager *)param;

        mgr->healthyCheck();
        return NULL;
    }

    int LcClientManager :: healthyCheck()
    {
        lc_log_initthread("healthyChecker");
        LC_LOG_DEBUG("%s", __func__);

        while ((_healthy_run)) {
            _service_rwlock.read();
            
            int vec_size = (int)_service_map.size();
            for (int i=0; i<vec_size; i++) {
                LcClientPool *pool = _service_map[i]->pool;
                if (pool->isEnable()) {
                    pool->healthyCheck();
                }
            }
            _service_rwlock.unlock();

            comcm::ms_sleep(_healthyCheckTime);
        }
        lc_log_closethread();
        return 0;
    }


int LcClientManager :: reload(const comcfg::ConfigUnit & conf)
{
    LC_LOG_DEBUG("%s", __func__);

    
    
    

    
    if (conf[LCCLIENT_CONF_SERVICE].selfType() == comcfg::CONFIG_ERROR_TYPE) {
        LC_LOG_WARNING("[%s] is not exist in Conf", LCCLIENT_CONF_SERVICE);
        return -1;
    }
    int serviceNum = conf[LCCLIENT_CONF_SERVICE].size();
    int i = 0;
    typedef bsl::hashmap<bsl::string, int> str_int_hashmap;
    str_int_hashmap currMap;
    int ret = currMap.create(LCCLIENT_HASHMAP_SIZE);
    if (0 != ret) {
        LC_LOG_WARNING("create hashmap error [%d][%m], reload over", ret);
        return -1;
    }
    bsl::string serviceName;
    while (i < serviceNum) {
        if (conf[LCCLIENT_CONF_SERVICE][i][LCCLIENT_CONF_SERVICE_NAME].get_bsl_string(
                    &serviceName) != 0) {

            LC_LOG_WARNING("get servicename[%d] error", i);
            i ++;
            continue;
        }
        currMap.set(serviceName, i);
        i ++;
    }

    _service_rwlock.read();
    
    
    str_int_hashmap :: iterator it2;
    int vec_size = (int)_service_map.size();
    for (int i=0; i<vec_size; i++) {
        for (it2=currMap.begin(); it2!=currMap.end(); it2++) {
            if (strcmp( _service_map[i]->serviceName, (*it2).first.c_str()) != 0) {
                continue;
            } else {
                LcClientPool *tempPool = _service_map[i]->pool;
                if (_service_map[i]->pos == 1) {
                    tempPool->setReloadTag(2);;
                } else {
                    tempPool->setReloadTag(1);;
                }
            }    
        }
    }

    
    ret = 0;
    vec_size = (int)_service_map.size();
    for (int i=0; i<(int)_service_map.size(); i++) {
        LcClientPool *pool = _service_map[i]->pool;
        if (pool->getReloadTag() == 0) {
            if (_service_map[i]->pos == 1) {
                continue;
            } else {
                pool->setEnable(0);
            }
            continue;
        } else if (pool->getReloadTag() == 1){
            int currNum;
            if (currMap.get(_service_map[i]->serviceName, &currNum) != bsl::HASH_EXIST) {
                continue;
            }
            ret = pool->reload(conf[LCCLIENT_CONF_SERVICE][currNum]);
            if (0 != ret) {
                LC_LOG_WARNING("reload Service[%d] error, set unenable", currNum);
                pool->setEnable(0);
            }
            currMap.erase(_service_map[i]->serviceName);
            pool->setEnable(1);
        } else {
            LC_LOG_WARNING("local resource[%s] add galileo's resource",
                           _service_map[i]->serviceName);
            currMap.erase(_service_map[i]->serviceName);
        }
    }
    _service_rwlock.unlock();

    
    
    
    _service_rwlock.write();
    for (it2=currMap.begin(); it2!=currMap.end(); it2++) {
        int currNum = (*it2).second;
        LcClientPool *pool = new (std::nothrow)LcClientPool;
        if (NULL == pool) {
            _service_rwlock.unlock();
            currMap.destroy();
            LC_LOG_WARNING("new pool[%d] [%s] error, end reload",
                            currNum, (*it2).first.c_str());
            return -1;
        }
        ret = pool->init(conf[LCCLIENT_CONF_SERVICE][currNum]);
        if (0 != ret) {
            LC_LOG_WARNING("init new pool[%d] [%s] error", currNum, (*it2).first.c_str());
            delete pool;
            pool = NULL;
            continue;
        }
        if (addService((*it2).first.c_str(), pool, "", "", 0) != 0) {
            delete pool;
            pool = NULL;
            continue;
        }
    }

    for (int i=0; i<(int)_service_map.size(); i++) {
        LcClientPool *pool = _service_map[i]->pool;
        pool->setReloadTag(0);
    }

    _service_rwlock.unlock();
    currMap.destroy();
    
    return 0;
}

    int LcClientManager :: getServiceCount()
    {
        LC_LOG_DEBUG("%s", __func__);

        _service_rwlock.read();
        int num = _service_map.size();
        _service_rwlock.unlock();

        return num;
    }

    LcClientPool * LcClientManager :: getServiceByID(int id) 
    {
        LC_LOG_DEBUG("%s", __func__);

        _service_rwlock.read();
        int vec_size = (int)_service_map.size();
        if ((id < 0) || (id >= vec_size)) {
            _service_rwlock.unlock();
            return NULL;
        }

        LcClientPool *currPool = NULL;
        
        int i = 0;
        
        for (int j=0; j<vec_size; j++) {
            if (i == id) {
                currPool = _service_map[j]->pool;
                break;
            }
            i ++;
        }

        _service_rwlock.unlock();
        return currPool;
    }

    LcClientServer *LcClientManager :: fetchServer(const char *servicename, int serverID)
    {
        LC_LOG_DEBUG("%s", __func__);

        if (NULL == servicename) {
            return NULL;
        }
        
        LcClientPool *currPool = getManagerPool(servicename);
        if (NULL == currPool) {
            return NULL;
        }

        LcClientServer *server = currPool->getServerByID(serverID);

        return server;
    }

    bsl::var::IVar & LcClientManager :: getConf()
    {
        LC_LOG_DEBUG("%s", __func__);

        return _main_conf;
    }

    int LcClientManager :: getReqAndResBufLen(const char *servicename, int &reqBufLen,
                                            int &resBufLen)
    {
        LC_LOG_DEBUG("%s", __func__);

        if (NULL == servicename) {
            return -1;
        }

        _service_rwlock.read();
        LcClientPool *currPool = getManagerPool(servicename);
        if (NULL == currPool) {
            _service_rwlock.unlock();
            return -1;
        }
        _service_rwlock.unlock();

        return currPool->getReqAndResBufLen(reqBufLen, resBufLen);
    }

    int LcClientManager :: getReqBufLen(const char *servicename)
    {
        if (NULL == servicename) {
            return -1;
        }
        _service_rwlock.read();
        LcClientPool *currPool = getManagerPool(servicename);
        if (NULL == currPool) {
            _service_rwlock.unlock();
            return -1;
        }
        _service_rwlock.unlock();

        return currPool->getReqBufLen();
    }

    int LcClientManager :: getResBufLen(const char *servicename)
    {
        if (NULL == servicename) {
            return -1;
        }
        _service_rwlock.read();
        LcClientPool *currPool = getManagerPool(servicename);
        if (NULL == currPool) {
            _service_rwlock.unlock();
            return -1;
        }
        _service_rwlock.unlock();

        return currPool->getResBufLen();
    }

int LcClientManager :: dump_galileo_conf(const char *txt)
{
    LC_LOG_DEBUG("%s", __func__);

    if (NULL == txt){
        LC_LOG_WARNING("Failed to get a valid dir, name or txt");
        return -1;
    }

    char path[LCCLIENT_PATH_LEN];
    int len = snprintf(path, sizeof(path), "%s", _galileoDir);
    if (len >= 1024) {
        return -1;
    }
    if ('/' != path[len-1]) {
        path[len] = '/';
        len ++;
    }
    if (len >= 1024) {
        return -1;
    }

    int len1 = snprintf(path+len, 1024-len, "%s", _galileoFile);
    if (len1 >= (1024-len)) {
        return -1;
    }

    FILE *file = fopen(path, "w");
    if (NULL == file){
        LC_LOG_WARNING("Failed to open dump file [%s]", path);
        return -1;
    }
    fprintf(file, "%s", txt);
    fclose(file);

    return 0;
}

    int LcClientManager :: readLock(const char *servicename)
    {
        LC_LOG_DEBUG("%s", __func__);

        if (NULL == servicename) {
            return -1;
        }

        _service_rwlock.read();
        LcClientPool *currPool = getManagerPool(servicename);
        if (NULL == currPool) {
            _service_rwlock.unlock();
            return -1;
        }
        
        currPool->readLock();

        return 0;
    }

    int LcClientManager :: unlock(const char *servicename)
    {
        LC_LOG_DEBUG("%s", __func__);

        if (NULL == servicename) {
            return -1;
        }
        
        LcClientPool *currPool = getManagerAllPool(servicename);
        if (NULL == currPool) {
            return -1;
        }
        
        currPool->unlock();
        _service_rwlock.unlock();
        return 0;
    }

    bsl::var::IVar & LcClientManager :: getServiceConf(const char *servicename, 
                                                       bsl::ResourcePool *rp, int *errNo)
    {
        LC_LOG_DEBUG("%s", __func__);
        
        static bsl::var::Null emptyVal;
        if (NULL == errNo) {
            return emptyVal;
        }
        
        if ((NULL == servicename) || (NULL == rp)) {
            *errNo = -1;
            return emptyVal;
        }

        _service_rwlock.read();
        LcClientPool *currPool = getManagerPool(servicename);
        if (NULL == currPool) {
            _service_rwlock.unlock();
            *errNo = -1;
            return emptyVal;
        }
        _service_rwlock.unlock();

        return currPool->getConf(rp, errNo);
    }

    int LcClientManager :: close()
    {
        _depres_lock.lock();
        _galileoRun = 0;
        _depres_lock.unlock();
        if (NULL != _netreactor && 0 == _isOut) {
            _netreactor->stopUntilEmpty();
            _netreactor->join();
        }

        return 0;
    }

    LcClientEventPool * LcClientManager :: getLocalEventPool(LcClientEventPool *pool)
    {
        LcClientEventPool *currEventPool = NULL;
		if (NULL == pool) {
			currEventPool = (LcClientEventPool *)pthread_getspecific(_poolkey);
			if (NULL == currEventPool) {
				LC_LOG_DEBUG("create thread pool");
                currEventPool = new (std::nothrow)LcClientEventPool;
				if (NULL == currEventPool) {
					LC_LOG_WARNING("new LcClientEventPool error");
					return NULL;
				}
				if (currEventPool->init() != 0) {
					delete currEventPool;
					currEventPool = NULL;
					LC_LOG_WARNING("event pool init error");
					return NULL;
				}
				pthread_setspecific(_poolkey, (void *)currEventPool);
			}
		} else {
			currEventPool = pool;
		}

        return currEventPool;
    }
    
    LcClientEventPool * LcClientManager :: getLocalEventPool1(LcClientEventPool *pool)
    {
        LcClientEventPool *currPool = NULL;
        if (NULL == pool) {
            currPool = (LcClientEventPool *)pthread_getspecific(_poolkey);
            if (NULL == currPool) {
                LC_LOG_WARNING("get eventpool error");
                return NULL;
            }
        } else {
            currPool = pool;
        }
        return currPool;
    }

    int LcClientManager :: postSingleTalk(nshead_talkwith_t* talkmsg, bsl::var::IVar & confVar,
                                          LcClientPool *currPool, LcClientEventPool *currEventPool, 
                                          const char* servicename)
    {
        if (NULL == talkmsg || NULL == currPool || NULL == currEventPool || NULL == servicename) {
            return -1;
        }

        talkmsg->success = lc::NSHEAD_LC_IN_PROCESSING;
        talkmsg->innerTalk.slctalk.err = 0;

        if (NULL == talkmsg) {
            LC_LOG_WARNING("nshead_talk : is NULL");
            talkmsg->success = lc::NSHEAD_LC_INIT_ERROR;
            return -1;
        }

        struct timeval fetch_start, fetch_end;
        gettimeofday(&fetch_start, NULL);

        int errNo = 0;
        LcClientConnection *conn = currPool->fetchConnection(talkmsg->defaultserverarg,
                                                             confVar, &errNo, this);
        gettimeofday(&fetch_end, NULL);
        if (NULL == conn) {
            LC_LOG_WARNING("nshead_talk : fetchConnection error");
            if (-1 != errNo) {
                talkmsg->success = errNo;
            } else {
                talkmsg->success = lc::NSHEAD_LC_INIT_ERROR;
            }
            return -1;
        }

        conn->setTID(pthread_self());
        int *strFD = (int*)(conn->getHandle());
        if (NULL == strFD) {
            talkmsg->success = lc::NSHEAD_TCP_CONNECT_ERROR;
            LC_LOG_WARNING("nshead_talk: socket fd error");
            conn->setErrno(lc::NSHEAD_TCP_CONNECT_ERROR);
            currPool->freeConnection(conn);
            return -1;
        }
        int currFD = *strFD;
        if (currFD < 0) {
            talkmsg->success = lc::NSHEAD_TCP_CONNECT_ERROR;
            LC_LOG_WARNING("nshead_talk: socket fd error");
            conn->setErrno(lc::NSHEAD_TCP_CONNECT_ERROR);
            currPool->freeConnection(conn);
            return -1;
        }

        add_monitor_info_ipport("QUERY_CONNECT_NUMBER", currFD, servicename);
        int fetch_time = (fetch_end.tv_sec-fetch_start.tv_sec)*1000 + 
                (fetch_end.tv_usec - fetch_start.tv_usec)/1000;
        add_monitor_info_ipport_ex("CONNECT_TIME", currFD, servicename, fetch_time);

        int handleError = 0;
        socklen_t len = 0;
        len = sizeof(handleError);

        int val = getsockopt(currFD, SOL_SOCKET, SO_ERROR, &handleError, &len);
        if (-1 == val) {
            talkmsg->success = lc::NSHEAD_TCP_CONNECT_ERROR;
            LC_LOG_WARNING("nshead_talk : get sockFD [%d] error [%d]", handleError, handleError);
            conn->setErrno(lc::NSHEAD_TCP_CONNECT_ERROR);
            currPool->freeConnection(conn);
            return -1;
        };

        lcclient_nshead_atalk_t *currTalk = &talkmsg->innerTalk;
        currTalk->mgr = this;
        currTalk->conn = conn;
        currTalk->eventpool = currEventPool;
        currTalk->pool = currPool;
        currTalk->talk = talkmsg;
        int serverID = conn->serverID();
        LcClientServer *ser = fetchServer(servicename, serverID);
        if (NULL == ser) {
            talkmsg->success = lc::NSHEAD_LC_FETCH_SERVER_ERROR;
            LC_LOG_WARNING("nshead_talk fetch server error");
            conn->setErrno(lc::NSHEAD_LC_FETCH_SERVER_ERROR);
            currPool->freeConnection(conn);
            return -1;
        }
        currTalk->server = ser;
        currTalk->readtimeout = ser->getReadTimeout();
        currTalk->writetimeout = ser->getWriteTimeout();
        set_lcnshead_atalk(currTalk, talkmsg, lc_client_callback, currTalk);

        lc::SockEvent *currEvent = dynamic_cast<lc::SockEvent *>(currEventPool->setTalk(talkmsg));
        if (NULL == currEvent) {
            talkmsg->success = lc::NSHEAD_LC_OTHER_ERROR;
            LC_LOG_WARNING("nshead_talk new event error");
            currPool->freeConnection(conn);
            return -1;
        }
        
        currEvent->setHandle(currFD);
        currEvent->setTimeout(currTalk->writetimeout);
        lc::nshead_atalkwith1(currEvent, &currTalk->slctalk);
        
        if (0 == _isOut) {
            if (lc::IReactor::STOP == _netreactor->status()) {
                if (_netreactor->run() <= 0) {
                    LC_LOG_WARNING("Reactor run error");
                    currEvent->setResult(1UL<<5UL);
                    currTalk->slctalk.err = NSHEAD_READ_BODY_BUF_NO_ENOUGH;
                    lc_client_callback(currEvent, &currTalk->slctalk);
                    talkmsg->success = lc::LC_NETREACTOR_RUN_ERROR;
                    return -1;
                }
            }
        }
        
        if(_netreactor->post(currEvent) != 0) {
            LC_LOG_WARNING("Reactor post error");
            currEvent->setResult(1UL<<5UL);
            currTalk->slctalk.err = NSHEAD_READ_BODY_BUF_NO_ENOUGH;
            lc_client_callback(currEvent, &currTalk->slctalk);
            talkmsg->success = lc::LC_NETREACTOR_POST_ERROR;
            return -1;
        }
        return 0;
    }


int LcClientManager :: saveGalileoCopy(bsl::var::IVar & galileo_conf_array)
{
    int arraySize = galileo_conf_array.size();
	if(arraySize == 0)
		return 0;

	
    bsl::var::Dict d;
    d.set(LCCLIENT_CONF_SERVICE, galileo_conf_array);
	
    bsl::var::Dict client_dict;
    client_dict.set(LCCLIENT_CONF_LCCLIENT, d);

	
    comcfg::Configure client_conf;
    if (client_conf.loadIVar(client_dict) != 0) {
		return -1;
    }

	
    size_t conflen = 0;
    char *confstr = client_conf.dump(&conflen);
    int ret = dump_galileo_conf(confstr);
    if (0 != ret) {
        LC_LOG_WARNING("Failed to write galileo conf to local copy.");
    }

	
    if (_galileoConfSaveLen <= int(conflen)) {
        if (NULL != _galileoConfSave){
            delete []_galileoConfSave;
            _galileoConfSave = NULL;
            _galileoConfSaveLen = 0;
        }
        _galileoConfSaveLen = conflen + 1;
        _galileoConfSave = new (std::nothrow) char[_galileoConfSaveLen];
        if (NULL == _galileoConfSave) {
            LC_LOG_WARNING("new buffer for galileoConf error");
            _galileoConfSaveLen = 0;
            return -1;
        }
    }

    _galileoConfSaveLen = conflen + 1;
    snprintf(_galileoConfSave, _galileoConfSaveLen, "%s", confstr);
    return 0;
}


    int LcClientManager :: isNsheadTalkInProcessing(nshead_talkwith_t *talk, LcClientEventPool *pool)
    {
        return isTalkInProcessing(talk, pool);
               
    }

    int LcClientManager :: isCommonTalkInProcessing(LcEvent *event, LcClientEventPool *pool)
    {
        if (NULL == event) {
            return -1;
        }
        common_talkwith_t *curr_talk = (common_talkwith_t *)(event->get_tag());
        return isTalkInProcessing(curr_talk, pool);
    }

    int LcClientManager :: isTalkInProcessing(lcclient_talkwith_t *talk, LcClientEventPool *pool)
    {
        if (NULL == talk) {
            return -1;
        }

        LcClientEventPool *currPool = getLocalEventPool1(pool);
        if (NULL == currPool) {
            return -1;
        }
        
        return currPool->isTalkIn(talk);
    }

    LcClientPool * LcClientManager :: getManagerPool(const char *servicename)
    {
        if (NULL == servicename) {
            return NULL;
        }

        LcClientPool *currPool = NULL;
        currPool = getManagerAllPool(servicename);
        if (NULL == currPool) {
            return NULL;
        }

        if (!currPool->isEnable()) {
            LC_LOG_WARNING("service [%s] not enable", servicename);
            return NULL;
        }

        return currPool;
    }

    LcClientPool *LcClientManager :: getManagerAllPool(const char *servicename)
    {
        if (NULL == servicename) {
            return NULL;
        }

        LcClientPool *currPool = NULL;
        
        if (getService(servicename, &currPool) != 0) {
            LC_LOG_WARNING("service [%s] not exist", servicename);
            return NULL;
        }
        if (NULL == currPool) {
            LC_LOG_WARNING("service [%s] is NULL", servicename);
            return NULL;
        }

        return currPool;
    }

int LcClientManager::_init_with_localconf(const comcfg::ConfigUnit & conf)
{
	RETURN_IF_KEY_ERROR(conf, LCCLIENT_CONF_LCCLIENT, -1);

    int ret = init(conf[LCCLIENT_CONF_LCCLIENT]);
    if (ret != 0) {
        LC_LOG_WARNING("init LcClientManager error");
        return -1;
    }
    return 0;
}

int LcClientManager::_init_with_localconf_ext(const comcfg::ConfigUnit & conf)
{
	RETURN_IF_KEY_ERROR(conf, LCCLIENT_CONF_LCCLIENT, -1);

    int ret = initAfterGalileo(conf[LCCLIENT_CONF_LCCLIENT]);
    if (ret != 0) {
        LC_LOG_WARNING("init LcClientManager error");
        return -1;
    }
    return 0;
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

static void _dump_var_info(bsl::var::IVar &var)
{
	bsl::var::JsonSerializer js;
	bsl::AutoBuffer buf;
	js.serialize(var, buf);
	LC_LOG_DEBUG("VAR content: (%s)", buf.c_str());
}





static int galileo_event_callback(int watch_type, const char *realpath, bsl::var::IVar& res_data, void *cb_ctx)
{
    

    if (NULL==realpath)   {
        return -1;
    }

    int ret = 0;
    bsl::ResourcePool rp;
    bsl::string addr;

	bsl::var::IVar &gailieo_service_dict = rp.create<bsl::var::Dict>();
	
	LcClientManager::cluster_wrap_t *wrap = (LcClientManager::cluster_wrap_t*)cb_ctx;
	LcClientManager *lccm = wrap->lccm;
	GalileoCluster  *cluster = wrap->cluster;
	
	LC_LOG_DEBUG("Enter galileo_event_callback, (%d) (%s)", watch_type, realpath);
	_dump_var_info(res_data);

    switch(watch_type){
        case GALILEO_ABSTRACT_CHANGED_EVENT:
            
			ret = cluster->query_resource(realpath, gailieo_service_dict, rp);
            if (0 != ret) {
                LC_LOG_WARNING("Query resource (%s) failed, %d", realpath, ret);
                break;
            }
			LC_LOG_DEBUG("Query resource (%s) success.", realpath);
			_dump_var_info(gailieo_service_dict);
			if(lccm->reload_by_galileo_event(gailieo_service_dict, realpath) != 0) {
				LC_LOG_WARNING("reload_by_galileo_event failed.");
			}
			LC_LOG_DEBUG("reload_by_galileo_event success.");
            break;
			
        case GALILEO_ENTITY_CHANGED_EVENT:
			
            if (parseRealPath(realpath, addr) != 0) {
                LC_LOG_WARNING("parseRealPath (%s) failed.", realpath);
                break;
            }
			
            ret = cluster->query_resource(addr.c_str(), gailieo_service_dict, rp);
            if (0 != ret) {
                LC_LOG_WARNING("Query resource (%s) failed, %d", addr.c_str(), ret);
                break;
            }
			LC_LOG_DEBUG("Query resource (%s) success.", addr.c_str());
			_dump_var_info(gailieo_service_dict);
			if(lccm->reload_by_galileo_event(gailieo_service_dict, addr.c_str()) != 0) {
				LC_LOG_WARNING("reload_by_galileo_event failed.");
			}
			LC_LOG_DEBUG("reload_by_galileo_event success.");
			
            break;
        case GALILEO_RESOURCE_UPDATE_EVENT:
			
			if(lccm->reload_by_galileo_event(res_data, realpath) != 0) {
				LC_LOG_WARNING("reload_by_galileo_event failed.");
			}	
			LC_LOG_DEBUG("reload_by_galileo_event success.");
            break;
        default:
            LC_LOG_WARNING("Get an invalid event [%d], path[%s]", watch_type, realpath);
            break;
    }

    return 0;
}

int LcClientManager::_process_request_resource(
	const comcfg::ConfigUnit& galileo_conf,
	const comcfg::ConfigUnit& local_conf, 
	int merge_flag, 
	bsl::ResourcePool &rp,
	bsl::var::IVar &galileo_conf_array)
{

	LC_LOG_DEBUG("Processing REQUEST_RESOURCE group.");

	RETURN_IF_KEY_ERROR(galileo_conf, LCCLIENT_CONF_GALILEO_REQSES, -1);
	const comcfg::ConfigUnit& reqres_conf = galileo_conf[LCCLIENT_CONF_GALILEO_REQSES];
	
	RETURN_IF_KEY_ERROR(reqres_conf, LCCLIENT_CONF_GALILEO_REQSES_RESADDR, -1);
	const comcfg::ConfigUnit& reqresaddr_conf = reqres_conf[LCCLIENT_CONF_GALILEO_REQSES_RESADDR];		

	int service_num = reqresaddr_conf.size();
	if(service_num == 0)
		return 0;
	
	int i;
	int ret;
	GalileoCluster *cluster = NULL;
	cluster_wrap_t *cluster_wrap = NULL;

	
	for (i = 0; i < service_num; i++) {
		
		bsl::string service_name;
		RETURN_IF_NO_VALUE(reqresaddr_conf[i], get_bsl_string,
			LCCLIENT_CONF_GALILEO_REQSES_RESADDR_NAME, service_name, -1);
		
		bsl::string proxy;
		bsl::string addr;
		bsl::string proxy_name;
		bsl::string simple_name;
		
		if (reqresaddr_conf[i][LCCLIENT_CONF_GALILEO_REQSES_RESADDR_PROXY].get_bsl_string(&proxy) == 0) {
			
			
			const char *c_proxy = proxy.c_str();
			if ('/' != c_proxy[0]) {
				LC_LOG_WARNING("Invalid proxy (%s)", c_proxy);
				break;
			}
			int ret = strrchr(c_proxy, '/') - c_proxy;
			if (0 == ret || 1 == ret || ret == ((int)proxy.size()-1)) {
				LC_LOG_WARNING("Invalid proxy (%s)", c_proxy);
				break;
			}
			char c_addr[ret+1];
			strncpy(c_addr, c_proxy, ret);
			c_addr[ret] = '\0';
			addr = c_addr;
			int proxy_len = proxy.size();
			char c_name[proxy_len-ret+1];
			strncpy(c_name, c_proxy+ret+1, proxy_len-ret);
			c_name[proxy_len-ret] = '\0';
			proxy_name = c_name;
			  
		} else if (reqresaddr_conf[i][LCCLIENT_CONF_GALILEO_REQSES_RESADDR_ADDR].get_bsl_string(&addr) != 0) {
			
			
			const char *c_name = service_name.c_str();
			if ('/' != c_name[0]) {
				LC_LOG_WARNING("Invalid service name (%s).", c_name);
				break;
			}
			int ret = strrchr(c_name, '/') - c_name;
			if (0 == ret || 1 == ret || ret == ((int)service_name.size()-1)) {
				LC_LOG_WARNING("Invalid service name (%s).", c_name);
				break;
			}
			char c_addr[ret+1];
			strncpy(c_addr, c_name, ret);
			c_addr[ret] = '\0';
			addr = c_addr;
			int ret1 = service_name.size() - ret;
			char s_name[ret1+1];
			strncpy(s_name, c_name+ret+1, ret1);
			s_name[ret1] = '\0';
			simple_name = s_name;
		} else {
			simple_name = service_name;
		}
	
	
		int host_index = 0;
		if(m_new_version) {
			RETURN_IF_NO_VALUE(reqresaddr_conf[i], get_int32, "HOST_INDEX", host_index, -1);
		} 
	
		if(host_index < 0 || host_index >= (int)m_cluster_vector.size()) {
			LC_LOG_WARNING("Invalid HOST_INDEX (%d)", host_index);
			break;
		}
		cluster_wrap = m_cluster_vector[host_index];
		cluster = cluster_wrap->cluster;
	
		
		bsl::var::IVar &galileo_service_dict = rp.create<bsl::var::Dict>();
		ret = cluster->query_resource(addr.c_str(), galileo_service_dict, rp);
		if (ret != 0) {
			LC_LOG_WARNING("Query resource (%s) (%s) failed.", addr.c_str(), service_name.c_str());
			break;
		}
		LC_LOG_DEBUG("Query resource (%s) (%s) success.", addr.c_str(), service_name.c_str());
	
		path_sev_pair tempPair(addr.c_str(), service_name.c_str());
		_galileo_servmap.insert(tempPair);
	
		
		ret = init(galileo_service_dict, service_name.c_str(), rp, galileo_conf_array, local_conf, merge_flag,
					proxy_name.c_str(), simple_name.c_str());
		if (ret != 0) {
			LC_LOG_WARNING("Init clientpool of (%s) (%s) failed.", addr.c_str(), service_name.c_str());
			break;
		}

		LC_LOG_WARNING("Init clientpool of (%s) (%s) success.", addr.c_str(), service_name.c_str());
	
		
		ret = cluster->watch_resource(addr.c_str(), galileo_event_callback, cluster_wrap);
		if (ret != 0) {
			LC_LOG_WARNING("Watch resource (%s) failed.", addr.c_str());
			break;
		}
		LC_LOG_DEBUG("Watch resource (%s) (%s) success.", addr.c_str(), service_name.c_str());
	}
	
	
	if (i < service_num) {
		LC_LOG_WARNING("Process REQUEST_RESOURCE group failed.");
		return -1;
	}
	LC_LOG_DEBUG("Process REQUEST_RESOURCE group success.");
	return 0;

}

static int _register_resource(GalileoCluster *cluster, const char *res_addr, const char *res_data, int *ptagno)
{
	bsl::ResourcePool rp;
	bsl::var::JsonDeserializer jd(rp);
	bsl::var::Dict json_var;
	int ret;

	try{
		json_var = jd.deserialize(res_data);

		if (!json_var.is_dict()){
			LC_LOG_WARNING("[%s:%d] Failed to get a valid dict", __FILE__, __LINE__);
			return -1;
		}

		
		if (json_var.get(GALILEO_CONF_GALILEO_IP).is_null()){
			char* tmpip = new char[LCCLIENT_IP_LEN];
			ret = get_local_conn_ip(tmpip, LCCLIENT_IP_LEN);
			if (0 == ret){
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
			ret = gethostname(tmphostname, LCCLIENT_HOSTNAME_STR_LEN);
			if (0 == ret){
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

	return cluster->register_resource(res_addr, json_var, ptagno);

}


int LcClientManager::_process_register_resource(const comcfg::ConfigUnit& galileo_conf)
{
	LC_LOG_DEBUG("Processing REGISTER_RESOURCE group.");

	RETURN_IF_KEY_ERROR(galileo_conf, LCCLIENT_CONF_GALILEO_REGSES, 0);
	const comcfg::ConfigUnit& regres_conf = galileo_conf[LCCLIENT_CONF_GALILEO_REGSES];
	
	RETURN_IF_KEY_ERROR(regres_conf, LCCLIENT_CONF_GALILEO_REGSES_RESADDR, 0);	
	const comcfg::ConfigUnit& regresaddr_conf = regres_conf[LCCLIENT_CONF_GALILEO_REGSES_RESADDR];
		
	int service_num = regresaddr_conf.size();
	if(service_num == 0)
		return 0;
	
	bsl::string raddr;
	bsl::string rdata;
	int i;
	GalileoCluster *cluster = NULL;
	cluster_wrap_t *cluster_wrap = NULL;
	int ret;

	
	for (i = 0; i < service_num; i++) {

		RETURN_IF_NO_VALUE(regresaddr_conf[i], get_bsl_string, 
			LCCLIENT_CONF_GALILEO_REGSES_RESADDR_ADDR, raddr, -1);
		
		RETURN_IF_NO_VALUE(regresaddr_conf[i], get_bsl_string,
			LCCLIENT_CONF_GALILEO_REGSES_RESADDR_DATA, rdata, -1);
		
		galileo_regres_t *regres = new (std::nothrow)galileo_regres_t((int)rdata.size()+1);
		if (NULL == regres) {
			LC_LOG_WARNING("new galileo_regres_t error[%m]");
			break;
		}
		snprintf(regres->res_addr, LCCLIENT_PATH_LEN, "%s", raddr.c_str());
		snprintf(regres->res_data, rdata.size()+1, "%s", rdata.c_str());

		int host_index = 0;
		if(m_new_version) {
			RETURN_IF_NO_VALUE(regresaddr_conf[i], get_int32, "HOST_INDEX", host_index, -1);
		} 
		
		if(host_index < 0 || host_index >= (int)m_cluster_vector.size()) {
			LC_LOG_WARNING("Invalid HOST_INDEX (%d)", host_index);						
			break;
		}
		
		cluster_wrap = m_cluster_vector[host_index];
		cluster = cluster_wrap->cluster;

		
		int tagno;
		
		ret = _register_resource(cluster, raddr.c_str(), rdata.c_str(), &tagno);
		if (ret < 0) {
			delete regres;
			LC_LOG_WARNING("Register resource (%s) failed.", raddr.c_str());
			break;
		} else {
			regres->res_tag = tagno;
			_reg_resource_list.push_back(regres);
		}					 
	}
	if (i < service_num) {
		LC_LOG_WARNING("Process REGISTER_RESOUCE group failed.");
		return -1;
	}
	LC_LOG_DEBUG("Process REGISTER_RESOURCE group success.");
	return 0;
}



    int LcClientManager :: exitThreadUsingInsidePool() 
    {
        LcClientEventPool *currPool = (LcClientEventPool*)pthread_getspecific(_poolkey);
        if (NULL != currPool) {
            delete currPool;
            currPool = NULL;
            pthread_setspecific(_poolkey, NULL);
            return 0;
        }

        return 0;
    }

    void LcClientManager :: deleteServiceMap()
    {
        clearService();
        return;
    }

    void LcClientManager :: deleteConnMap()
    {
        
        bsl::string serviceName;
        conn_sev_hashmap :: iterator it1 = _conn_map.begin();
        for (; it1!=_conn_map.end(); it1++) {
            serviceName = (*it1).second;
            LcClientPool *pool;
            if (getService(serviceName.c_str(), &pool) != 0) {
                continue;
            }
            else if (NULL == pool) {
                continue;
            }
            else {
                LcClientConnection *conn = reinterpret_cast<LcClientConnection *>((*it1).first);
                if (NULL != conn) {
                    pool->freeConnection(conn);
                }
            }
        }
        _conn_map.destroy();

        return;   
    }

    int LcClientManager :: stop_healthy_check()
    {
        if (1 == _healthy_run) {
            _healthy_run = 0;
            pthread_join(_healthy_check_t, NULL);
        }
        return 0;
    }

    int LcClientManager :: stopHealthyCheck()
    {
        return stop_healthy_check();
    }

    int LcClientManager :: healthyCheckOnce() 
    {
        LC_LOG_DEBUG("%s", __func__);
    
        _service_rwlock.read();
        int vec_size = (int)_service_map.size();
        for (int i=0; i<vec_size; i++) {
            LcClientPool *pool = _service_map[i]->pool;
            if (pool->isEnable()) {
                pool->healthyCheck();
            }
        }
        _service_rwlock.unlock();

        return 0;
    } 
    
    int LcClientManager :: nshead_async_talk(const char *servicename, nshead_talkwith_t *talkmsg, 
                                             lc::IEvent *event, bsl::var::IVar &confVar)
    {
        LC_LOG_DEBUG("%s", __func__);

        if (NULL == servicename || NULL == talkmsg || NULL == event) {
            LC_LOG_WARNING("input NULL");
            return -1;
        }

        if (NULL == talkmsg->talkwithcallback) {
            LC_LOG_WARNING("not set user callback, logid:%u", talkmsg->reqhead.body_len);
            return -1;
        }
        talkmsg->servicename = servicename;
        
        _service_rwlock.read();
        LcClientPool *currPool = getManagerPool(servicename);
        if (NULL == currPool) {
            _service_rwlock.unlock();
            LC_LOG_WARNING("[%s] service not exist, logid:%u", servicename, talkmsg->reqhead.body_len);
            return -1;
        }

        talkmsg->innerTalk.mgr = this;

        int ret = currPool->fetchConnectionAsync(talkmsg, confVar, event, _netreactor);
        
        _service_rwlock.unlock();
        
        return ret;
    }

	
	strategy_ioc *lcclient_get_strategyioc()
	{
        if (NULL == g_LcClientStrategyIoc) {
            g_LcClientStrategyIoc = new (std::nothrow)strategy_ioc;
            if (NULL == g_LcClientStrategyIoc) {
                return NULL;
            }
        }
		return g_LcClientStrategyIoc;
	}

	
	healthychecker_ioc *lcclient_get_healthycheckerioc()
	{
        if (NULL == g_LcClientHealthyCheckerIoc) {
            g_LcClientHealthyCheckerIoc = new (std::nothrow)healthychecker_ioc;
            if (NULL == g_LcClientHealthyCheckerIoc) {
                return NULL;
            }
        }

		return g_LcClientHealthyCheckerIoc;
	}

    void LcClientManager :: clearService()
    {
        int vec_size = (int)_service_map.size();
        for (int i=0; i<vec_size; i++) {
            if (NULL != _service_map[i]) {
                if (NULL != _service_map[i]->pool) {
                    delete _service_map[i]->pool;
                    _service_map[i]->pool = NULL;
                }

                delete _service_map[i];
                _service_map[i] = NULL;
            }
        }
        _service_map.clear();
    }

    int LcClientManager :: addService(const char* serviceName, LcClientPool *pool,
                                      const char *proxyname, const char *sname, int tag)
    {
        service_info *tempService = new (std::nothrow)service_info;
        if (NULL == tempService) {
            LC_LOG_WARNING("new buffer for service_info error [%m]");
            return -1;
        }
        tempService->pos = tag;
        int tempLen = snprintf(tempService->serviceName, sizeof(tempService->serviceName), "%s", serviceName);
        if (tempLen >= LCCLIENT_SERVERNAME_LEN) {
            LC_LOG_WARNING("service name is too long, max[%d], curr[%d]",
                    LCCLIENT_SERVERNAME_LEN, tempLen);
            delete tempService;
            tempService = NULL;
            return -1;
        }
        tempLen = snprintf(tempService->proxyName, sizeof(tempService->proxyName), "%s", proxyname);
        if (tempLen >= LCCLIENT_SERVERNAME_LEN) {
            LC_LOG_WARNING("proxy name is too long, max[%d], curr[%d]",
                    LCCLIENT_SERVERNAME_LEN, tempLen);
            delete tempService;
            tempService = NULL;
            return -1;
        }
        tempLen = snprintf(tempService->simpleName, sizeof(tempService->simpleName), "%s", sname);
        if (tempLen >= (int)sizeof(tempService->simpleName)) {
            LC_LOG_WARNING("proxy name is too long, max[%d], curr[%d]",
                            (int)sizeof(tempService->simpleName), tempLen);
            delete tempService;
            tempService = NULL;
            return -1;
        }
        tempService->pool = pool;
        _service_map.push_back(tempService);
        
        return 0;   
    }
    
    int LcClientManager :: findService(const char* serviceName)
    {
        int vec_size = (int)_service_map.size();
        for (int i=0; i<vec_size; i++) {
            if (strcmp(serviceName, _service_map[i]->serviceName) == 0) {
                return 0;
            }
        }

        return -1;
    }

    int LcClientManager :: getService(const char* serviceName, LcClientPool **pool)
    {
        int vec_size = (int)_service_map.size();
        for (int i=0; i<vec_size; i++) {
            if (strcmp(serviceName, _service_map[i]->serviceName) == 0) {
                *pool = _service_map[i]->pool;
                return 0;
            }
        }
        
        return -1;
    }

    const char * LcClientManager :: getProxyName(const char *serviceName)
    {
        int vec_size = (int)_service_map.size();
        for (int i=0; i<vec_size; i++) {
            if (strcmp(serviceName, _service_map[i]->serviceName) == 0) {
                return _service_map[i]->proxyName;
            }
        }
        return NULL;
    }    

    const char * LcClientManager :: getSimpleName(const char *serviceName)
    {
        int vec_size = (int)_service_map.size();
        for (int i=0; i<vec_size; i++) {
            if (strcmp(serviceName, _service_map[i]->serviceName) == 0) {
                return _service_map[i]->simpleName;
            }
        }
        return NULL;       
    }

    int LcClientManager :: getFD(nshead_talkwith_t* talkmsg, bsl::var::IVar & confVar,
                                 LcClientPool *currPool, const char* servicename)
    {
        if (NULL == talkmsg || NULL == currPool || NULL == servicename) {
            return -1;
        }

        talkmsg->success = 0;

        if (NULL == talkmsg) {
            LC_LOG_WARNING("nshead_talk : is NULL");
            talkmsg->success = lc::NSHEAD_LC_INIT_ERROR;
            return -1;
        }

        int errNo = 0;
        LcClientConnection *conn = currPool->fetchConnection(talkmsg->defaultserverarg,
                                                            confVar, &errNo, this);
        if (NULL == conn) {
            LC_LOG_WARNING("nshead_talk : fetchConnection error");
            if (-1 != errNo) {
                talkmsg->success = errNo;
            } else {
                talkmsg->success = lc::NSHEAD_LC_INIT_ERROR;
            }
            return -1;
        }

        conn->setTID(pthread_self());
        int *strFD = (int*)(conn->getHandle());
        if (NULL == strFD) {
            talkmsg->success = lc::NSHEAD_TCP_CONNECT_ERROR;
            LC_LOG_WARNING("nshead_talk: socket fd error");
            conn->setErrno(lc::NSHEAD_TCP_CONNECT_ERROR);
            currPool->freeConnection(conn);
            return -1;
        }
        int currFD = *strFD;
        if (currFD < 0) {
            talkmsg->success = lc::NSHEAD_TCP_CONNECT_ERROR;
            LC_LOG_WARNING("nshead_talk: socket fd error");
            conn->setErrno(lc::NSHEAD_TCP_CONNECT_ERROR);
            currPool->freeConnection(conn);
            return -1;
        }

        int handleError = 0;
        socklen_t len = 0;
        len = sizeof(handleError);

        int val = getsockopt(currFD, SOL_SOCKET, SO_ERROR, &handleError, &len);
        if (-1 == val) {
            talkmsg->success = lc::NSHEAD_TCP_CONNECT_ERROR;
            LC_LOG_WARNING("nshead_talk : get sockFD [%d] error [%d]", handleError, handleError);
            conn->setErrno(lc::NSHEAD_TCP_CONNECT_ERROR);
            currPool->freeConnection(conn);
            return -1;
        };

        lcclient_nshead_atalk_t *currTalk = &talkmsg->innerTalk;
        currTalk->conn = conn;
        currTalk->pool = currPool;

        int serverID = conn->serverID();
        LcClientServer *ser = fetchServer(servicename, serverID);
        if (NULL == ser) {
            talkmsg->success = lc::NSHEAD_LC_FETCH_SERVER_ERROR;
            LC_LOG_WARNING("nshead_talk fetch server error");
            conn->setErrno(lc::NSHEAD_TCP_CONNECT_ERROR);
            currPool->freeConnection(conn);
            return -1;
        }
        currTalk->server = ser;
        currTalk->readtimeout = ser->getReadTimeout();
        currTalk->writetimeout = ser->getWriteTimeout();

        return currFD;      
    }

    int LcClientManager :: postSingleTalkBlock(nshead_talkwith_t* talkmsg, bsl::var::IVar & confVar,
                            LcClientPool *currPool, const char* servicename)
    {
        int ret = 0;
        struct timeval s, e;

        struct timeval fetch_start, fetch_end;
        gettimeofday(&fetch_start, NULL);

        int fd = getFD(talkmsg, confVar, currPool, servicename);

        gettimeofday(&fetch_end, NULL);

        if (fd < 0){
            goto Fail1;
        } else {
            add_monitor_info_ipport("QUERY_CONNECT_NUMBER", fd, servicename);
            int fetch_time = (fetch_end.tv_sec-fetch_start.tv_sec)*1000 + 
                    (fetch_end.tv_usec-fetch_start.tv_usec)/1000;\
            add_monitor_info_ipport_ex("CONNECT_TIME", fd, servicename, fetch_time);

            gettimeofday(&s, NULL);
            talkmsg->reqhead.magic_num = NSHEAD_MAGICNUM;
            ret = nshead_write_ex(fd, &talkmsg->reqhead, talkmsg->reqbuf, 
                                talkmsg->innerTalk.writetimeout);
            if (ret != 0) {
                add_monitor_info_ipport("WRITE_ERROR_NUMBER", fd, servicename);
                talkmsg->innerTalk.conn->setErrno(lc::NSHEAD_WRITE_HEAD_ERROR);
                talkmsg->innerTalk.pool->freeConnection(talkmsg->innerTalk.conn);
                LC_LOG_WARNING("%s Server : %s:%d nshead_write_ex error %d", __func__,
                                talkmsg->innerTalk.server->getIP(talkmsg->innerTalk.conn->getCurrIP()),
                                talkmsg->innerTalk.server->getPort(), ret);                
                talkmsg->success = lc::NSHEAD_WRITE_HEAD_ERROR;
                goto Fail;
            }
            gettimeofday(&e, NULL);
            talkmsg->returninfo.realwritetime =
                                (int)((e.tv_sec-s.tv_sec)*1000+(e.tv_usec-s.tv_usec)/1000);
            gettimeofday(&s, NULL);
            ret = ul_sreado_ms_ex2(fd, &talkmsg->reshead, sizeof(nshead_t),
                                   talkmsg->innerTalk.readtimeout);
            if (ret <= 0) {
                add_monitor_info_ipport("READ_ERROR_NUMBER", fd, servicename);
                talkmsg->innerTalk.conn->setErrno(lc::NSHEAD_READ_HEAD_ERROR);
                talkmsg->innerTalk.pool->freeConnection(talkmsg->innerTalk.conn);
                LC_LOG_WARNING("%s Server : %s:%d read nshead_read_ex error %d", __func__,
                                talkmsg->innerTalk.server->getIP(talkmsg->innerTalk.conn->getCurrIP()),
                                talkmsg->innerTalk.server->getPort(), ret);                
                talkmsg->success = lc::NSHEAD_READ_HEAD_ERROR;
                goto Fail;
            }
            else if (ret != (int)sizeof(nshead_t)) {
                talkmsg->innerTalk.conn->setErrno(lc::NSHEAD_READ_HEAD_ERROR);
                talkmsg->innerTalk.pool->freeConnection(talkmsg->innerTalk.conn);
                LC_LOG_WARNING("%s Server : %s:%d nshead_read_ex fail: ret %d want %d ERR[%m]",
                                __func__,
                                talkmsg->innerTalk.server->getIP(talkmsg->innerTalk.conn->getCurrIP()),
                                talkmsg->innerTalk.server->getPort(), ret, (int)sizeof(nshead_t));                
                talkmsg->success = lc::NSHEAD_READ_HEAD_ERROR;
                goto Fail;
            }
            
            if (talkmsg->reshead.magic_num != NSHEAD_MAGICNUM) {
                talkmsg->innerTalk.conn->setErrno(lc::NSHEAD_READ_HEAD_ERROR);
                talkmsg->innerTalk.pool->freeConnection(talkmsg->innerTalk.conn);
                LC_LOG_WARNING("%s Server : %s:%d nshead_read_ex fail: magic_num error", __func__,
                                talkmsg->innerTalk.server->getIP(talkmsg->innerTalk.conn->getCurrIP()),
                                talkmsg->innerTalk.server->getPort());                
                talkmsg->success = lc::NSHEAD_READ_HEAD_ERROR;
                goto Fail;
            }

            if (NULL != talkmsg->alloccallback && talkmsg->reshead.body_len > 0) {
                lc_callback_alloc_t allocarg;
                allocarg.size = talkmsg->reshead.body_len;
                allocarg.arg = talkmsg->allocarg;
                int ret1 = talkmsg->alloccallback(&allocarg);
                if (ret1 != 0) {
                    talkmsg->innerTalk.conn->setErrno(lc::NSHEAD_LC_USER_CALLBACK_ERROR);
                    talkmsg->innerTalk.pool->freeConnection(talkmsg->innerTalk.conn);
                    talkmsg->success = lc::NSHEAD_LC_USER_CALLBACK_ERROR;
                    goto Fail;
                }
                
                talkmsg->resbuf = (char *)allocarg.buff;
            } else if (NULL == talkmsg->alloccallback) {
                if (talkmsg->reshead.body_len > talkmsg->maxreslen) {
                    talkmsg->innerTalk.conn->setErrno(lc::NSHEAD_READ_BODY_BUF_NO_ENOUGH);
                    talkmsg->innerTalk.pool->freeConnection(talkmsg->innerTalk.conn);
                    talkmsg->success = lc::NSHEAD_READ_BODY_BUF_NO_ENOUGH;
                    goto Fail;
                }
            }

            if (talkmsg->reshead.body_len > 0) {
                ret = ul_sreado_ms_ex2(fd, talkmsg->resbuf, talkmsg->reshead.body_len,
                                        talkmsg->innerTalk.readtimeout);
                if (ret <= 0) {
                    add_monitor_info_ipport("READ_ERROR_NUMBER", fd, servicename);
                    talkmsg->innerTalk.conn->setErrno(lc::NSHEAD_READ_BODY_ERROR);
                    talkmsg->innerTalk.pool->freeConnection(talkmsg->innerTalk.conn);
                    LC_LOG_WARNING("%s Server : %s:%d nshead_read_ex error %d", __func__,
                                talkmsg->innerTalk.server->getIP(talkmsg->innerTalk.conn->getCurrIP()),
                                talkmsg->innerTalk.server->getPort(), ret);                    
                    talkmsg->success = lc::NSHEAD_READ_BODY_ERROR;
                    if (NULL != talkmsg->freecallback) {
                        lc_callback_alloc_t allocarg;
                        allocarg.buff = talkmsg->resbuf;
                        allocarg.size = talkmsg->reshead.body_len;
                        allocarg.arg = talkmsg->allocarg;
                        talkmsg->freecallback(&allocarg);
                        talkmsg->resbuf = NULL;
                    }
                    goto Fail;
                } else if (ret != (int)talkmsg->reshead.body_len) {
                    talkmsg->innerTalk.conn->setErrno(lc::NSHEAD_READ_BODY_ERROR);
                    talkmsg->innerTalk.pool->freeConnection(talkmsg->innerTalk.conn);
                    LC_LOG_WARNING("%s Server : %s:%d nshead_read_ex error ret %d req %d",
                                    __func__,
                                    talkmsg->innerTalk.server->getIP(talkmsg->innerTalk.conn->getCurrIP()),
                                    talkmsg->innerTalk.server->getPort(), ret,
                                    talkmsg->reshead.body_len);                    
                    talkmsg->success = lc::NSHEAD_READ_BODY_ERROR;
                    if (NULL != talkmsg->freecallback) {
                        lc_callback_alloc_t allocarg;
                        allocarg.buff = talkmsg->resbuf;
                        allocarg.size = talkmsg->reshead.body_len;
                        allocarg.arg = talkmsg->allocarg;
                        talkmsg->freecallback(&allocarg);
                        talkmsg->resbuf = NULL;
                    }
                    goto Fail;
                }
            }
            gettimeofday(&e, NULL);
            talkmsg->returninfo.realreadtime =
                                    (int)((e.tv_sec-s.tv_sec)*1000+(e.tv_usec-s.tv_usec)/1000);
        }

        
        if (NULL != talkmsg->rpccallback1) {
            int ret2 = talkmsg->rpccallback1((void *)talkmsg);
            if (ret2 == 1) {
                
                talkmsg->innerTalk.conn->setErrno(lc::LC_RPC_CLOSE_CONN);
            }
        }
        talkmsg->innerTalk.pool->freeConnection(talkmsg->innerTalk.conn);

        
        talkmsg->innerTalk.pool->setServerArg(talkmsg->innerTalk.server, talkmsg);
        return 0;
    Fail :
        talkmsg->innerTalk.pool->setServerArg(talkmsg->innerTalk.server, talkmsg);
        return -1;
    Fail1 :
        return -1;
    }

int LcClientManager :: insertRegisterResource(const char *path, const char *res_data)
{
    if (NULL == path || NULL == res_data) {
        return -1;
    }

	if(m_cluster_vector.size() <= 0) {
		LC_LOG_WARNING("m_cluster_vector size is 0.");
		return -1;
	}
	cluster_wrap_t *cluster_wrap = m_cluster_vector[0];
	GalileoCluster *cluster = cluster_wrap->cluster;

	
    _regres_lock.lock();
    regres_list::iterator it1;
    for (it1=_reg_resource_list.begin(); it1!=_reg_resource_list.end(); it1++) {
        if ((strcmp((*it1)->res_addr, path) == 0) && (strcmp((*it1)->res_data, res_data) == 0)) {
            LC_LOG_WARNING("%s always in data[%s]", path, res_data);
            _regres_lock.unlock();
            return (*it1)->res_tag;
        }
    }
    galileo_regres_t *regres = new (std::nothrow)galileo_regres_t(strlen(res_data)+1);
    if (NULL == regres) {
        _regres_lock.unlock();
        LC_LOG_WARNING("new galileo_regres_t errror[%m]");
        return -1;
    }

	
	int tagno;
	int ret = _register_resource(cluster, path, res_data, &tagno);
    if (ret < 0) {
        _regres_lock.unlock();
        delete regres;
        return ret;
    }
    snprintf(regres->res_addr, LCCLIENT_PATH_LEN, "%s", path);
    snprintf(regres->res_data, strlen(res_data)+1, "%s", res_data);
    regres->res_tag = tagno;
    if( 0 != _reg_resource_list.push_back(regres)) {
        _regres_lock.unlock();
		cluster->delete_resource(path, regres->res_tag);
        delete regres;
        return -1;
    }
    _regres_lock.unlock();
    return regres->res_tag;
}

int LcClientManager :: delRegisterResource(const char *path, const int res_tag)
{
    if (NULL == path || res_tag <= 0) {
        return -1;
    }
	if(m_cluster_vector.size() <= 0) {
		LC_LOG_WARNING("m_cluster_vector size is 0.");
		return -1;
	}
	cluster_wrap_t *cluster_wrap = m_cluster_vector[0];
	GalileoCluster *cluster = cluster_wrap->cluster;
	
    _regres_lock.lock();
    regres_list::iterator it1;
    for (it1=_reg_resource_list.begin(); it1!=_reg_resource_list.end(); it1++) {
        if ((strcmp((*it1)->res_addr, path) == 0) && ((*it1)->res_tag == res_tag)) {
			int ret = cluster->delete_resource(path, res_tag);
            if (ret != 0) {
                LC_LOG_WARNING("delRegisterResource error[%d]", ret);
                _regres_lock.unlock();
                return ret;
            }
            galileo_regres_t * temp_item = (*it1);
            delete temp_item;
            _reg_resource_list.erase(it1);
            _regres_lock.unlock();
            return ret;
        }
    }
    _regres_lock.unlock();
    return -1;
}

bsl::var::IVar & LcClientManager :: insertDependingResource(const char *name, const char *addr,
                                                            bsl::ResourcePool *rp)
{
    static bsl::var::Null empty_var;
    if (NULL == name || NULL == addr || NULL == rp) {
        LC_LOG_WARNING("%s input error", __func__);
        return empty_var;
    }
	if(m_cluster_vector.size() <= 0) {
		LC_LOG_WARNING("m_cluster_vector size is 0.");
		return empty_var;
	}
	cluster_wrap_t *cluster_wrap = m_cluster_vector[0];
	GalileoCluster *cluster = cluster_wrap->cluster;
	

    
    int reg_tag = 0;
    _depres_lock.lock();
    path_sev_map :: iterator it1;
    for(it1=_galileo_servmap.begin(); it1!=_galileo_servmap.end(); it1++) {
        if (strcmp((*it1).first.c_str(), addr) == 0 && strcmp((*it1).second.c_str(), name) == 0) {
            reg_tag = 1;
            break;
        }
    }
    
    bsl::string strAddr = addr;
    int ret = 0;

	bsl::var::IVar &conf = rp->create<bsl::var::Dict>();
	ret = cluster->query_resource(addr, conf, *rp);
    if (0 != ret) {
        LC_LOG_WARNING("galileo query resource [%s] [%s] error", addr, name);
        _depres_lock.unlock();
        return empty_var;
    }
    
    _service_rwlock.write();
    if (0 == reg_tag) {
        LcClientPool *pool = NULL;
        if(0 == getService(name, &pool)) {
            if (!pool->isEnable()) {
                pool->setEnable(1);
            } else {
                _service_rwlock.unlock();
                _depres_lock.unlock();
                return empty_var;
            }
        } else {
            bsl::ResourcePool currPool;
            try {
                bsl::var::IVar & galileoConfVar = currPool.create<bsl::var::Array>();
                comcfg::Configure tmp_conf;
                ret = init(conf, name, currPool, galileoConfVar, tmp_conf, 0, "", name);
                if (0 != ret) {
                    LC_LOG_WARNING("init pool from [%s] [%s] error", addr, name);
                    _service_rwlock.unlock();
                    _depres_lock.unlock();
                    return empty_var;
                }
            } catch (bsl::Exception& e) {
                LC_LOG_WARNING("error [%s]", e.what());
                _service_rwlock.unlock();
                _depres_lock.unlock();
                return empty_var;
            } catch (std::exception& e){
                LC_LOG_WARNING("error [%s]", e.what());
                _service_rwlock.unlock();
                _depres_lock.unlock();
                return empty_var;
            } catch (...) {
                LC_LOG_WARNING("error unknown");
                _service_rwlock.unlock();
                _depres_lock.unlock();
                return empty_var;
            }
        }
    }
	
	ret = cluster->watch_resource(addr, galileo_event_callback, cluster_wrap);
    if (0 != ret) {
        LC_LOG_WARNING("galileo set watching [%s] error", addr);
        
        if (0 == reg_tag) {
            serv_vec :: iterator it1;
            for (it1=_service_map.begin(); it1!=_service_map.end(); it1++) {
                if (strcmp((*it1)->serviceName, name) == 0) {
                    delete (*it1)->pool;
                    _service_map.erase(it1);
                    break;
                }
            }
        }
        _service_rwlock.unlock();
        _depres_lock.unlock();
        return empty_var;
    }
    _service_rwlock.unlock();
    if (0 == reg_tag) {
        path_sev_pair tempPair(addr, name);
        _galileo_servmap.insert(tempPair);
    }
    _depres_lock.unlock();
    return conf;
}

int LcClientManager :: delDependingResource(const char *name, const char *addr)
{
    
    
    if (NULL == name || NULL == addr) {
        LC_LOG_WARNING("%s input error", __func__);
        return -1;
    }

	if(m_cluster_vector.size() <= 0) {
		LC_LOG_WARNING("m_cluster_vector size is 0.");
		return -1;
	}
	cluster_wrap_t *cluster_wrap = m_cluster_vector[0];
	GalileoCluster *cluster = cluster_wrap->cluster;
	
    _depres_lock.lock();
    path_sev_map :: iterator it1, it_result;
    int count = 0;
    int find = 0;
    for(it1=_galileo_servmap.begin(); it1!=_galileo_servmap.end(); it1++) {
        if (count == 0) {
            if (strcmp((*it1).first.c_str(), addr) == 0 && strcmp((*it1).second.c_str(), name) == 0) {
                it_result = it1;
                ++ find;
                ++ count;
            } else {
                if (strcmp((*it1).first.c_str(), addr) == 0) {
                    ++ count;
                }
            }
        } else {
            if (strcmp((*it1).first.c_str(), addr) == 0) {
                ++ count;
            }
        }
    }
    if (0 == find) {
        
        _depres_lock.unlock();
        return -2;
    }
    if (1 == count) {
		cluster->delete_watch((*it_result).first.c_str());
    }
    _galileo_servmap.erase(it_result);
    _service_rwlock.read();
    int vec_size = (int)_service_map.size(); 
    for (int i=0; i<vec_size; i++) {
        if (strcmp(name, _service_map[i]->serviceName) == 0) {
            _service_map[i]->pool->setEnable(0);
            break;
        }
    }
    _service_rwlock.unlock();         
    _depres_lock.unlock();
    return 0;
}
    
    int LcClientManager :: common_async_talk(const char *servicename, LcEvent *event,
                                             bsl::var::IVar &confVar)
    {
        
        LC_LOG_DEBUG("%s", __func__);

        if (NULL == servicename || NULL == event) {
            LC_LOG_WARNING("input NULL");
            return -1;
        }
        event->setResult(0);
        event->set_sock_status(0);
        common_talkwith_t *talkmsg = (common_talkwith_t*)(event->get_tag());
        if (NULL == talkmsg) {
            LC_LOG_WARNING("set talkwith is set_tag");
            return -1;
        }
        if (talkmsg->wait == DUSEP_TALK_NOWAIT || talkmsg->wait == DUSEP_TALK_WAIT) {
            if (event->reactor() == NULL) {
                LC_LOG_WARNING("duplex communication requires set reactor for event outside");
                return -1;
            }
        }
        talkmsg->servicename = servicename;
        talkmsg->mgr = this;

        if (NULL == talkmsg->talkwithcallback) {
            LC_LOG_WARNING("not set user callback");
            return -1;
        }
        
        _service_rwlock.read();
        LcClientPool *currPool = getManagerPool(servicename);
        if (NULL == currPool) {
            _service_rwlock.unlock();
            LC_LOG_WARNING("[%s] service not exist", servicename);
            return -1;
        }
    
        talkmsg->mgr = this;
        int ret = currPool->fetchConnectionAsync(talkmsg, confVar, event, _netreactor);
        
        _service_rwlock.unlock();
        
        return ret;
    }

    int LcClientManager :: setConnPool(const char *servicename, LcClientPool *pool)
    {
        if (NULL == servicename || NULL == pool) {
            LC_LOG_WARNING("%s input is NULL", __func__);
            return -1;
        }
        
        _service_rwlock.write();
        int size = _service_map.size();
        for (int i=0; i<size; i++) {
            
            if (strcmp(servicename, _service_map[i]->serviceName) == 0) {
                if (!_service_map[i]->pool->isEnable()) {
                    _service_map[i]->pool->setEnable(1);
                    _service_rwlock.unlock();
                    return -2;
                }
                _service_rwlock.unlock();
                return -3;
            }
        }
        service_info *servinfo = new (std::nothrow)service_info;
        if (NULL == servinfo) {
            LC_LOG_WARNING("new service_info error[%m]");
            _service_rwlock.unlock();
            return -1;
        }
        int ret = snprintf(servinfo->serviceName, LCCLIENT_SERVERNAME_LEN, "%s", servicename);
        if (ret >= LCCLIENT_SERVERNAME_LEN) {
            delete servinfo;
            _service_rwlock.unlock();
            LC_LOG_WARNING("servicename is too long req[%d] real[%d]", LCCLIENT_SERVERNAME_LEN, ret);
            return -4;
        }
        servinfo->pool = pool;
        _service_map.push_back(servinfo);
        _service_rwlock.unlock();
        return 0;
    }

    int LcClientManager :: getServiceConnType(const char *servicename, bool &conntype)
    {
        comcm::AutoRead auto_read(&_service_rwlock);
        LcClientPool *pool = getManagerAllPool(servicename);
        if (NULL == pool) {
            LC_LOG_WARNING("serviceName [%s]'s pool is NULL", servicename);
            return -1;
        }
        conntype = pool->getConnectType();
        return 0;
    }

void LcClientManager :: deleteRegRes()
{
    regres_list::iterator it1;
    galileo_regres_t *curr_res = NULL;
    for (it1=_reg_resource_list.begin(); it1!=_reg_resource_list.end(); it1++) {
        curr_res = (*it1);
        if (NULL != curr_res) {
            delete curr_res;
            curr_res = NULL;
        }
    }
    _reg_resource_list.destroy();
}



int LcClientManager :: load(const comcfg::ConfigUnit &conf)
{
	
    
    if (0 == _createTag) {
        if (initResource() != 0) {
            LC_LOG_WARNING("create resource error");
            return -1;
        }            
        _service_rwlock.write();
        
        if (0 != innerLoad(conf)) {
            _service_rwlock.unlock();
            return -1;
        }
        _createTag = 1;
        _service_rwlock.unlock();
        return 0;
    } else {
        return 0;
    }

    return -1;
}

int LcClientManager :: initResource()
{
    int ret = pthread_key_create(&_poolkey, deleteKeyValue);
    if (0 != ret) {
        LC_LOG_WARNING("create thread key error [%m]");
        return -1;
    }
    ret = _conn_map.create(LCCLIENT_HASHMAP_SIZE*2);
    if (0 != ret) {
        pthread_key_delete(_poolkey);
        LC_LOG_WARNING("_conn_map[hashmap] create error[%m]");
        return -1;
    }
    ret = _reg_resource_list.create();
    if (0 != ret) {
        _conn_map.destroy();
        pthread_key_delete(_poolkey);
        LC_LOG_WARNING("_reg_resource_list[bsl::list] create error[%m]");
        return -1;
    }
    return 0;    
}








int LcClientManager::_init_with_galileo(const comcfg::ConfigUnit &conf, int conftype)
{
	LC_LOG_DEBUG("Enter _init_with_galileo.");


	try {
	RETURN_IF_KEY_ERROR(conf, LCCLIENT_CONF_GALILEO, -1);
	const comcfg::ConfigUnit &galileo_conf = conf[LCCLIENT_CONF_GALILEO];

	int config_version;
	galileo_conf["CONFIG_VERSION"].get_int32(&config_version, 0);

    bsl::string galileoDir;
    bsl::string galileoLogDir;

    
    int galileo_retry = 0;
    galileo_conf["GALILEO_RETRY_TIME"].get_int32(&galileo_retry, 0);
    int galileo_interval = 1;
    galileo_conf["GALILEO_RETRY_INTERVAL"].get_int32(&galileo_interval, 1);

	
	if (galileo_conf[LCCLIENT_CONF_GALILEO_DIR].
			get_bsl_string(&galileoDir, LCCLIENT_DEFAULT_CONFDIR) != 0) {
		LC_LOG_WARNING("Get (%s) from config file failed.", LCCLIENT_CONF_GALILEO_DIR);
	}

    bsl::string galileoFile;
	if (galileo_conf[LCCLIENT_CONF_GALILEO_FILE].
			get_bsl_string(&galileoFile, LCCLIENT_DEFAULT_GALILEOCONF) != 0) {
		LC_LOG_WARNING("Get (%s) from config file failed.", LCCLIENT_CONF_GALILEO_FILE);
	}
	
	int fileLen = snprintf(_galileoDir, sizeof(_galileoDir), "%s", galileoDir.c_str());
	if (fileLen >= (int)sizeof(_galileoDir)) {
		LC_LOG_WARNING("galileo copy' dir max than 256 bytes [%s]", _galileoDir);
	}
	fileLen = snprintf(_galileoFile, sizeof(_galileoFile), "%s", galileoFile.c_str());
	if (fileLen >= (int)sizeof(_galileoFile)) {
		LC_LOG_WARNING("galileo copy' file-name max than 256 bytes [%s]", _galileoFile);
	}

	int bufsize;
	galileo_conf[LCCLIENT_CONF_GALILEO_DATAPACKSIZE].get_int32
			(&bufsize, GALILEO_BUFFER_SIZE);
	_galileoBufferSize = bufsize;

	bsl::string galileo_host;	
	GalileoCluster *cluster = NULL;
	cluster_wrap_t *cluster_wrap = NULL;
	
	bool galileo_ok = true;
	m_new_version = false;	

	if(config_version == 0) {
	

		LC_LOG_DEBUG("Old version galileo config file.");

		bsl::string log_dir;;
		galileo_conf[LCCLIENT_CONF_GALILEO_LOG].get_bsl_string(&log_dir, "../log/zoo.pspui.log");
		int log_level;
		galileo_conf[LCCLIENT_CONF_GALILEO_LOGLEVEL].get_int32(&log_level, 4);

        
		GalileoManager::open_log(log_dir.c_str(), log_level);
        
        GalileoManager::set_getchildren_retry_time(galileo_retry, galileo_interval);

		RETURN_IF_NO_VALUE(galileo_conf, get_bsl_string, LCCLIENT_CONF_GALILEO_HOST, galileo_host, -1); 

		cluster = GalileoManager::get_instance()->get_cluster(galileo_host.c_str(), true);
		if(cluster == NULL) {
			LC_LOG_WARNING("Init with galileo (%s) failed", galileo_host.c_str());
			galileo_ok = false;
		} else {
			LC_LOG_DEBUG("Init with galileo (%s) success", galileo_host.c_str());
			cluster_wrap = (cluster_wrap_t*)malloc(sizeof(cluster_wrap_t));
			cluster_wrap->cluster = cluster;
			cluster_wrap->lccm = this;
			m_cluster_vector.push_back(cluster_wrap);
		}
		
	} else if(config_version == 1) {
	
		LC_LOG_DEBUG("New version galileo config file.");

		m_new_version = true;

		
		bsl::string log_dir;;
		galileo_conf[LCCLIENT_CONF_GALILEO_LOG].get_bsl_string(&log_dir, "../log/zoo.pspui.log");
		int log_level;
		galileo_conf[LCCLIENT_CONF_GALILEO_LOGLEVEL].get_int32(&log_level, 4);		
		
        
        GalileoManager::open_log(log_dir.c_str(), log_level);
        
        GalileoManager::set_getchildren_retry_time(galileo_retry, galileo_interval);

		RETURN_IF_KEY_ERROR(galileo_conf, LCCLIENT_CONF_GALILEO_CLUSTER, -1);
		const comcfg::ConfigUnit& cluster_conf = galileo_conf[LCCLIENT_CONF_GALILEO_CLUSTER];
		int cluster_num = cluster_conf.size();
		
		for(int i = 0; i < cluster_num; i++) {

			if(cluster_conf[i]["HOST"].get_bsl_string(&galileo_host) != 0) {
				LC_LOG_WARNING("Failed to get galileo host.");
				return -1;
			}

			cluster = GalileoManager::get_instance()->get_cluster(galileo_host.c_str(), true);
			if(cluster == NULL) {
				LC_LOG_WARNING("Init with galileo (%s) failed", galileo_host.c_str());
				galileo_ok = false;
				break;
			} else {
				LC_LOG_DEBUG("Init with galileo (%s) success", galileo_host.c_str());
				cluster_wrap = (cluster_wrap_t*)malloc(sizeof(cluster_wrap_t));
				cluster_wrap->cluster = cluster;
				cluster_wrap->lccm = this;
				m_cluster_vector.push_back(cluster_wrap);
			}
		}
	} else {
		LC_LOG_WARNING("Invalid CONFIG_VERSION, must be [0|1].");
		return -1;
	}

	if(!galileo_ok) {
	
        if(galileo_conf[LCCLIENT_CONF_GALILEO_REGSES].selfType() != comcfg::CONFIG_ERROR_TYPE) {
			
			LC_LOG_FATAL("Galileo init failed and has %s.", LCCLIENT_CONF_GALILEO_REGSES);
			return -1;
		}
		
		LC_LOG_DEBUG("Galileo init failed, using local backup");
		
		
		comcfg::Configure local_backup_conf;
		int ret = local_backup_conf.load(galileoDir.c_str(), galileoFile.c_str());
		if (ret != 0) {
			LC_LOG_DEBUG("Load galileo local backup failed, using local conf.");
			
			if (_init_with_localconf(conf) != 0) {
				LC_LOG_WARNING("Init with local conf failed.");
				return -1;
			}
		} else {
			LC_LOG_DEBUG("Load galileo local backup success.");
			
			ret = init(local_backup_conf[LCCLIENT_CONF_LCCLIENT]);	
			if (ret != 0) {
				LC_LOG_WARNING("Init with galileo local backup failed, %d", ret);
				return -1;
			}
		}

		return 0;
	} 

	
	
	
	

	const comcfg::ConfigUnit *pLocalConf = NULL;
	int merge_flag = 0;

	
	if(conftype == LCCLIENT_CONFTYPE_GALILEO) {
		pLocalConf = &conf;		
	} else if(conftype == LCCLIENT_CONFTYPE_HYBRID1) {
		pLocalConf = &(conf["LcClient"]);
	} else { 
		merge_flag = 1;
		pLocalConf = &(conf["LcClient"]);
	} 

	bsl::ResourcePool rp;
	bsl::var::IVar & galileo_conf_array = rp.create<bsl::var::Array>();
	
	
	if(_process_request_resource(galileo_conf, *pLocalConf, merge_flag, rp, galileo_conf_array) != 0)
		return -1;
	
	
	if(_process_register_resource(galileo_conf) != 0)
		return -1;
	
	
	if (saveGalileoCopy(galileo_conf_array) != 0) {
		LC_LOG_WARNING("Save galileo into local backup failed.");
		return -1;
	}
	return 0;

	
    } catch (bsl::Exception& e) {
        LC_LOG_WARNING("_init_with_galileo exception (%s) (%m)", e.what());
        return -1;
    } catch (std::exception& e){
        LC_LOG_WARNING("_init_with_galileo exception (%s) (%m)", e.what());
        return -1;
    } catch(...){
        LC_LOG_WARNING("_init_with_galileo exception (%m)");
        return -1;
    }

}


int LcClientManager :: innerLoad(const comcfg::ConfigUnit &conf)
{
    int errcode = 0;        
    _main_conf = conf.to_IVar(&_rpool, &errcode);
    if (errcode != 0) {
        LC_LOG_WARNING("make main conf to var error [%d]", errcode);
        goto Fail;
    }

    if (conf[LCCLIENT_CONF_TYPE].get_int32(&_confType, LCCLIENT_CONFTYPE_LOCAL) != 0) {
        LC_LOG_WARNING("get [ConfType] error, using default value : %d", _confType);
    }

	
    if (LCCLIENT_CONFTYPE_LOCAL == _confType){
        if (_init_with_localconf(conf) != 0) {
            goto Fail;
        }
    } else if(	_confType == LCCLIENT_CONFTYPE_GALILEO ||
		_confType == LCCLIENT_CONFTYPE_HYBRID1 ||
		_confType == LCCLIENT_CONFTYPE_HYBRID2) {
		if(_init_with_galileo(conf, _confType) != 0) {
			LC_LOG_WARNING("_init_with_galileo failed.");
			goto Fail;
		}

		if(	_confType == LCCLIENT_CONFTYPE_HYBRID1 ||
			_confType == LCCLIENT_CONFTYPE_HYBRID2) {
			if (_init_with_localconf_ext(conf) != 0) {
			    goto Fail;
			}
		}
		
    } else {
        LC_LOG_WARNING("ConfType : %d not support, please use 0/1/2/3", _confType);
        goto Fail;
    }
	
    if (0 == _isOut) {
        
        if (conf[LCCLIENT_CONF_REACTOR].selfType() == 
                comcfg::CONFIG_ERROR_TYPE) {
            LC_LOG_WARNING("[$%s] is error", LCCLIENT_CONF_REACTOR);
            goto Fail;
        }
        _netreactor = new (std::nothrow)lc::NetReactor;
        if (NULL == _netreactor) {
            LC_LOG_WARNING("new NetReactor error [%m]");
            goto Fail;
        }
        if (_netreactor->load(conf[LCCLIENT_CONF_REACTOR]) != 0) {
            delete _netreactor;
            _netreactor = NULL;
            LC_LOG_WARNING("netreactor load error");
            goto Fail;
        }
        if(_netreactor->run() <= 0) {
            LC_LOG_WARNING("net reactor run error");
            delete _netreactor;
            _netreactor = NULL;
            goto Fail;
        }
    }
    if (registerMonitorInfo() != 0) {
        LC_LOG_WARNING("register monitor info error");
        goto Fail;
    }
    _monitor_info = _monitor_rp.create<bsl::var::Dict>();
    
    if (gen_monitor_info_note() != 0) {
        LC_LOG_WARNING("gen_monitor_info error");
        goto Fail;
    }

    return 0;
Fail:
      
    
    memset(_galileoDir, 0, sizeof(_galileoDir));
    memset(_galileoFile, 0, sizeof(_galileoFile));
    
    deleteServiceMap();
    deleteConnMap();         
    deleteRegRes();
    _rpool.reset();
    
    
    if (NULL != _galileoConfSave) {
        delete [] _galileoConfSave;
        _galileoConfSaveLen = 0;
    }

    if (NULL != _netreactor && 0 == _isOut) {
        delete _netreactor;
        _netreactor = NULL;
    }        
    
    return -1;
}
    
    const LcClientPool * LcClientManager :: getServiceByName(const char *servicename)
    {
        if (NULL == servicename) {
            return NULL;
        }

        LcClientPool *pool = NULL;
        _service_rwlock.read();
        int ret = getService(servicename, &pool);
        if (0 != ret) {
            _service_rwlock.unlock();
            return NULL;
        }
        _service_rwlock.unlock();
        return pool;
    }

    int LcClientManager :: getServiceRetryTime(const char *servicename)
    {
        comcm::AutoRead auto_read(&_service_rwlock);
        LcClientPool *pool = getManagerAllPool(servicename);
        if (NULL == pool) {
            LC_LOG_WARNING("serviceName [%s]'s pool is NULL", servicename);
            return -1;
        }
        LcClientConnectManager * conn_mgr = pool->getManager();
        return conn_mgr->getRetryTime();
    }   
        
    int LcClientManager :: getServiceServerNum(const char *servicename)
    {   
        comcm::AutoRead auto_read(&_service_rwlock);
        LcClientPool *pool = getManagerAllPool(servicename);
        if (NULL == pool) {
            LC_LOG_WARNING("serviceName [%s]'s pool is NULL", servicename);
            return -1;
        }
        return pool->getServerCount();
    }

    int LcClientManager :: getServiceRealServerNum(const char *servicename)
    {   
        comcm::AutoRead auto_read(&_service_rwlock);
        LcClientPool *pool = getManagerAllPool(servicename);
        if (NULL == pool) {
            LC_LOG_WARNING("serviceName [%s]'s pool is NULL", servicename);
            return -1;
        }
        return pool->getRealServerCount();
    }


    int LcClientManager :: common_singletalk(
                                        const char *servicename,
                                        LcEvent *event,
                                        bsl::var::IVar &confVar
                                            )
    {
        if (NULL == servicename || NULL == event) {
            return -1;
        }

        if (0 != common_nonblock_singletalk(servicename, event, confVar, NULL)) {
            return -1;
        }

        wait(event, -1);
        if (event->isError()) {
            return -1;
        } else {
            return 0;
        }
    }

    int LcClientManager :: common_nonblock_singletalk(
                                const char *servicename,
                                LcEvent *event, 
                                bsl::var::IVar &confVar,
                                LcClientEventPool *pool
                             )
    {   
        if (NULL == servicename || NULL == event) {
            return -1;
        }
        event->setResult(0);
        event->set_sock_status(0);
        common_talkwith_t *talkmsg = (common_talkwith_t*)(event->get_tag());
        if (NULL == talkmsg) {
            LC_LOG_WARNING("set talkwith is set_tag");
			event->set_sock_status(LcEvent::STATUS_ERROR);
            return -1;
        }
        if (talkmsg->wait == DUSEP_TALK_NOWAIT || talkmsg->wait == DUSEP_TALK_WAIT) {
            if (event->reactor() == NULL) {
                LC_LOG_WARNING("duplex communication requires set reactor for event outside");
				event->set_sock_status(LcEvent::STATUS_ERROR);
				return -1;
            }
        }

        talkmsg->servicename = servicename;
        talkmsg->event = event;
        talkmsg->mgr = this;
        
        _service_rwlock.read();
        LcClientPool *currPool = getManagerPool(servicename);
        if (NULL == currPool) {
            _service_rwlock.unlock();
			event->set_sock_status(LcEvent::STATUS_ERROR);
            return -1;
        }

        LcClientEventPool *currEventPool = getLocalEventPool(pool);
        if (NULL == currEventPool) {
             _service_rwlock.unlock();
			 event->set_sock_status(LcEvent::STATUS_ERROR);
             return -1;
        }
        if (currEventPool->setTalk(event, talkmsg) != 0) {
            _service_rwlock.unlock();
			event->set_sock_status(LcEvent::STATUS_ERROR);
            return -1;
        }
        talkmsg->eventpool = currEventPool;
        talkmsg->talkwithcallback = lc_client_common_callback;
        event->addRef();
        int ret = currPool->fetchConnectionAsync(talkmsg, confVar, event, _netreactor);       
        _service_rwlock.unlock();
        if (0 != ret) {
			
			if(event->get_sock_status() == 0)
				event->set_sock_status(LcEvent::STATUS_ERROR);
            currEventPool->delTalk(event);
            event->release();
        }

        return ret;
    }

    int LcClientManager :: common_talk(
                                    const char *servicename,
                                    LcEvent *event_vec[],
                                    int count,
                                    bsl::var::IVar & confVar
                                    )
    {
        if (NULL == servicename || NULL == event_vec || count <= 0) {
            return -1;
        }

        for (int i=0; i<count; i++) {
            common_nonblock_singletalk(servicename, event_vec[i], confVar, NULL);
        }

        
        for (int i=0; i<count; i++) {
            if (!isCommonTalkInProcessing(event_vec[i])) {
                continue;
            }
            wait(event_vec[i], -1);
        }

        return 0;
    }

    int LcClientManager :: common_nonblock_talk(
                                            const char *servicename,
                                            LcEvent *event_vec[],
                                            int count,
                                            bsl::var::IVar & confVar,
                                            LcClientEventPool *pool
                                            )
    {
        if (NULL == servicename || NULL == event_vec || count <= 0) {
            return -1;
        }
        for (int i=0; i<count; i++) {
            common_nonblock_singletalk(servicename, event_vec[i], confVar, pool);
        }

        return 0;
    }

    int LcClientManager :: wait(
                            LcEvent *req_event,
                            int ms_timeout,
                            LcClientEventPool *pool
                            )
    {
        if (NULL == req_event) {
            return -1;
        }

        common_talkwith_t *talkmsg = (common_talkwith_t*)(req_event->get_tag());
        if (NULL == talkmsg) {
            LC_LOG_WARNING("set talkwith is set_tag");
            return -1;
        }

        LcClientEventPool *currEventPool = getLocalEventPool(pool);
        if (NULL == currEventPool) {
            return -1;
        }
        return currEventPool->poll_single(talkmsg, ms_timeout);
    }

    LcEvent * LcClientManager :: fetchFirstReadyReqForCommon(LcClientEventPool *pool)
    {
        LC_LOG_DEBUG("fetchFirstReadyReq()");
        LcClientEventPool *currPool = getLocalEventPool1(pool);
        if (NULL == currPool) {
            return NULL;
        }

        common_talkwith_t *talk = currPool->fetchFirstReadyTalkForCommon();
        if (NULL == talk) {
            return NULL;
        }
        return dynamic_cast<LcEvent*>(talk->event);
    }

    int LcClientManager :: common_nonblock_singletalk_ex(
                                                    LcEvent *event,
                                                    LcClientEventPool *pool
                                                    )
    {
        if (NULL == event) {
            return -1;
        }
        event->setResult(0);
        event->set_sock_status(0);
        common_talkwith_t *talkmsg = (common_talkwith_t*)(event->get_tag());
        if (NULL == talkmsg) {
            LC_LOG_WARNING("set talkwith is set_tag");
            return -1;
        }
        talkmsg->event = event;
        
        LcClientEventPool *currEventPool = getLocalEventPool(pool);
        if (NULL == currEventPool) {
             return -1;
        }
        if (currEventPool->setTalk(event, talkmsg) != 0) {
            return -1;
        }
        talkmsg->eventpool = currEventPool;
        talkmsg->talkwithcallback = lc_client_common_callback;
        event->addRef();
        event->set_firstreactor(_netreactor);
        event->session_begin();
        return 0;
    }

    int LcClientManager :: common_singletalk_ex(LcEvent *event)
    {
        if (NULL == event) {
            return -1;
        }

        if (0 != common_nonblock_singletalk_ex(event, NULL)) {
            return -1;
        }

        wait(event, -1);
        if (event->isError()) {
            return -1;
        } else {
            return 0;
        }
    }
    
    int LcClientManager :: common_async_talk_ex(LcEvent *event)
    {
        if (NULL == event) {
            return -1;
        }
        event->setResult(0);
        event->set_sock_status(0);
        common_talkwith_t *talkmsg = (common_talkwith_t*)(event->get_tag());
        if (NULL == talkmsg) {
            LC_LOG_WARNING("set talkwith is set_tag");
            return -1;
        }
        talkmsg->event = event;
        talkmsg->talkwithcallback = lc_client_common_callback_ex;
        event->set_firstreactor(_netreactor);
        event->session_begin();
        return 0;
    }
    
    int LcClientManager :: common_nonblock_talk_ex(LcEvent *event_vec[], int num, LcClientEventPool *pool)
    {
        if (NULL == event_vec || num <= 0) {
            return -1;
        }

        int ret = 0;
        for (int i=0; i<num; i++) {
            ret = common_nonblock_singletalk_ex(event_vec[i], pool);
            if (0 != ret && NULL != event_vec[i]) {
                event_vec[i]->set_sock_status(LcEvent::STATUS_ERROR);
            }
        }
        
        
        for (int i=0; i<num; i++) {
            if (isCommonTalkInProcessing(event_vec[i], pool) == 1) {
                wait(event_vec[i], -1);
            }
        }
            
        return 0;
    }






int LcClientManager::resourceReload(const comcfg::ConfigUnit &conf)
{
    
    

	
    bsl::ResourcePool rp;
    int reload_tag = 0;

    try {
        AutoMLock lock1(_depres_lock);
        bsl::var::IVar & galileoConfVar = rp.create<bsl::var::Array>();

		RETURN_IF_KEY_ERROR(conf, LCCLIENT_CONF_GALILEO_REQSES, -1);
		const comcfg::ConfigUnit &reqres_conf = conf[LCCLIENT_CONF_GALILEO_REQSES];

		RETURN_IF_KEY_ERROR(reqres_conf, LCCLIENT_CONF_GALILEO_REQSES_RESADDR, -1);
		const comcfg::ConfigUnit &reqresaddr_conf = reqres_conf[LCCLIENT_CONF_GALILEO_REQSES_RESADDR];		

        int service_num = reqresaddr_conf.size();

		if(m_cluster_vector.size() <= 0) {
			return -1;
		}
		cluster_wrap_t *cluster_wrap = m_cluster_vector[0];
		GalileoCluster *cluster = cluster_wrap->cluster;

        int i = 0;
		int ret;
		
        
        for (i = 0; i < service_num; i++) {
            bsl::string service_name;
            bsl::string proxy;
            bsl::string addr;
            bsl::string proxy_name;
            bsl::string simple_name;
			
            if (reqresaddr_conf[i][LCCLIENT_CONF_GALILEO_REQSES_RESADDR_NAME].
                    get_bsl_string(&service_name) != 0) {
                
                LC_LOG_WARNING("galileo conf service name not exist");
                reload_tag = 1;
                continue;
            }
            if (reqresaddr_conf[i][LCCLIENT_CONF_GALILEO_REQSES_RESADDR_PROXY].get_bsl_string(&proxy) == 0) {
                
                
                const char *c_proxy = proxy.c_str();
                if ('/' != c_proxy[0]) {
                    LC_LOG_WARNING("proxy is error, proxy[%s]", c_proxy);
                    reload_tag = 1;
                    continue;
                }
                int ret = strrchr(c_proxy, '/') - c_proxy;
                if (0 == ret || 1 == ret || ret == ((int)proxy.size()-1)) {
                    LC_LOG_WARNING("proxy is error, proxy[%s]", c_proxy);
                    reload_tag = 1;
                    continue;
                }
                char c_addr[ret+1];
                strncpy(c_addr, c_proxy, ret);
                c_addr[ret] = '\0';
                addr = c_addr;
                int proxy_len = proxy.size();
                char c_name[proxy_len-ret+1];
                strncpy(c_name, c_proxy+ret+1, proxy_len-ret);
                c_name[proxy_len-ret] = '\0';
                proxy_name = c_name;
                  
            } else if (reqresaddr_conf[i][LCCLIENT_CONF_GALILEO_REQSES_RESADDR_ADDR].get_bsl_string(&addr) != 0) {
                
                
                const char *c_name = service_name.c_str();
                if ('/' != c_name[0]) {
                    LC_LOG_WARNING("servicename is error, name[%s]", c_name);
                    reload_tag = 1;
                    continue;
                }
                int ret = strrchr(c_name, '/') - c_name;
                if (0 == ret || 1 == ret || ret == ((int)service_name.size()-1)) {
                    LC_LOG_WARNING("servicename is error, name[%s]", c_name);
                    reload_tag = 1;
                    continue;
                }
                char c_addr[ret+1];
                strncpy(c_addr, c_name, ret);
                c_addr[ret] = '\0';
                addr = c_addr;
                int ret1 = service_name.size() - ret;
                char s_name[ret1+1];
                strncpy(s_name, c_name+ret+1, ret1);
                s_name[ret1] = '\0';
                simple_name = s_name;
            } else {
                simple_name = service_name;
            }

			
            
            

			
			
			
			
            int find_tag = 0;
            path_sev_map::iterator idx;
			
            for (idx = _galileo_servmap.begin(); idx != _galileo_servmap.end(); idx++) {
                if (0 == strcmp((*idx).first.c_str(), addr.c_str()) && 
                    0 == strcmp((*idx).second.c_str(), service_name.c_str())) {
                    
                    
                    find_tag = 2;
                    break;
                } else if (0 == strcmp((*idx).second.c_str(), service_name.c_str())) {
                	
                    find_tag = 1;
                    break;
                }
            }

            if (find_tag == 1) {
			
				cluster->delete_watch((*idx).first.c_str());
                _galileo_servmap.erase(idx);
            } 
			if(find_tag == 0 || find_tag == 1) {
				path_sev_pair tempPair(addr.c_str(), service_name.c_str());
				_galileo_servmap.insert(tempPair);
            }

			
			bsl::var::IVar &service_conf = rp.create<bsl::var::Dict>();
			ret = cluster->query_resource(addr.c_str(), service_conf, rp);
			if (0 != ret) {
				LC_LOG_WARNING("Query resource (%s) (%s) failed.", 
					addr.c_str(), service_name.c_str());
				reload_tag = 1;
				continue;
			
			}
			LC_LOG_DEBUG("Query resource (%s) (%s) success.", 
				addr.c_str(), service_name.c_str());


            if (find_tag == 1 || find_tag == 2) {
			
                _service_rwlock.read();
                ret = init_reload(service_conf, service_name.c_str(), rp, galileoConfVar,
                           proxy_name.c_str(), simple_name.c_str());
                _service_rwlock.unlock();

                if (0 != ret) {
                    LC_LOG_WARNING("init pool from [%s] [%s] error",
                                    addr.c_str(), service_name.c_str());
                    reload_tag = 1;
                    continue;
                }
        	} else {
        	
				_service_rwlock.write();
				ret = init(service_conf, service_name.c_str(), rp, galileoConfVar, conf, 0,
						   proxy_name.c_str(), simple_name.c_str());
				_service_rwlock.unlock();
				if (0 != ret) {
					LC_LOG_WARNING("init pool from [%s] [%s] error", 
						addr.c_str(), service_name.c_str());
					reload_tag = 1;
					continue;
				}
			}
				
			if(find_tag == 0 || find_tag == 1) {
				ret = cluster->watch_resource(addr.c_str(), galileo_event_callback, cluster_wrap);
				if (0 != ret) {
					LC_LOG_WARNING("Watch resource (%s) failed.", addr.c_str());
					reload_tag = 1;
					continue;
				
				}
				LC_LOG_DEBUG("Watch resource (%s) (%s) success.", addr.c_str(), service_name.c_str());
			}			
        }

        if (i < service_num) {
            LC_LOG_WARNING("init using galileo error : not all service init OK");
            return -2;
        }

        
        if (saveGalileoCopy(galileoConfVar) != 0) {
            LC_LOG_WARNING("save conf in manager error");
            return -3;
        }
        if (reload_tag == 1) {
            return -2;
        }
        return 0;

    } catch (bsl::Exception& e) {
        LC_LOG_WARNING("resourceReload error[%s] [%m]", e.what());
        return -1;
    } catch (std::exception& e){
        LC_LOG_WARNING("resourceReload error[%s] [%m]", e.what());
        return -1;
    } catch(...){
        LC_LOG_WARNING("resourceReload error [%m]");
        return -1;
    }

    return 0;
}

    int LcClientManager :: registerMonitorInfo()
    {
        if (NULL == _monitor) {
            LC_LOG_WARNING("not set LcMonitor");
            return 0;
        }

        
        int service_num = getServiceCount();
        for (int i=0; i<service_num; i++) {
            LcClientPool *pool = const_cast<LcClientPool *>(getServiceByID(i));
            if (NULL == pool) {
                return -1;
            }
            LcClientConnectManager *mgr = pool->getManager();
            if (NULL == mgr) {
                return -1;
            }
            
            bsl::string service_name = mgr->getServiceName();
            if (_monitor) {
                if (_monitor->register_service(service_name.c_str(), _netreactor->getThread() +
_monitor_threadnum) != 0) {
                    return -1;
                }
            }
        }

        registerSingleMonitorInfo("QUERY_CONNECT_NUMBER");
        registerSingleMonitorInfo("CONNECT_FAIL_NUMBER");
        registerSingleMonitorInfo("READ_ERROR_NUMBER");
        registerSingleMonitorInfo("WRITE_ERROR_NUMBER");
        registerSingleMonitorInfo("CONNECT_TIME", 1);

        registerSingleCallbackInfo("CONNECT_POOL_USAGE");
        registerSingleCallbackInfo("CONNECT_POOL_CURENT");
        registerSingleCallbackInfo("CONNECT_POOL_MAX");

        return 0;
    }

    int LcClientManager :: registerSingleCallbackInfo(const char *name)
    {
        
        if (NULL == _monitor) {
            return 0;
        }

        
        bsl::string item_name = name;
        int service_num = getServiceCount();
        for (int i=0; i<service_num; i++) {
            LcClientPool *pool = const_cast<LcClientPool *>(getServiceByID(i));
            if (NULL == pool) {
                return -1;
            }
            LcClientConnectManager *mgr = pool->getManager();
            if (NULL == mgr) {
                return -1;
            }

            bsl::string service_name = mgr->getServiceName();
            client_monitor_info *info = &(_monitor_rp.create<client_monitor_info>());
            info->lcmgr = this;
            info->service_name =service_name;

            _monitor->register_monitor_item(service_name.append(".").append(item_name).c_str(),lc_client_get_pool_usage, (void *)info);
        }
        return 0;
       
    }

    int LcClientManager :: registerSingleMonitorInfo(const char *name, int tag)
    {
        if (NULL == _monitor) {
            return -1;
        }

        
        bsl::string item_name = name;
        int service_num = getServiceCount();
        for (int i=0; i<service_num; i++) {
            LcClientPool *pool = const_cast<LcClientPool *>(getServiceByID(i));
            if (NULL == pool) {
                return -1;
            }
            LcClientConnectManager *mgr = pool->getManager();
            if (NULL == mgr) {
                return -1;
            }
            int server_num = mgr->getServerSize();
            for (int j=0; j<server_num; j++) {
                LcClientServer *server = dynamic_cast<LcClientServer*>(mgr->getServerInfo(j));
                if (NULL == server) {
                    return -1;
                }
                int ip_num = server->getIPCount();
                for (int k=0; k<ip_num; k++) {
                    bsl::string service_name = mgr->getServiceName();
                    char server_name[32];
                    snprintf(server_name, sizeof(server_name), "%s_%d", server->getIP(), server->getPort());
                    lc_client_convert_ip(server_name);
                    char server_ip[32];
                    snprintf(server_ip, sizeof(server_ip), "%s", server->getIP(k));
                    lc_client_convert_ip(server_ip);

                    _monitor->register_item(service_name.append(".").append(item_name).append(".").append(server_name).append(".").append(server_ip).c_str(), tag);
                }
            }
        }
        return 0;
    }

    bsl::var::IVar &lc_client_get_pool_usage(bsl::var::IVar &query, bsl::ResourcePool &rp)
    {
        try {
            const char *tmp_buf = query["Arg"].c_str();
            long lp; 

            sscanf(tmp_buf, "%ld", &lp);

            client_monitor_info *info = (client_monitor_info*)((void *)lp);
            if (NULL == info) {
                return bsl::var::Null::null;
            }
 
            LcClientManager *lcmgr = info->lcmgr;
            if (NULL == lcmgr) {
                return bsl::var::Null::null;
            }

            bsl::string service_name = info->service_name;
            LcClientPool *pool = const_cast<LcClientPool*>(lcmgr->getServiceByName(service_name.c_str()));
            if (NULL == pool) {
                return bsl::var::Null::null;
            }

            bsl::var::IVar &output = rp.create<bsl::var::Dict>();
            
            if (strcmp(query["Key"].c_str(), "CONNECT_POOL_USAGE") == 0) {
                output = pool->getPoolUsage(rp);  
            } else if (strcmp(query["Key"].c_str(), "CONNECT_POOL_CURENT") == 0) {
                output = pool->getPoolUsingNum(rp);  
            } else if (strcmp(query["Key"].c_str(), "CONNECT_POOL_MAX") == 0) {
                output = pool->getPoolMaxNum(rp);  
            } else {
                return bsl::var::Null::null;
            }
            return output;
        } catch (bsl::Exception &e) {
            LC_LOG_WARNING("catch bsl exception:%s", e.all());
            return bsl::var::Null::null;
        } catch (std::exception &e) {
            LC_LOG_WARNING("catch stl exception:%s", e.what());
            return bsl::var::Null::null;
        } catch (...) {
            LC_LOG_WARNING("catch unknown exception");
            return bsl::var::Null::null;
        }
    }

    

    int LcClientManager :: add_monitor_info_ipport(const char *monitor_item, const char *ip, int
port, const char *service_name)
    {
        if (_monitor) {
            char sip[32];
            snprintf(sip, sizeof(sip), "%s", ip);
            lc_client_convert_ip(sip);
            char ip_port[32];
            snprintf(ip_port, sizeof(ip_port), "%s_%d", ip, port);
            lc_client_convert_ip(ip_port);
            bsl::string path = service_name;
            path.append(".").append(monitor_item);
            _monitor->add_path(path.append(".").append(ip_port).append(".").append(sip).c_str(), 1);
        }
        return 0;
    }

    int LcClientManager :: add_monitor_info_ipport(const char *monitor_item, int fd, const char *service_name)
    {
        return add_monitor_info_ipport_ex(monitor_item, fd, service_name, 1);
    }

    int LcClientManager :: add_monitor_info_ipport_ex(const char *monitor_item, int fd, const char *service_name, int num)
    {
        if (_monitor) {
            struct sockaddr_in addr;
            socklen_t addr_len = sizeof(addr);
            getpeername(fd, (struct sockaddr*)&addr, &addr_len);
            bsl::string path = service_name;
            path.append(".").append(monitor_item);
            char ip[32];
            snprintf(ip, sizeof(ip), "%s", inet_ntoa(addr.sin_addr));
            lc_client_convert_ip(ip);
            char ip_port[32];
            snprintf(ip_port, sizeof(ip_port), "%s_%d", ip, ntohs(addr.sin_port));
            lc_client_convert_ip(ip_port);
            _monitor->add_path(path.append(".").append(ip_port).append(".").append(ip).c_str(), num);
        }
        return 0;
    }

    bsl::var::IVar & LcClientManager :: get_note_var()
    {
        return _monitor_info;
    }

    int LcClientManager :: gen_monitor_info_note()
    {
        
        if (NULL == _monitor) {
            return 0;
        }

        bsl::var::IVar &note = get_note_var();

        
        int service_num = getServiceCount();
        for (int i=0; i<service_num; i++) {
            LcClientPool *pool = const_cast<LcClientPool *>(getServiceByID(i));
            if (NULL == pool) {
                return -1;
            }
            LcClientConnectManager *mgr = pool->getManager();
            if (NULL == mgr) {
                return -1;
            }

            bsl::string service_name = mgr->getServiceName();
            note[service_name] = _monitor_rp.create<bsl::var::Dict>();

            bsl::var::IVar &note_service = note[service_name];

            gen_monitor_service_info(note_service, mgr, "QUERY_CONNECT_NUMBER",
                    "向该IP发出的请求数");
            gen_monitor_service_info(note_service, mgr, "READ_ERROR_NUMBER",
                    "与该IP通信的读错误");
            gen_monitor_service_info(note_service, mgr, "WRITE_ERROR_NUMBER",
                    "与该IP通信的写错误");
            gen_monitor_service_info(note_service, mgr, "CONNECT_TIME",
                    "向该IP连接的平均耗时");
            gen_monitor_service_info_ex(note_service, mgr, "CONNECT_POOL_USAGE",
                    "连接池使用率");
            gen_monitor_service_info_ex(note_service, mgr, "CONNECT_POOL_CURENT",
                    "连接池使用数");
            gen_monitor_service_info_ex(note_service, mgr, "CONNECT_POOL_MAX",
                    "连接池上限");
        }
        return 0;
    }
    
    int LcClientManager :: gen_monitor_service_info_ex(bsl::var::IVar &service_note,
                                                       LcClientConnectManager *mgr,
                                                       const char *item,
                                                       const char *note)
    {
        service_note[item] = _monitor_rp.create<bsl::var::Dict>();
        bsl::var::IVar &item_note = service_note[item];

        int server_num = mgr->getServerSize();
        for (int j=0; j<server_num; j++) {
            LcClientServer *server = dynamic_cast<LcClientServer*>(mgr->getServerInfo(j));
            if (NULL == server) {
                return -1;
            }
            char server_name[32];
            snprintf(server_name, sizeof(server_name), "%s_%d", server->getIP(),
                    server->getPort());
            lc_client_convert_ip(server_name);
            item_note[server_name] = _monitor_rp.create<bsl::var::String>(note);
        }

        return 0;
    }

    int LcClientManager :: gen_monitor_service_info(bsl::var::IVar &service_note,
                                                    LcClientConnectManager *mgr,
                                                    const char *item,
                                                    const char *note)
    {
        service_note[item] = _monitor_rp.create<bsl::var::Dict>();
        bsl::var::IVar &item_note = service_note[item];

        int server_num = mgr->getServerSize();
        for (int j=0; j<server_num; j++) {
            LcClientServer *server = dynamic_cast<LcClientServer*>(mgr->getServerInfo(j));
            if (NULL == server) {
                return -1;
            }
            char server_name[32];
            snprintf(server_name, sizeof(server_name), "%s_%d", server->getIP(),
                    server->getPort());
            lc_client_convert_ip(server_name);
            item_note[server_name] = _monitor_rp.create<bsl::var::Dict>();
            bsl::var::IVar &server_note = item_note[server_name];

            int ip_num = server->getIPCount();
            for (int k=0; k<ip_num; k++) {
                server_note[server->getIP(k)] = _monitor_rp.create<bsl::var::String>(note);
            }
        }
        return 0;
    }

    void lc_client_convert_ip(char *ip)
    {
        char *split = strstr(ip, ".");
        while (split != NULL) {
            *split = '_';
            split = strstr(ip, ".");
        }

    }

    int LcClientManager :: common_singletalk_ip(LcEvent *event, char *ip, int port, unsigned int ms_timeout)
    {
        if (NULL == event || NULL == ip || port <= 0) {
            return -1;
        }
        
        int fd = ul_tcpconnecto_ms(ip, port, ms_timeout);
        if (fd < 0) {
            LC_LOG_WARNING("conect %s:%d error [%m]", ip, port);
            return -1;
        }
        
        event->setHandle(fd);
		event->setTimeout(ms_timeout);
        int ret = common_singletalk_ex(event);
        ::close(fd);
        event->setHandle(-1);
        return ret;
    }

    int LcClientManager :: common_nonblock_singletalk_ip(LcEvent *event, char *ip, int port, unsigned int ms_timeout, LcClientEventPool *pool)
    {
        if (NULL == event || NULL == ip || port <= 0) {
            return -1;
        }
        
        int fd = ul_tcpconnecto_ms(ip, port, ms_timeout);
        if (fd < 0) {
            LC_LOG_WARNING("conect %s:%d error [%m]", ip, port);
            return -1;
        }
        
        event->setHandle(fd);
		event->setTimeout(ms_timeout);
        return common_nonblock_singletalk_ex(event, pool);
    }

    void LcClientManager :: lc_client_callback_ip(lc::IEvent *e, void *param)
    {
        if (NULL == e || NULL == param) {
            return;
        }

        lc::LcEvent *sock_event = dynamic_cast<lc::LcEvent *>(e);
        if (NULL == sock_event) {
            return;
        }
        common_talkwith_t *talkmsg = (common_talkwith_t*)param;
        sock_event->setCallback(talkmsg->user_callback, talkmsg->user_arg);

        if (sock_event->isError()) {
            LC_LOG_WARNING("connect error happened, logid:%d", sock_event->get_logid());
            sock_event->callback();
            return;
        }

        if (talkmsg->mgr) {
            if (talkmsg->mgr->common_async_talk_ex(sock_event) != 0) {
                LC_LOG_WARNING("common_async_talk_ex error, logid:%d", sock_event->get_logid());
                sock_event->setResult(lc::IEvent::ERROR);
                sock_event->callback();
            }
            return;
        } else {
            sock_event->setResult(lc::IEvent::ERROR);
            sock_event->callback();
        }
    }

    int LcClientManager :: common_async_talk_ip(LcEvent *event, char *ip, int port, unsigned int ms_timeout)
    {
        
        
        if (NULL == event || NULL == ip || port <= 0) {
            return -1;
        }

        event->setResult(0);
        event->set_sock_status(0);
        common_talkwith_t *talkmsg = (common_talkwith_t*)(event->get_tag());
        if (NULL == talkmsg) {
            LC_LOG_WARNING("set talkwith is set_tag");
            return -1;
        }

        event->getCallback(&talkmsg->user_callback, &talkmsg->user_arg);
        
        
        if (event->tcpConnect(ip, port) != 0) {
            LC_LOG_WARNING("tcp connect %s:%d error [%m]", ip, port);
            return -1;
        }

        event->setTimeout(ms_timeout);
        event->setCallback(lc_client_callback_ip, talkmsg);
        talkmsg->mgr = this;

        if (_netreactor->post(event) != 0) {
            LC_LOG_WARNING("post event erorr [%m]");
            event->setCallback(talkmsg->user_callback, talkmsg->user_arg);
            return -1;
        }

        return 0;
    }
} 


