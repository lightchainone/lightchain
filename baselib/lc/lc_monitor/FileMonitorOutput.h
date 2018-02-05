


#ifndef  __FILEMONITOROUTPUT_H_
#define  __FILEMONITOROUTPUT_H_

#include "MonitorOutput.h"
#include <ul_conf.h>
#include <bsl/var/IVar.h>
#include <bsl/ResourcePool.h>
#include "lc_log.h"

namespace lc
{


class FileMonitorOutput : plclic MonitorOutput {
plclic:

    
    FileMonitorOutput(LcMonitor *monitor):MonitorOutput(monitor),
        _pid(0), _path("./info"), _name("file.out"), _run(1), _filefd(-1),
        _single_monitor_size(1024000), _tmp_buf(NULL), _last_size(0)
    {
    }

    
    virtual ~FileMonitorOutput(){}

    
    virtual int init(ul_confdata_t *conf);

    
    virtual int run();

    
    virtual int stop();

    
    virtual int join();
protected:
    
    
    int convert_var2json(bsl::var::IVar &info, bsl::AutoBuffer &buffer);

    
    static void *work_thread(void *arg);

    
    void do_work();

    
    char *self_fprintf(char *buffer, size_t &buf_size, const char *input);

    
    int merge_var(bsl::var::IVar &output, bsl::var::IVar &input, bsl::ResourcePool &rp);

    int self_openfile() {
		bsl::string path = _path;
		int fd = open((path.append("/").append(_name)).c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0744);
		if (fd < 0) {
			LC_LOG_WARNING("open file for FileMonitorOutput error[%m]");
			
			
			mkdir(_path.c_str(), 0755);
			fd = open(path.c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0744);
			if (fd < 0) {
				LC_LOG_WARNING("open file for FileMonitorOutput error after mkdir %s [%m]", _path.c_str());
				return -1;
			}
		}
		_filefd = fd;
        return 0;
    }

    void self_close() {
        if (_filefd < 0) return;
        close(_filefd);
        _filefd = -1;
    }

    int self_write(const char *buf, int len) {
        if (!buf || len <= 0) {
            LC_LOG_WARNING("write buf to file error, buf is NULL or len <= 0");
            return -1;
        }
        int ret = 0;
		if ((ret=write(_filefd, buf, len)) < len) {
			LC_LOG_WARNING("write len:%d to mointor_file error, real write:%d %m", len, ret);
			self_close();
            _last_size = _last_size>ret?_last_size:ret;
            return -1;
		}
        return 0;
    }
protected:
    bsl::ResourcePool _rp;		  
    pthread_t _pid;		          
    bsl::string _path;		      
    bsl::string _name;		      
    int _run;		              
    int _filefd;		          
    int _single_monitor_size;     
    char *_tmp_buf;
    int _last_size;
};

}















#endif  


