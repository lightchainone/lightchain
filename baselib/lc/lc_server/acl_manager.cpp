#include <stdio.h>

#include "Lsc/var/Array.h"
#include "Lsc/var/Dict.h"
#include "Lsc/ResourcePool.h"
#include "Lsc/AutoBuffer.h"
#include "Lsc/var/JsonSerializer.h"

#include "lc_log.h"
#include "lc_auth.h"
#include "lc_server.h"

#include "../lc_lleo/lleo_cluster.h"
#include "../lc_lleo/lleo_manager.h"

#include "acl_manager.h"




static int acl_event_callback(int watch_type, const char *realpath, Lsc::var::IVar& res_data, void *cb_ctx)
{
	AclManager *acl = (AclManager*)cb_ctx;
	
	switch(watch_type) {
		case GALILEO_ABSTRACT_CHANGED_EVENT:
			
			acl->handle_lleo_acl_result(res_data, true);	
			break;
		case GALILEO_ENTITY_CHANGED_EVENT:
			
			break;
		case GALILEO_RESOURCE_UPDATE_EVENT:
			
			break;
		default:
			break;
	}
	return 0;
}


int AclManager::handle_lleo_acl_result(Lsc::var::IVar &res_data, bool from_callback)
{
	Lsc::var::IVar* acl_result = lleo_get_acl_from_data(res_data);
	
	if(! acl_result->is_array()) {
		LC_LOG_WARNING("lleo_get_acl_from_data() result is (%s), not array", acl_result->get_type().c_str());
		return -1;
	}

	Lsc::var::JsonSerializer js;
	Lsc::AutoBuffer buf;
	js.serialize(*acl_result, buf);
	if(from_callback) {
		if(m_last_snapshot == buf.c_str()) {
			LC_LOG_DEBUG("Same acl result as before, no need update");
			return 0;
		}
	}
	
	m_last_snapshot = buf.c_str();

	lc_auth_t *old_auth = lc_server_get_ip_auth(m_lc_server);
	lc_auth_delref(old_auth);

	lc_auth_t *new_auth;
	if(from_callback) {
		new_auth = lc_auth_create();

		
		std::list<std::string>::iterator idx;
		for(idx = m_local_acl_list.begin(); idx != m_local_acl_list.end(); idx++) {
			lc_auth_push_ip(new_auth, (*idx).c_str());
		}
	} else
		new_auth = old_auth;

	int size = acl_result->size();
	int ret;

	for(int index = 0; index < size; index++) {
		Lsc::var::IVar& value = acl_result->get(index);
		try {
			const char *acl_ip  = value["acl_ip"].c_str();
			
			
			
		
			LC_LOG_DEBUG("acl_ip: (%s)", acl_ip); 
			
			ret = lc_auth_push_ip(new_auth, acl_ip);
		} catch (...) {
			continue;
		}
	}
	lc_auth_compile(new_auth);

	if(from_callback) {
		
		lc_server_set_ip_auth(m_lc_server, new_auth);
		
		if(old_auth) {
			
			int count = 0;
			while(1) {
				if(lc_auth_refcnt(old_auth) > 0) {
					sleep(1);
					count++;
					if(count <= 10)
						continue;
					
					LC_LOG_WARNING("destroy old lc_auth forcelly!!!");
				}
				LC_LOG_DEBUG("destroy old lc_auth");
				lc_auth_destroy(old_auth);
				break;
			}
		}
	}
	return 0;
}


AclManager::AclManager(lc_server_t *server)	
{
	m_cluster = NULL;
	m_lc_server = server;
}

AclManager::~AclManager()	
{
	if(m_cluster) {
		GalileoManager::get_instance()->release_cluster(m_cluster);
		m_cluster = NULL;
	}	
}

static void add_local_ip(const char *ip, void *data)
{
	std::list<std::string>* local_acl_list = (std::list<std::string> *)data;
	local_acl_list->push_back(ip);
}




int AclManager::handle_acl(lc_svr_t *svrconf) 
{
	lc_auth_t *auth = NULL;

	
	if (svrconf->auth[0] != 0) {
		auth = lc_auth_create();
		
		if(lc_auth_load_ip_ex(auth, svrconf->auth, add_local_ip, (void*)&m_local_acl_list) != 0) {
			LC_LOG_WARNING("lc_auth_load_ip() failed, (%s)", svrconf->auth);
			lc_auth_destroy(auth);
			auth = NULL;
			
		} else {
			lc_auth_compile(auth);
			m_lc_server->ip_auth_cry = 1;
			lc_server_set_ip_auth(m_lc_server, auth);
		}
	}

	
	if(svrconf->lleo_acl_enable == 0)
		return 0;

	
	if(auth == NULL) {
		auth = lc_auth_create();
		lc_server_set_ip_auth(m_lc_server, auth);
	}

	
	if(svrconf->lleo_svrlist[0] == 0 || svrconf->lleo_path[0] == 0) {
		LC_LOG_WARNING("lleoo_svrlist or lleo_path is null.");
		return 0;
	}

	LC_LOG_DEBUG("lleo_svrlist (%s), lleo_path (%s)", 
			svrconf->lleo_svrlist, 
			svrconf->lleo_path);

	
	m_cluster = GalileoManager::get_instance()->get_cluster(svrconf->lleo_svrlist, true);
	if(m_cluster == NULL) {
		return 0;
	}

	
	Lsc::ResourcePool rp;
	Lsc::var::IVar &res_data = rp.create<Lsc::var::Dict>();
	int ret = m_cluster->query_resource(svrconf->lleo_path, res_data, rp);
	if(ret != 0) {
		LC_LOG_WARNING("query_resource (%s) failed, (%d)", svrconf->lleo_path, ret);
		return 0;
	}

	
	handle_lleo_acl_result(res_data.get(GALILEO_ABSTRACT), false);

	
	ret = m_cluster->watch_resource(svrconf->lleo_path, acl_event_callback, this);
	if(ret != 0) {
		return 0;
	}
	return 0;
}


