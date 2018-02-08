
#include "lcfw.h"
#include <map>
#include <signal.h>
#include "lcserver_core.h"
#include <vector>
#include "CmdOption.h"
#include "LcMonitor.h"
#include "../lc_galileo/galileo_manager.h"

const char *lcmonitor_info = "\n\n\
#监控库 监控项平均值计算周期 \n\
lcmonitor_compute_time_ms : 50000 \n\
#监控库 监控项的监控信息输出周期 \n\
lcmonitor_monitor_time_ms : 600000 \n\
#监控库 监控信息输出设备 \n\
lcmonitor_output_num : 2 \n\
#socket输出 \n\
lcmonitor_device0 : SOCKET \n\
#监听端口 其余配置项同lc_server\n\
lcmonitor_socket_port : 9094 \n\
lcmonitor_socket_name : monitor\n\
lcmonitor_socket_servertype : 0 \n\
lcmonitor_socket_threadnum : 5 \n\
lcmonitor_socket_read_bufsize : 1048576 \n\
lcmonitor_socket_write_bufsize : 1048576 \n\
lcmonitor_socket_connecttimeout : 500 \n\
lcmonitor_socket_readtimeout : 500 \n\
lcmonitor_socket_writetimeout : 500 \n\
lcmonitor_socket_connecttype : 0 \n\
lcmonitor_socket_quenesize : 100 \n\
lcmonitor_socket_socksize : 500 \n\
#lcmonitor_socket_auth : \n\
#文件输出 \n\
lcmonitor_device1 : FILE \n\
#文件输出目录 \n\
lcmonitor_file_path : ./info \n\
#文件输出文件 \n\
lcmonitor_file_name : file.out \n\
#单条监控信息最大长度 \n\
lcmonitor_single_monitor_size : 1024000 \n\
\n";

void lc_safe_exit()
{
	for (;;) {

		raise(SIGKILL);
	}
}


static void
print_help()
{
    printf("%s", "\t-d:     conf_dir\n");
    printf("%s", "\t-f:     conf_file\n");
    printf("%s", "\t-h:     show this help page\n");
    printf("%s", "\t-t:     check config file syntax\n");
    printf("%s", "\t-g:     generate config sample file\n");
    printf("%s", "\t-v:     show version infomation\n");
    return;
}





lc_t *lc_init(const char *name, int argc, char **argv, 
		void (*ver)(), void (*help)())
{
	if (name == NULL || argv == NULL) {
		return NULL;
	}
	lc_t *fw = NULL;
	try {
		fw = new lc_t;
	} catch (...) {
		return NULL;
	}
	int len = strlen(argv[0]);
	while (len > 0 && argv[0][len-1] != '/') {
		-- len;
	}
	char *rn = argv[0];
	if (len > 0) {
		rn = argv[0] + len;
	}
	snprintf(fw->file, sizeof(fw->name), "%s.conf", rn);
	snprintf(fw->path, sizeof(fw->name), "%s", "./conf/");
	snprintf(fw->name, sizeof(fw->name), "%s", name);
	fw->conf_build = 0;
	fw->conf_check = 0;
	fw->conf = 0;

	comcfg::CmdOption opt;
	if (opt.init(argc, argv, "d:f:gthv") != 0) {
		return NULL;
	}
	if (opt.hasOption("d")) {
		snprintf(fw->path, LC_PATHSIZE, "%s/", opt["d"].to_cstr());
	}
	if (opt.hasOption("f")) {
         snprintf(fw->file, LC_PATHSIZE, "%s", opt["f"].to_cstr());
	}
	if (opt.hasOption("v")) {
		if (ver) {
			ver();
		}
		return NULL;
	}
	if (opt.hasOption("h")) {
		if (help) {
			help();
		} else {
			print_help();
		}
		return NULL;
	}
	if (opt.hasOption("g")) {
		fw->conf_build = 1;
	}
	if (opt.hasOption("t")) {
		fw->conf_check = 1;
	}
	signal(SIGPIPE, SIG_IGN);
	return fw;
};

void lc_close(lc_t *fw)
{
	if (fw) {
		if (fw->conf) {
			lc_close_conf(fw);
		}
		delete fw;
	}
}

lc_conf_data_t *lc_open_conf(lc_t *fw, const int version)
{
	if (fw) {
		if (fw->conf) {
			return fw->conf;
		}
		fw->conf = lc_conf_init(fw->path, fw->file, fw->conf_build, LC_CONF_DEFAULT_ITEMNUM, version);
		return fw->conf;
	}
	return NULL;
}

void lc_close_conf(lc_t *fw)
{
	if (fw) {
		if (fw->conf) {
			lc_conf_close(fw->conf);
			fw->conf = 0;
		}
	}
}

lc_server_t *lc_load_svr(lc_t *fw, const char *name)
{
	if (fw == NULL || name == NULL) {
		return NULL;
	}
	lc_svr_t svrconf;
	if (lc_conf_getsvr_ex(fw->conf, fw->name, name, &svrconf, "") != LC_CONF_SUCCESS) {
		return NULL;
	}
	if (fw->conf_build) {
		return NULL;
	}

	lc_server_t *svr = lc_server_create(name);
	if (svr == NULL) {
		return svr;
	}
	lc_server_load_ex(svr, &svrconf);
	return svr;
}


lc_client_t *lc_load_clt(lc_t *fw, const char *name, const char *cfg, lc_client_attr_t *attr)
{
	if (fw == NULL || name == NULL || cfg == NULL) {
		return NULL;
	}
	int cfglen = strlen(cfg) + 10;
	if (cfglen < 256) {
		cfglen = 256;
	}
	char * svrn = (char *)malloc(cfglen);
	if (svrn == NULL) {
		return NULL;
	}

	lc_client_t *clt = NULL;
	if (!fw->conf_build) {
		clt = lc_client_init(attr);
		if (clt == NULL) {
			free (svrn);
			return NULL;
		}
	}

	memset(svrn, 0, cfglen);

	const char *ptr = cfg;
	lc_request_svr_t conf;
	while (sscanf(ptr, "%s", svrn) == 1) {
		ptr += strlen(svrn);
		while (*ptr && (*ptr == ' ' || *ptr == '\t')) {
			++ptr;
		}
		conf.no_magic_checker = 0;
		if (lc_conf_getreqsvr_ex(fw->conf, name, svrn, &conf, "") == LC_CONF_SUCCESS) {
			if (!fw->conf_build) {
				lc_client_mod_attr_t at;
				at.nomagiccheck = conf.no_magic_checker;
				lc_client_add(clt, &conf, svrn, &at);
			}
		}
		LC_LOG_DEBUG("scan %s module in client, left %s", svrn, ptr);
	}

	free (svrn);
	return clt;
}

int lc_load_log(lc_t *fw)
{
	if (fw == NULL) {
		return -1;
	}
	if (fw->conf_build) {


		char str[256] = "";
		u_int val;
		u_int def = 2;
		lc_conf_getnstr(fw->conf, "COMLOG_PROCNAME", str, sizeof(str), "进程名", fw->name);
		lc_conf_getuint(fw->conf, "COMLOG_DEVICE_NUM", &val, "设备数目", &def);
		lc_conf_getnstr(fw->conf, "COMLOG_DEVICE0", str, sizeof(str), "设备0 名", "FILE");
		lc_conf_getnstr(fw->conf, "COMLOG_DEVICE1", str, sizeof(str), "设备0 名", "TTY");
		def = 16;
		lc_conf_getuint(fw->conf, "COMLOG_LEVEL", &val, "日志等级", &def);

		lc_conf_getnstr(fw->conf, "FILE_TYPE", str, sizeof(str), "设备类型, ULLOG", "ULLOG");
		char log[256];
		snprintf(log, sizeof(log), "%s.log", fw->name);
		lc_conf_getnstr(fw->conf, "FILE_NAME", str, sizeof(str), "日志名", log);
		lc_conf_getnstr(fw->conf, "FILE_PATH", str, sizeof(str), "日志路径", "./log");
		def = 1;
		lc_conf_getuint(fw->conf, "FILE_OPEN", &val, "是否打开这个设备", &def);


		lc_conf_getnstr(fw->conf, "TTY_TYPE", str, sizeof(str), "设备类型, TTY", "TTY");
		lc_conf_getnstr(fw->conf, "TTY_NAME", str, sizeof(str), "日志名", fw->name);
		lc_conf_getnstr(fw->conf, "TTY_PATH", str, sizeof(str), "日志路径", "./log");
		def = 1;
		lc_conf_getuint(fw->conf, "TTY_OPEN", &val, "是否打开这个设备", &def);


	} else {
		return lc_load_comlog(fw->path, fw->file);
	}
	return 0;
}

void lc_builddone_exit(lc_t *fw)
{
	if (fw == NULL) {
		return;
	}
	if (fw->conf_build) {	
		ul_writelog(UL_LOG_NOTICE, "gen conf to [%s/%s.sample] success",
				fw->path, fw->file);
		lc_close(fw);
		lc_safe_exit();
	}
	ul_writelog(UL_LOG_NOTICE, "load configure done...");
}

struct lc_svr_vec_t
{
	std::vector<lc_server_t *> vec;
};
lc_svr_vec_t * lc_svr_vec_create()
{
	try {
		return new lc_svr_vec_t;
	} catch (...) {
		LC_LOG_FATAL("memory is not enough");
		lc_safe_exit();
	}
	return NULL;
}
int lc_svr_vec_add(lc_svr_vec_t *vec, lc_server_t *svr, lc_t *fw)
{
	if (vec == NULL) {
		return -1;
	}
	try {
		if (fw==NULL || fw->conf_build == 0) {
			vec->vec.push_back(svr);
		}
	} catch (...) {
		LC_LOG_FATAL("memory is not enough");
		return -1;
	}
	return 0;
}
int lc_svr_vec_run(lc_svr_vec_t *vec)
{
	if (vec == NULL) {
		return -1;
	}
	for (size_t i=0; i<vec->vec.size(); ++i) {
		lc_server_t *svr = vec->vec[i];
		if (svr) {
			if (lc_server_run(svr) != 0) {
				LC_LOG_FATAL("run %s server error %m", svr->server_name);
				lc_server_stop(svr);
				lc_server_join(svr);
				for (size_t r=0; r<i; ++r) {
					svr = vec->vec[r];
					if (svr) {
						LC_LOG_TRACE("stop %s server, because of run vectors error", 
								svr->server_name);
						lc_server_stop(svr);
						lc_server_join(svr);
					}
				}
				return -1;
			}
		}
	}
	return 0;
}
int lc_svr_vec_join(lc_svr_vec_t *vec)
{
	if (vec == NULL) {
		return -1;
	}
	for (size_t i=0; i<vec->vec.size(); ++i) {
		lc_server_join(vec->vec[i]);
	}
	return 0;
}
int lc_svr_vec_destroy(lc_svr_vec_t *vec)
{
	if (vec == NULL) {
		return 0;
	}
	for (size_t i=0; i<vec->vec.size(); ++i) {
		lc_server_destroy(vec->vec[i]);
	}
	delete vec;
	return 0;
}

int lc_svr_vec_stop(lc_svr_vec_t *vec)
{
    if (vec == NULL) {
        return -1;
    }

    for (size_t i=0; i<vec->vec.size(); ++i) {
        lc_server_stop(vec->vec[i]);
    }
    return 0;
}

lc::LcMonitor *lc_load_monitor(lc_t *fw)
{
    if (NULL == fw) {
        return NULL;
    }

    lc::LcMonitor *monitor = new (std::nothrow)lc::LcMonitor;
    if (NULL == monitor) {
        return NULL;
    }

    if (!fw->conf_build) {
        if (NULL != fw->conf) {
            if (monitor->init(fw->path, fw->file) != 0) {
                delete monitor;
                monitor = NULL;
                return NULL;
            }
            return monitor;
        }
        delete monitor;
        monitor = NULL;
        return NULL;
    }

    fprintf(fw->conf->conf_file, "%s", lcmonitor_info);
    return monitor;
}

int lcfw_open_galileo_log(lc_t *fw)
{
	char logpath[256];
	lc_conf_getnstr(fw->conf, "GALILEO_LOG_PATH",  logpath, sizeof(logpath), "Galileo log path", "./log/galileo.log");
	uint32_t loglevel;
	uint32_t default_loglevel = 4;
	lc_conf_getuint(fw->conf, "GALILEO_LOG_LEVEL", &loglevel, "Galileo log level", &default_loglevel);	
	return GalileoManager::open_log(logpath, loglevel);
}

void lcfw_close_galileo_log()
{
	GalileoManager::close_log();
}


