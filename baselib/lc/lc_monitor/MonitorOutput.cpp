
#include "MonitorOutput.h"

namespace lc
{
void MonitorOutput :: set_monitor(LcMonitor *monitor)
{
    _monitor = monitor;
}

void MonitorOutput :: set_proc_path_name()
{
    
    pid_t pid = getpid();
    
    char exec_file[1024];
    char proc_name[1024];
    snprintf(exec_file, sizeof(exec_file), "/proc/%d/exe", pid);
    int ret = readlink(exec_file, proc_name, sizeof(proc_name));
    proc_name[ret] = '\0';
    
    char *p = strrchr(proc_name, '/');
    snprintf(_proc_name, sizeof(_proc_name), "%s", p + 1);
    
    *p = '\0';
    snprintf(_path_name, sizeof(_path_name), "%s", proc_name);
    if (_path_name[0] == '\0') {
        _path_name[0] = '/';
        _path_name[1] = '\0';
    }
    _proc_name_is_set = true;
}

}




















