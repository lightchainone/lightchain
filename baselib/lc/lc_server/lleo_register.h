#ifndef __GALILEO_REGISTER_H__
#define __GALILEO_REGISTER_H__

#include <list>

class GalileoCluster;

class GalileoRegister 
{
plclic:
	GalileoRegister(const char *svrlist, const char *path, const char *resdata);	
	~GalileoRegister();

	int register_resource();
private:
	std::string     m_svrlist;
	std::string     m_respath;
	std::string     m_resdata;

	GalileoCluster* m_cluster;
};

#endif

