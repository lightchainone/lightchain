


#ifndef  __SOCKETMONITOROUTPUT_H_
#define  __SOCKETMONITOROUTPUT_H_

#include "MonitorOutput.h"
#include <ul_conf.h>
#include <bsl/var/IVar.h>
#include <bsl/ResourcePool.h>
#include "lc_auth.h"

typedef struct _lc_server_t lc_server_t;

namespace lc
{


class SocketMonitorOutput : plclic MonitorOutput {
plclic:

	int convert_var2mcpack_ex(const char *key, bsl::var::IVar &info, char *buffer, size_t buf_size, int all=0);
    
    SocketMonitorOutput(LcMonitor *monitor):
        MonitorOutput(monitor),
        _server(NULL),_auth(NULL)
    {
    }

    
    virtual ~SocketMonitorOutput();

    
    virtual int init(ul_confdata_t *conf);

    
    virtual int run();

    
    virtual int stop();

    
    virtual int join();
protected:
    
    int init_conf(ul_confdata_t *conf);

    
    static int query_server_callback();

    
    
    int parse_query();

    
    int process_query_cmd();

    
    int process_manage_cmd();

    
    
    int convert_var2mcpack(bsl::var::IVar &info, char *buffer, size_t buf_size, int all=0);

    
    int parse_manage_cmd();

    
    int parse_query_cmd();

    
    int merge_var(bsl::var::IVar &output, bsl::var::IVar &input, bsl::ResourcePool &rp); 
protected:
    lc_server_t * _server;		  
    lc_auth_t * _auth;		      
};

}















#endif  


