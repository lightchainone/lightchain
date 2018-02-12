
#include <ul_conf.h>

#include "comlog.h"
#include "comsig.h"
#include "xqueue.h"
#include "logstat.h"
#include "xutils.h"
#include "xdebug.h"
#include "appender/appender.h"
#include "appender/appenderfactory.h"

namespace comspace
{

struct reg_node
{
	LogStat *logstat;
	cb_loadconf loadconf;
	char path[COM_MAXHOSTNAME];
	char file[COM_MAXFILENAME];
};

static const int ARRAYSIZ = 1024;
static reg_node g_array[ARRAYSIZ];
static int g_arysiz = 0;
static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;

static int _def_loadconf_(com_logstat_t *lg, const char *path, const char *file);
extern u_int comlog_get_lower_syslevel(int sysevents);

int reg_logstat(LogStat *ls, cb_loadconf lc, const char *path, const char *file)
{
	xAutoLock lock(&g_lock);

	if (g_arysiz >= ARRAYSIZ) return -1;

	reg_node &node = g_array[g_arysiz];
	node.logstat = ls;
	node.loadconf = lc;
	snprintf(node.path, sizeof(node.path), "%s", path);
	snprintf(node.file, sizeof(node.file), "%s", file);

	++ g_arysiz;
	return 0;
}

int reg_logstat(LogStat *ls, const char *path, const char *file)
{
	return reg_logstat(ls, _def_loadconf_, path, file);
}

int reg_deal()
{
	xAutoLock lock(&g_lock);

	for (int i = 0; i < g_arysiz; ++i) {
		com_logstat_t lg;
		int ret = g_array[i].loadconf(&lg, g_array[i].path, g_array[i].file);
		if (ret != -1) {
			u_int syslvl = comlog_get_lower_syslevel(lg.sysevents);
			g_array[i].logstat->setLevel(syslvl, lg.userevents);
		}
	}
	return 0;
}

static int _def_loadconf_(com_logstat_t *lg, const char *path, const char *file)
{
	if (lg == NULL || path == NULL || file == NULL) {
		_com_error("invalid param");
		return -1;
	}
	Ul_confdata *ptr = ul_initconf(MAX_CONF_ITEM_NUM-1);
	if (ptr == NULL) {
		_com_error("init conf err");
		return -1;
	}

	int ret = ul_readconf(path, file, ptr);
	if (ret != 1) {
		_com_error("read conf err");
		return -1;
	}

	char flag[256];
	char name[256];
	char val[1024*2];

	com_logstat_t logstat;
	memset(&logstat, 0, sizeof(logstat));

	snprintf(flag, sizeof(flag), "%s", COMLOG_LEVEL);
	ret = ul_getconfint(ptr, flag, &logstat.sysevents);
	if (ret != 1) {
		logstat.sysevents = 16;
	}
	//转载自定义等级
	snprintf(flag, sizeof(flag), "%s", COMLOG_SELFDEFINE);
	ret = ul_getconfnstr(ptr, flag, logstat.userevents, sizeof(logstat.userevents)-1);
	logstat.userevents[sizeof(logstat.userevents)-1] = 0;
	if (ret != 1) {
		logstat.userevents[0] = 0;
	}

	//
	int num = 0;
	int tmp = 0;
	Appender *app = NULL;

	snprintf(flag, sizeof(flag), "%s", COMLOG_DEVICE_NUM);
	ret = ul_getconfint(ptr, flag, &num);
	
	if (num > 0) {
		com_device_t device;

		for (int i=0; i<num; ++i) {
			snprintf(flag, sizeof(flag), "%s%d", COMLOG_DEVICE, i);
			ret = ul_getconfnstr(ptr, flag, name, sizeof(name)-1);
			if (ret != 1) {
				_com_error("can't get flag[%s]", flag);
				ret = -1;
				goto end;
			}

			//type
			snprintf(flag, sizeof(flag), "%s_%s", name, COMLOG_DEVICE_TYPE);
			ret = ul_getconfnstr(ptr, flag, val, sizeof(val)-1);
			if (ret != 1) {
				_com_error("can't get flag[%s]", flag);
				ret = -1;
				goto end;
			}
			if(AppenderFactory::isAppender(val)){
				strcpy(device.type, val);
			}
			else{
				_com_error("Unkown device type[%s] at flag[%s]", val, flag);
				ret = -1;
				goto end;
			}
#if 0
			if (strcmp(val, COMLOG_DEVICE_FILE) == 0) {
				device.type = COM_FILEDEVICE;
			} else if (strcmp(val, COMLOG_DEVICE_NET) == 0) {
				device.type = COM_NETDEVICE;
			} else if (strcmp(val, COMLOG_DEVICE_TTY) == 0) {
				device.type = COM_TTYDEVICE;
			} else if (strcmp(val, COMLOG_DEVICE_ULLOG) == 0) {
				device.type = COM_ULLOGDEVICE;
			} else if (strcmp(val, COMLOG_DEVICE_AFILE) == 0) {
				device.type = COM_AFILEDEVICE;
			} else {
				_com_error("invalid conf[%s] at flag[%s]", val, flag);
				ret = -1;
				goto end;
			}
#endif
			//path
			snprintf(flag, sizeof(flag), "%s_%s", name, COMLOG_DEVICE_PATH);
			ret = ul_getconfnstr(ptr, flag, device.host, sizeof(device.host)-1);
			if (ret != 1 && strcmp(device.type, "TTY") != 0) {
				_com_error("can't get flag[%s]", flag);
				ret = -1;
				goto end;
			}
			//name
			snprintf(flag, sizeof(flag), "%s_%s", name, COMLOG_DEVICE_NAME);
			ret = ul_getconfnstr(ptr, flag, device.file, sizeof(device.file)-1);
			if (ret != 1 && strcmp(device.type, "TTY") != 0) {
				_com_error("can't get flag[%s]", flag);
				ret = -1;
				goto end;
			}
			

			//open
			snprintf(flag, sizeof(flag), "%s_%s", name, COMLOG_DEVICE_OPEN);
			ret = ul_getconfint(ptr, flag, &tmp);
			if (ret == 1) {
				device.open = tmp;
			} else {
				device.open = 1;//默认打开
			}

#if 0
			//是否支持系统日志
			snprintf(flag, sizeof(flag), "%s_%s", name, COMLOG_DEVICE_SYSLEVEL);
			ret = ul_getconfint(ptr, flag, &tmp);
			if (ret == 1 && tmp < 32) {
				device.log_mask = 0;
				for (int j=1; j<=tmp; ++j) {
					device.log_mask |= (1<<j);
				}
			}

			//支持的自定义日志等级
			snprintf(flag, sizeof(flag), "%s_%s", name, COMLOG_DEVICE_SELFLEVEL);
			ret = ul_getconfnstr(ptr, flag, val, sizeof(val)-1);
			if (ret == 1) {
				char res[COM_SELFLEVELNUM][COM_MAXLEVELNAMESIZE];
				int items = comspace::xparserSelfLog(val, res, sizeof(res)/sizeof(res[0]));
				_com_debug("get selflog items[%d] aipos[%d]", items, i);
				for (int j=0; j<items; ++j) {
					//u_int id = comspace::getLogId(res[j]);
					u_int id = comspace::createSelfLog(res[j]);
					_com_debug("selflog[%d][%s]", id, res[j]);
					if (id >= COM_USRLEVELSTART && id < COM_LEVELNUM) {
						COMLOG_ADDMASK(device, id);
						_com_debug("now self[%llx] add id[%d]", device.log_mask, id);
					}
				}
			}
#endif
			app = AppenderFactory::tryAppender(device);
			if (app) {
				app->_open = device.open;
			}
		}
		ret = 0;
	} else {
		ret = -1;
	}
end:
	ul_freeconf(ptr);
	*lg = logstat;
	return ret;
}


};

/* vim: set ts=4 sw=4 sts=4 tw=100 */
