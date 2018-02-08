
#include "zcacheapi.h"
#include "zcachedata.h"
#include "lnhead.h"
#include "mynet.h"
#include "msgdef.h"
#include "asyncremoteinvoker.h"
#include "util.h"
#include "commwithcs.h"

static void status_req_cb(async_remote_invoker_arg_t *arg);

extern int csc_running;

int send_status_req(single_zcache_t *zcache, const unsigned int ip[2], unsigned int port, bool is_report_detail)
{
    CS_REQ_DEF(zcache_cmd_status_req_t, zcache_cmd_status_res_t);
    req_head->cmd_type = ZCACHE_CMD_STATUS;
    req->is_report_detail = is_report_detail;
    req->product_num = 1;
    memcpy(req->products, zcache->product->GetName(), ZCACHE_MAX_PRODUCT_NAME_LEN);

    ret = zcache->mgr->AsyncRemoteInvoker(arg, status_req_cb);
    if (ret != 0)
    {
        ret  = -1;
        WARNING_LOG("AsyncRemoteInvoker error");
        goto err_exit;
    }

    CS_REQ_DESTROY();
    return ret;
}

void status_req_cb(async_remote_invoker_arg_t *arg)
{
    char *res_buf = NULL;
    unsigned int res_buf_len = 0;
    unsigned int nodeid = 0;
    if (!arg) {
        WARNING_LOG("param error");
        return;
    }

    CS_REQ_CB_DEF(arg, zcache_cmd_status_req_t, zcache_cmd_status_res_t);

    int ret = 0;

    if (!zcache->product) {
        goto out;
    }

    ret = zcache->product->get_nodeid(arg->ip, arg->port, &nodeid);
    if (ret < 0) {
        WARNING_LOG("node not exist [ip1: %s, ip2: %s, port: %u]", csvr_ip0, csvr_ip1, arg->port);
        goto out;
    }

    if (arg->status != ZCACHE_OK) {
        WARNING_LOG("async_remote_invoker is NOT OK [status: %s]", get_err_code((zcache_err_t)arg->status));
        zcache->product->InvalidNode(nodeid);
        goto out;
    }

    if (!is_data_correct) {
        WARNING_LOG("data incorrect");
        zcache->product->InvalidNode(nodeid);
        goto out;
    }

    if (res_head->cmd_ret != ZCACHE_OK) {
        WARNING_LOG("cache server response NOT OK [status: %s]", get_err_code((zcache_err_t)res_head->cmd_ret));
        zcache->product->InvalidNode(nodeid);
        goto out;
    }

    if (req->is_report_detail) {
        zcache->product->SetNodeStatus(nodeid, res->status);
    } else {
        zcache->product->ValidNode(nodeid);
    }

out:
    CS_REQ_CB_DESTROY();
    return;
}

void *cs_check(void *arg)
{
    single_zcache_t *cache = (single_zcache_t *)arg;
    zcache_err_t err = ZCACHE_OK;
    zcache_node_t node;
    zlog_open_r("cscheck");

    while (!cache->is_block_fetched) {
        struct timeval v;
        time_to_val(1000*cache->ms_refetch_time_ms, v);
        select(0, NULL, NULL, NULL, &v);

        if (!cache->is_running || 0 == csc_running) {
            time_to_val(ZCACHEAPI_DEFAULT_CSCHECK_TIME * 2, v);
            select(0, NULL, NULL, NULL, &v);
            pthread_exit(0);
        }
    }

    while (1) {
        struct timeval v;
        time_to_val(cache->cscheck_time_us, v);
        select(0, NULL, NULL, NULL, &v);

        if (!cache->is_running || 0 == csc_running) {
            time_to_val(ZCACHEAPI_DEFAULT_CSCHECK_TIME * 2, v);
            select(0, NULL, NULL, NULL, &v);
            pthread_exit(0);
        }

        for(unsigned int i = 0; i < cache->product->get_node_num(); i++) {

            if (!cache->is_running || 0 == csc_running) {
                time_to_val(ZCACHEAPI_DEFAULT_CSCHECK_TIME * 2, v);
                select(0, NULL, NULL, NULL, &v);
                pthread_exit(0);
            }

            if (cache->product->get_node(i, &node) < 0) {
                WARNING_LOG("get node error [nodeid: %u]", i);
            } else {
                send_status_req(cache, node.ip, node.port, true);
            }
        }
    }
    return NULL;
}






















