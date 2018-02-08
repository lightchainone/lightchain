
#include "lleo_admin.h"
#include "lleo_admin_work.h"

extern ul_logstat_t admin_log_stat;

static void print_help()
{
    printf("%s\n",   "Usage: lleo_admin [optiions]");
    printf("%s\n",   "options:");
    printf("\t%s\n", "-v:   show the version information");
    printf("\t%s\n", "-h:   the host:port for resource server");
    printf("\t%s\n", "-p:   the path of node at the resource server");
    printf("\t%s\n", "-d:   the resource data pack to set");
    printf("\t%s\n", "-a:   the resource acl pack to set");
    printf("\t%s\n", "-f:   the configure file used in batch nodes creation");
    printf("\t%s\n", "-g:   batch create nodes from configure file");
    printf("\t%s\n", "-q:   query the data and acl of a resource node");
    printf("\t%s\n", "-s:   query the statistical data of a resource node");
    printf("\t%s\n", "-c:   query the children list of a resource node");
    printf("\t%s\n", "-b:   build a abstract resource node");
    printf("\t%s\n", "-u:   update the data and acl of a resource node");
    printf("\t%s\n", "-r:   remove the resource node");
    printf("\t%s\n", "-t:   test register entity node");
    printf("\t%s\n", "-w:   test watch a resource");
    printf("\t%s\n", "-i:   identification of a user");

    return;
}



static void print_version()
{
    printf("Project    :%s\n", PROJECT_NAME);
    printf("Version    :%s\n", VERSION);
    printf("CVSTag     :%s\n", CVSTAG);
    printf("BuildDate  :%s\n", BUILD_DATE);
    return;
}



static void reset_admin_req(admin_req_t *req)
{
    if (NULL == req){
        fprintf(stderr, "%s\n", "Failed to get a valid admin request");
        return;
    }

    req->req_type     = '\0';
    req->host[0]      = '\0';
    req->path[0]      = '\0';
    req->data_pack[0] = '\0';
    req->acl_pack[0]  = '\0';
    req->id[0]        = '\0';

    req->conf[0]      = '\0';
}




static int process_admin_req(admin_req_t *req)
{
    int grc = GALILEO_E_OK;

    switch(req->req_type){
        case 'g':
            grc = process_admin_req_g(req);
            break;
        case 'q':
            grc = process_admin_req_q(req);
            break;
        case 's':
            grc = process_admin_req_s(req);
            break;
        case 'c':
            grc = process_admin_req_c(req);
            break;
        case 'b':
            grc = process_admin_req_b(req);
            break;
        case 'u':
            grc = process_admin_req_u(req);
            break;
        case 'r':
            grc = process_admin_req_r(req);
            break;
        case 't':
            grc = process_admin_req_t(req);
            break;
        case 'w':
            grc = process_admin_req_w(req);
            break;
        default:
            print_help();
            grc = GALILEO_E_BADPARAM;
    }

    return grc;
}



int main(int argc, char** argv)
{
    int c = 0;
    int ret = 0;
    int grc = GALILEO_E_OK;
    admin_req_t req;

    if (argc < 2){
        print_help();
        exit(1);
    }

    reset_admin_req(&req);

    while (-1 != (c = getopt(argc, argv, "h:p:d:a:f:i:gvqscburtw"))) {
        switch (c) {
            case 'v':
                print_version();
                return 0;
            case 'h':
                snprintf(req.host, GALILEO_DEFAULT_PATH_LEN, "%s", optarg);
                break;
            case 'p':
                snprintf(req.path, GALILEO_DEFAULT_PATH_LEN, "%s", optarg);
                break;
            case 'd':
                snprintf(req.data_pack, GALILEO_DEFAULT_BUF_SIZE, "%s", optarg);
                break;
            case 'a':
                snprintf(req.acl_pack, GALILEO_DEFAULT_BUF_SIZE, "%s", optarg);
                break;
            case 'f':
                snprintf(req.conf, GALILEO_DEFAULT_PATH_LEN, "%s", optarg);
                break;
            case 'i':
                snprintf(req.id, GALILEO_ACL_ID_LEN, "%s", optarg);
                break;

            case 'g':
                req.req_type = ('\0' == req.req_type)?'g':req.req_type;
                break;
            case 'q':
                req.req_type = ('\0' == req.req_type)?'q':req.req_type;
                break;
            case 's':
                req.req_type = ('\0' == req.req_type)?'s':req.req_type;
                break;
            case 'c':
                req.req_type = ('\0' == req.req_type)?'c':req.req_type;
                break;
            case 'b':
                req.req_type = ('\0' == req.req_type)?'b':req.req_type;
                break;
            case 'u':
                req.req_type = ('\0' == req.req_type)?'u':req.req_type;
                break;
            case 'r':
                req.req_type = ('\0' == req.req_type)?'r':req.req_type;
                break;
            case 't':
                req.req_type = ('\0' == req.req_type)?'t':req.req_type;
                break;
            case 'w':
                req.req_type = ('\0' == req.req_type)?'w':req.req_type;
                break;
            default:
                print_help();
                return 0;
        }
    }

#ifdef GALILEO_ADMIN_TEST
    printf("admin_req:\n");
    printf("\thost: %s\n",      req.host);
    printf("\treq_type: %c\n",  req.req_type);
    printf("\tpath: %s\n",      req.path);
    printf("\tdata_pack: %s\n", req.data_pack);
    printf("\tacl_pack: %s\n\n",  req.acl_pack);
#endif

    
    ul_logstat_t admin_log_stat = {UL_LOG_DEBUG, 0, 0};
    ret = ul_openlog("./log/", "GalileoAdmin.", &admin_log_stat, 1500);
    if (0 != ret){
        fprintf(stderr, "lc_log_init [dir:%s file:%s size:%d level:%d ] failed!",
            "./", "lleo_admin.", 1500, 16);
    }
    lleo_set_log("GalileoAdmin.Zoo.log", 16);
    grc = (GALILEO_E_OK != grc)? grc : process_admin_req(&req);
    lleo_close_log();

   return 0;
}
