#include "lcclient_utils.h"

#include <galileo.h>
#include <lc_log.h>

#include "lcclient_manager.h"
#include "lcclient_define.h"



namespace lc {
    static const char * ERRINFO[] = {
                                "success",
                                "param_error",
                                "unknown error occur in talk",
                                "read head error in talk",
                                "read body error in talk",
                                "buf len for read body not enough",    
                                "write head error in talk",
                                "netreactor post read head error",
                                "connect error",
                                "write body err in talk",
                                "lc reactor post read body error, maybe socket is destroyed",  
                                "lc reactor post write body error, maybe socket is destroyed",
                                "talk init error, check the input",
                                "fetch server for socket error",
                                "other talk in init, maybe new error",
                                "wait but no msg", 
                                "talk is in processing",
                                "run user callback error", 
                                "netreactor post write head error",
                                "netreactor run error",
                                "netreactor is not null",
                                "too many asynctalks are waiting in server list",
                                "range in talk is error",
    };

    static const int ERRINFOLEN = sizeof(ERRINFO) / sizeof(char *);
    
    
    const char *get_talk_errorinfo(int errNo)
    {
        if (errNo > 0) {
            return "";
        }
        int realNo = -errNo;
        if (realNo < ERRINFOLEN) {
            return ERRINFO[realNo];
        } else {
            return ERRINFO[ERRINFOLEN-1];
        }
    }

    
    void print_talk_errorinfo(int errNo)
    {
        LC_LOG_WARNING("%s", get_talk_errorinfo(errNo));

        return;
    }

    
    int IntVector :: push_back(int id)
    {
        if (isFull()) {
            return -1;    
        }
        _id[_currSize++] = id;

        return 0;
    }

    static int get_hostent_ip(const char *domain, bsl::string &result) {
        struct hostent *host = NULL;
        host = gethostbyname(domain);
        if (NULL == host || NULL == host->h_addr_list ||
                NULL == host->h_addr_list[0]) {
            LC_LOG_WARNING("gethostbyname(%s) error", domain);
            return -1;
        }

        int i = 0;
        char *tmp_ip = host->h_addr_list[i];
        while (tmp_ip != NULL) {
            result.append(inet_ntoa( *(struct in_addr *)tmp_ip));
            result.append(" ");

            i ++;
            tmp_ip = host->h_addr_list[i];
        }

        return 0;
    }

    int convert_conf_domain2ip(comcfg::ConfigUnit &conf)
    {
        if (conf.selfType() == comcfg::CONFIG_ERROR_TYPE ||
                conf["LcClient"].selfType() == comcfg::CONFIG_ERROR_TYPE ||
                conf["LcClient"]["Service"].selfType() == comcfg::CONFIG_ERROR_TYPE) {
            LC_LOG_WARNING("Conf Type is Error, pleaes Check [.LcClient][@Service]");
            return -1;
        }

        int service_num = 0;
        int server_num = 0;
        try {
            service_num = conf["LcClient"]["Service"].size();
            for (int i=0; i<service_num; i++) {
                if (conf["LcClient"]["Service"][i]["Server"].selfType() == comcfg::CONFIG_ERROR_TYPE) {
                    LC_LOG_WARNING("conf Server not exist in Service[%d]", i);
                    return -1;
                }

                server_num = conf["LcClient"]["Service"][i]["Server"].size();
                comcfg::ConfigUnit &server_conf = conf["LcClient"]["Service"][i]["Server"];
                for (int j=0; j<server_num; j++) {
                    if (server_conf[j]["ServerDomain"].selfType() == comcfg::CONFIG_ERROR_TYPE) {
                        LC_LOG_WARNING("ServerDomain not exist in Service[%d] Server[%d]", i, j);
                        return -1;
                    }

                    bsl::string ip_list;
                    bsl::string server_domain;
                    if (server_conf[j]["ServerDomain"].get_bsl_string(&server_domain) != 0){
                        LC_LOG_WARNING("ServerDomain not exist in Service[%d] Server[%d]", i, j);
                        return -1;
                    }

                    const char *str_server_domain = server_domain.c_str();
                    int server_domain_size = server_domain.size();
                    char tmp_single_domain[1024];
                    memset(tmp_single_domain, 0, sizeof(tmp_single_domain));
                    int tmp_domain_pos = 0;
                    int k = 0;

                    while (k < server_domain_size) {
                        if (' ' == str_server_domain[k]) {
                            if (0 == tmp_domain_pos) {
                                k ++;
                                continue;
                            }

                            if (tmp_domain_pos > (int)(sizeof(tmp_single_domain)-1)) {
                                LC_LOG_WARNING("single domain to long, max[%d]", (int)sizeof(tmp_single_domain));
                                return -1;
                            }

                            tmp_single_domain[tmp_domain_pos] = '\0';
                            bsl::string bsl_ip;
                            int get_ret = get_hostent_ip(tmp_single_domain, bsl_ip);
                            if (get_ret != 0) {
                                LC_LOG_WARNING("get ip error");
                                return -1;
                            }
                            ip_list.append(bsl_ip);
                            ip_list.append(" ");

                            memset(tmp_single_domain, 0, sizeof(tmp_single_domain));
                            tmp_domain_pos = 0;
                            k ++;
                        } else {
                            tmp_single_domain[tmp_domain_pos] = str_server_domain[k];
                            tmp_domain_pos++;
                            k ++;
                        }
                    }

                    if (k > 0) {
                        if (str_server_domain[k-1] != ' ') {
                            if (tmp_domain_pos <= (int)(sizeof(tmp_single_domain)-1)) {
                                tmp_single_domain[tmp_domain_pos] = '\0';
                                bsl::string bsl_ip;
                                int get_ret = get_hostent_ip(tmp_single_domain, bsl_ip);
                                if (get_ret != 0) {
                                    LC_LOG_WARNING("get ip error");
                                    return -1;
                                }
                                ip_list.append(bsl_ip);
                                ip_list.append(" ");
                            } else {
                                LC_LOG_WARNING("single domain to long, max[%d]", (int)sizeof(tmp_single_domain));
                                return -1;
                            }
                        }
                    }
                    if (server_conf[j].add_unit("IP", ip_list) != 0) {
                        LC_LOG_WARNING("add ip_list to conf error");
                        return -1;
                    }
                }
            }

        } catch(bsl::Exception &e) {
            LC_LOG_WARNING("catch bsl exception:%s", e.all());
            return -1;
        } catch(...) {
            LC_LOG_WARNING("catch unknown exception");
            return -1;
        }
        return 0;
    }

    int register_server_bygalileo(const comcfg::ConfigUnit &conf)
    {
        if (conf.selfType() == comcfg::CONFIG_ERROR_TYPE ||
                 conf[LCCLIENT_CONF_GALILEO_HOST].selfType() == comcfg::CONFIG_ERROR_TYPE ||
                 conf[LCCLIENT_CONF_GALILEO_REGSES].selfType() == comcfg::CONFIG_ERROR_TYPE ||
                 conf[LCCLIENT_CONF_GALILEO_REGSES][LCCLIENT_CONF_GALILEO_REGSES_RESADDR].selfType() == comcfg::CONFIG_ERROR_TYPE) {
            LC_LOG_WARNING("register_server conf error, check %s %s", LCCLIENT_CONF_GALILEO_HOST, LCCLIENT_CONF_GALILEO_REGSES);
            return -1;
        }

        bsl::string galileo_log;
        if (conf[LCCLIENT_CONF_GALILEO_LOG].
                get_bsl_string(&galileo_log, "../log/zoo.pspui.log") != 0) {
            LC_LOG_WARNING("get %s error", LCCLIENT_CONF_GALILEO_LOG);
        }
		GalileoManager::open_log(galileo_log.c_str());

        bsl::string galileo_host;
        if (conf[LCCLIENT_CONF_GALILEO_HOST].get_bsl_string(&galileo_host) != 0) {
            LC_LOG_WARNING("get %s from conf error", LCCLIENT_CONF_GALILEO_HOST);
            return -1;
        }

		GalileoCluster *cluster = 
			GalileoManager::get_instance()->get_cluster(galileo_host.c_str(), true);
		if(cluster == NULL) {
            LC_LOG_WARNING("create galileo error:%s[%m]", galileo_host.c_str());
            return -1;
        }

        int resource_size = conf[LCCLIENT_CONF_GALILEO_REGSES][LCCLIENT_CONF_GALILEO_REGSES_RESADDR].size();
        for (int i=0; i<resource_size; i++) {
            if (conf[LCCLIENT_CONF_GALILEO_REGSES][LCCLIENT_CONF_GALILEO_REGSES_RESADDR][i][LCCLIENT_CONF_GALILEO_REGSES_RESADDR_DATA].selfType() == comcfg::CONFIG_ERROR_TYPE ||
                    conf[LCCLIENT_CONF_GALILEO_REGSES][LCCLIENT_CONF_GALILEO_REGSES_RESADDR][i][LCCLIENT_CONF_GALILEO_REGSES_RESADDR_DATA].selfType() == comcfg::CONFIG_ERROR_TYPE)
            {
                LC_LOG_WARNING("register_resource:%d not has ADDR or DATA", i);
                return -1;
            }
            bsl::string tmp_addr;
            bsl::string tmp_data;
            if (conf[LCCLIENT_CONF_GALILEO_REGSES][LCCLIENT_CONF_GALILEO_REGSES_RESADDR][i][LCCLIENT_CONF_GALILEO_REGSES_RESADDR_ADDR].get_bsl_string(&tmp_addr) != 0) {
                LC_LOG_WARNING("register_resource:%d addr is not string", i);
                return -1;
            }
            if(conf[LCCLIENT_CONF_GALILEO_REGSES][LCCLIENT_CONF_GALILEO_REGSES_RESADDR][i][LCCLIENT_CONF_GALILEO_REGSES_RESADDR_DATA].get_bsl_string(&tmp_data) != 0) {
                LC_LOG_WARNING("register_resource:%d DATA is not string", i);
                return -1;
            }

			if(cluster->register_resource(tmp_addr.c_str(), tmp_data.c_str(), NULL) != 0) {
                LC_LOG_WARNING("register_resource:%d error", i);
                return -1;
            }
        }

        return 0;
    }
}
    
    
