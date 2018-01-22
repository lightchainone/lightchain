#ifndef __ACL_MANAGER_HPP__
#define __ACL_MANAGER_HPP__

#include <list>
#include <string>

#include "lcserver_core.h"
#include "lc_conf.h"

class GalileoCluster;


class AclManager
{
plclic:
	AclManager(lc_server_t *server); 
	~AclManager();

	
	int handle_acl(lc_svr_t*);
	
plclic:
	
	int handle_lleo_acl_result(Lsc::var::IVar &res_data, bool from_callback);
	
private:
	GalileoCluster* m_cluster;
	lc_server_t*    m_lc_server;

	
	std::list<std::string> m_local_acl_list;
	
	std::string     m_last_snapshot;

};

#endif

