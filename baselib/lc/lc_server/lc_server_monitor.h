


#ifndef  __LC_SERVER_MONITOR_H_
#define  __LC_SERVER_MONITOR_H_

#include "lc_server.h"

#include <Lsc/var/IVar.h>
#include <Lsc/ResourcePool.h>


int lc_server_set_monitor(lc_server_t *sev, lc::LcMonitor *monitor);

Lsc::var::IVar &lc_server_monitor_get_queue_size(Lsc::var::IVar &query, Lsc::ResourcePool &rp);

Lsc::var::IVar &lc_server_monitor_get_thread_pool_usage(Lsc::var::IVar &query, Lsc::ResourcePool &rp);

int lc_server_register_monitor_callback(lc_server_t *sev);

Lsc::var::IVar &get_monitor_list(Lsc::ResourcePool &rp);
















#endif  


