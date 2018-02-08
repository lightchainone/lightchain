
#include "lcfw2.h"

const char *lcclient2_top = "\n\n\n\
[LcClientConfig] \n\
#配置类型 \n\
#0:优先使用资源定位 \n\
#1:优先使用本地配置 \n\
ConfType  :  1 \n\
[.Reactor] \n\
ThreadNum  :  4 \n\
CheckTime : 1 \n\
[.LcClient] \n";

const char *lcclient2_galileo_top = "\n\
[LcClientConfig.Galileo] \n\
# 伽利略资源服务器地址 \n\
GALILEO_HOST  :  db-passport-test05.vm:2181 \n\
# 伽利略数据包大小，注意该数据包并不是单个数据pack大小，而是一次返回时最大可能 \n\
# 数据长度，单位byte，默认大小为128K \n\
GALILEO_DATA_PACK_SIZE  :  131072 \n\
# 存放Zookeeper客户端日志文件名 \n\
GALILEO_ZOO_LOG : ./log/zoo.pspui.log \n\
#资源定位配置本地的备份位置 \n\
DIR : ./conf \n\
FILE : galileo-back.conf \n\
# 依赖资源配置项，注意资源名必须全局唯一，注意机房配置 \n\
[..REQUEST_RESOURCE] \n";

const char *lcclient2_galileo = "\
[...@RES_ARR] \n\
NAME  :  %s \n\
ADDR  :  /lcclient_node20 \n";
const char *lcclient2_metaconf = "\n[..@Service] \n\
Name  :  %s \n\
ConnectAll :  0 \n\
DefaultConnectTimeOut  :  1000 \n\
DefaultReadTimeOut  :  1000 \n\
DefaultWriteTimeOut  :  1000 \n\
DefaultMaxConnect  :  10 \n\
DefaultRetry  :  5 \n\
#LONG / SHORT \n\
DefaultConnectType  :  SHORT \n\
DefaultLinger  :  0 \n\
#声明将要使用的策略类及属性 \n\
[...CurrStrategy] \n\
ClassName  :  LcClientStrategy \n\
[...CurrHealthy] \n\
ClassName  :  LcClientHealthyChecker  \n\
[...@Server] \n\
IP : 127.0.0.1 \n\
Port : 6667 \n";

lc::LcClientManager *lc_load_clt2(lc_t *fw, const char *name, const char *cfg, lc::LcMonitor *monitor, int threadnum)
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
    
    lc::LcClientManager *lcmgr = new (std::nothrow)lc::LcClientManager;
    if (NULL == lcmgr) {
        free(svrn);
        return NULL;
    }

    lcmgr->setMonitor(monitor, threadnum);

	if (!fw->conf_build) {
        if (NULL != fw->conf && NULL != fw->conf->conf_new) {
            comcfg::ConfigUnit &temp_conf = *(fw->conf->conf_new);
            if (temp_conf["LcClientConfig"].selfType() != comcfg::CONFIG_ERROR_TYPE) {
	            if (0 != lcmgr->load(temp_conf["LcClientConfig"])) {
                    delete lcmgr;
                    lcmgr = NULL;
                    free(svrn);
                    return NULL;
                }
	            free (svrn);
                return lcmgr;
            }
        }
	    free (svrn);
        return NULL;
    }
    memset(svrn, 0, cfglen);

    fprintf(fw->conf->conf_file, "%s", lcclient2_top);
	const char *ptr = cfg;
	while (sscanf(ptr, "%s", svrn) == 1) {
		ptr += strlen(svrn);
		while (*ptr && (*ptr == ' ' || *ptr == '\t')) {
			++ptr;
		}
        fprintf(fw->conf->conf_file, lcclient2_metaconf, svrn);
		LC_LOG_DEBUG("scan %s module in client, left %s", svrn, ptr);
	}
    fprintf(fw->conf->conf_file, lcclient2_galileo_top, svrn);
	ptr = cfg;
    while (sscanf(ptr, "%s", svrn) == 1) {
		ptr += strlen(svrn);
		while (*ptr && (*ptr == ' ' || *ptr == '\t')) {
			++ptr;
		}
        fprintf(fw->conf->conf_file, lcclient2_galileo, svrn);
		LC_LOG_DEBUG("scan %s module in client, left %s", svrn, ptr);
	}

	free (svrn);
    return lcmgr;
}



















