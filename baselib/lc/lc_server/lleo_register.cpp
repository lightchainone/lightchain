#include <stdio.h>

#include "lc_log.h"

#include "../lc_lleo/lleo_cluster.h"
#include "../lc_lleo/lleo_manager.h"
#include "lleo_register.h"

GalileoRegister::GalileoRegister(const char *svrlist, const char *path, const char *resdata)	
{
	m_cluster = NULL;

	m_svrlist = svrlist;
	m_respath = path;
	m_resdata = resdata;
}

GalileoRegister::~GalileoRegister()	
{
	if(m_cluster) {
		GalileoManager::get_instance()->release_cluster(m_cluster);
		m_cluster = NULL;
	}
}

int GalileoRegister::register_resource()
{
	m_cluster = GalileoManager::get_instance()->get_cluster(m_svrlist.c_str(), true);
	if(m_cluster == NULL) {
		return -1;
	}
	int tagno;
	int ret = m_cluster->register_resource(m_respath.c_str(), m_resdata.c_str(), &tagno);
	if(ret != 0) {
		LC_LOG_WARNING("register rsource failed, %d\n", ret);
		return -1;
	}
	return 0;
}


