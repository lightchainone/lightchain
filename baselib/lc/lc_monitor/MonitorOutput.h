


#ifndef  __MONITOROUTPUT_H_
#define  __MONITOROUTPUT_H_

#include <ul_conf.h>

namespace lc
{

class LcMonitor;

class MonitorOutput {

plclic:

    
    MonitorOutput(LcMonitor *monitor):
        _proc_name_is_set(false)
    {
        _monitor = monitor;
    }

    
    virtual ~MonitorOutput(){}

    
    virtual int init(ul_confdata_t * ) {
        return 0;
    }

    
    virtual int run() = 0;

    
    virtual int stop() = 0;

    
    virtual int join() = 0;

    
    void set_monitor(LcMonitor *monitor);

    
    void set_proc_path_name();

    
    const char *get_proc_name() {
        if (!_proc_name_is_set) {
            set_proc_path_name();
        }
        return _proc_name;
    }
protected:
    LcMonitor *_monitor;		  
    char _proc_name[1024];		  
    char _path_name[1024];		  
    bool _proc_name_is_set;		  
};


}















#endif  


