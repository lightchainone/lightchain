

#include <Lsc/containers/list/Lsc_list.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "resourcepool.h"

int shutdownit = 0;
RsrcPool *respool_hdlr = RsrcPool::get_instance();
char tmpbuff[10];

int startsWith(const char *line, const char *prefix) {
    int len = strlen(prefix);
    return strncmp(line, prefix, len) == 0;
}

int get_socket_peer_ip(int socket, char *ip, size_t size)
{
    assert(socket > 0); 

    socklen_t slen = sizeof(struct sockaddr_in);
    struct sockaddr_in sa; 

    if(0 != getpeername(socket, (struct sockaddr*)&sa, &slen)) {
        return 0;
    }   
    if (slen > 0) {
        inet_ntop(AF_INET, &(sa.sin_addr.s_addr), ip, size);
    }   
    else {
        return 0;
    }   

    return sa.sin_addr.s_addr;
}

void processline(char* line)
{
    int rc = 0;
    if (0 == strlen(line)){
        return;
    }

    if (startsWith(line, "help")){
        fprintf(stdout, "HELP:\n");
        fprintf(stdout, "     add_req_res $res_lleo $res_name $res_addr $capacity\n");
        fprintf(stdout, "     rm_req_res $res_lleo $res_name $res_addr\n");
        fprintf(stdout, "     add_reg_res $res_lleo $res_addr '$data'\n");
        fprintf(stdout, "     rm_reg_res $res_lleo $res_addr $res_tag\n");
        fprintf(stdout, "     exit\n");
    }
    else if (startsWith(line, "exit")){
        shutdownit = 1;
    }
    else if (startsWith(line, "add_req_res")){
        
        char res_name[GALILEO_DEFAULT_PATH_LEN];
        char res_lleo[GALILEO_DEFAULT_PATH_LEN];
        char res_addr[GALILEO_DEFAULT_PATH_LEN];
        int capacity = 0;
        char *ptr = line;
        char *ptr_end = line;

        ptr = strchr(line, ' ');
        if (NULL == ptr) goto ERROR_EXIT;

        ptr += 1;
        while(*ptr == ' ')
            ptr += 1;
            
        ptr_end = strchr(ptr, ' ');
        if (NULL == ptr_end) goto ERROR_EXIT;

        strncpy(res_lleo, ptr, ptr_end-ptr);
        res_lleo[ptr_end-ptr] = '\0';
        fprintf(stdout, "res_name:%s\n", res_lleo);
        ptr = ptr_end + 1;
        
        while(*ptr == ' ')
            ptr += 1;
            
        ptr_end = strchr(ptr, ' ');
        if (NULL == ptr_end) goto ERROR_EXIT;

        strncpy(res_name, ptr, ptr_end-ptr);
        res_name[ptr_end-ptr] = '\0';
        fprintf(stdout, "res_name:%s\n", res_name);
        
        ptr = ptr_end + 1;
        while(*ptr == ' ')
            ptr += 1; 

        ptr_end = strchr(ptr, ' ');
        if (NULL == ptr_end) goto ERROR_EXIT;
            
        strncpy(res_addr, ptr, ptr_end-ptr);
        res_addr[ptr_end-ptr] = '\0';
        fprintf(stdout, "res_addr:%s\n", res_addr);

        ptr = ptr_end + 1;
        while(*ptr == ' ')
            ptr += 1;
        capacity = atoi(ptr);
        fprintf(stdout, "capacity:%d\n", capacity);

        rc = respool_hdlr->add_request_resource(res_name, res_addr, capacity);
        fprintf(stdout, "rc:%d\n", rc);
        respool_hdlr->show_request_resource();
    }
    else if (startsWith(line, "rm_req_res")){
        
        char res_name[GALILEO_DEFAULT_PATH_LEN];
        char res_lleo[GALILEO_DEFAULT_PATH_LEN];
        char res_addr[GALILEO_DEFAULT_PATH_LEN];
        char *ptr = line;
        char *ptr_end = line;

        ptr = strchr(line, ' ');
        if (NULL == ptr) goto ERROR_EXIT;
        ptr += 1;
        while(*ptr == ' ')
            ptr += 1;

        ptr_end = strchr(ptr, ' ');
        if (NULL == ptr_end) goto ERROR_EXIT;
        
        strncpy(res_lleo, ptr, ptr_end-ptr);
        res_lleo[ptr_end-ptr] = '\0';
        fprintf(stdout, "res_lleo:%s\n", res_lleo);
        ptr = ptr_end + 1;
        while(*ptr == ' ')
            ptr += 1;

 
        ptr_end = strchr(ptr, ' ');
        if (NULL == ptr_end) goto ERROR_EXIT;
        
        strncpy(res_name, ptr, ptr_end-ptr);
        res_name[ptr_end-ptr] = '\0';
        fprintf(stdout, "res_name:%s\n", res_name);

        ptr = ptr_end + 1;
        while(*ptr == ' ')
            ptr += 1;
        
        ptr_end = strchr(ptr, ' ');
        if (NULL == ptr_end)
            ptr_end = strchr(ptr, '\0');

        strncpy(res_addr, ptr, ptr_end-ptr);
        res_addr[ptr_end-ptr] = '\0';
        fprintf(stdout, "res_addr:%s\n", res_addr);

        rc = respool_hdlr->remove_request_resource(res_name, res_addr);
        fprintf(stdout, "rc:%d\n", rc);
        respool_hdlr->show_request_resource();
    }
    else if (startsWith(line, "add_reg_res")){
        
        char res_addr[GALILEO_DEFAULT_PATH_LEN];
        char res_lleo[GALILEO_DEFAULT_PATH_LEN];
        char res_data[GALILEO_DEFAULT_BUF_SIZE];
        char *ptr = line;
        char *ptr_end = line;

        ptr = strchr(line, ' ');
        if (NULL == ptr) goto ERROR_EXIT;
        ptr += 1;

        while(*ptr == ' ')
            ptr += 1;
         
        ptr_end = strchr(ptr, ' ');
        if (NULL == ptr_end) goto ERROR_EXIT;

        strncpy(res_lleo, ptr, ptr_end-ptr);
        res_lleo[ptr_end-ptr] = '\0';
        fprintf(stdout, "res_lleo:%s\n", res_lleo);
        ptr = ptr_end + 1;
        while(*ptr == ' ')
            ptr += 1;
       
        ptr_end = strchr(ptr, ' ');
        if (NULL == ptr_end) goto ERROR_EXIT;

        strncpy(res_addr, ptr, ptr_end-ptr);
        res_addr[ptr_end-ptr] = '\0';
        fprintf(stdout, "res_addr:%s\n", res_addr);

        ptr = ptr_end + 1;
        while(*ptr != '\'')
            ptr += 1;
        if (*ptr == '\0') goto ERROR_EXIT;

        ptr += 1;
        ptr_end = strchr(ptr, '\'');
        if (NULL == ptr_end) goto ERROR_EXIT;

        strncpy(res_data, ptr, ptr_end-ptr);
        res_data[ptr_end-ptr] = '\0';
        fprintf(stdout, "res_data:%s\n", res_data);

        rc = respool_hdlr->add_register_resource(res_addr, res_data);
        fprintf(stdout, "rc:%d\n", rc);
        respool_hdlr->show_register_resource();
    }
    else if (startsWith(line, "rm_reg_res")){
        
        char res_addr[GALILEO_DEFAULT_PATH_LEN];
        char res_lleo[GALILEO_DEFAULT_PATH_LEN];

        int res_tag = 0;
        char *ptr = line;
        char *ptr_end = line;

        ptr = strchr(line, ' ');
        if (NULL == ptr) goto ERROR_EXIT;
        ptr += 1;

        while(*ptr == ' ')
            ptr += 1;

        ptr_end = strchr(ptr, ' ');
        if (NULL == ptr_end) goto ERROR_EXIT;

        strncpy(res_lleo, ptr, ptr_end-ptr);
        res_lleo[ptr_end-ptr] = '\0';
        fprintf(stdout, "res_addr:%s\n", res_lleo);
        ptr = ptr_end + 1;
        while(*ptr == ' ')
            ptr += 1;

        ptr_end = strchr(ptr, ' ');
        if (NULL == ptr_end) goto ERROR_EXIT;

        strncpy(res_addr, ptr, ptr_end-ptr);
        res_addr[ptr_end-ptr] = '\0';
        fprintf(stdout, "res_addr:%s\n", res_addr);
        
        ptr = ptr_end + 1;
        while(*ptr == ' ')
            ptr += 1;
        res_tag = atoi(ptr);
        fprintf(stdout, "res_tag: %d\n", res_tag);

        rc = respool_hdlr->remove_register_resource(res_addr, res_tag);
        fprintf(stdout, "rc:%d\n", rc);
        respool_hdlr->show_register_resource();
    }
    else if (startsWith(line, "fetch_socket")){
        
        char res_name[GALILEO_DEFAULT_PATH_LEN];
        char *ptr = line;
        char *ptr_end = line;

        ptr = strchr(line, ' ');
        if (NULL == ptr) goto ERROR_EXIT;

        ptr += 1;
        while(*ptr == ' ')
            ptr += 1;

        ptr_end = strchr(ptr, ' ');
        if (NULL == ptr_end) goto ERROR_EXIT;

        strncpy(res_name, ptr, ptr_end-ptr);
        res_name[ptr_end-ptr] = '\0';
        fprintf(stdout, "res_name:%s\n", res_name);
        
        ptr = ptr_end + 1;
        while(*ptr == ' ')
            ptr += 1;
        if (*ptr == '\0') goto ERROR_EXIT;

        int key = atoi(ptr);
        
        rc = respool_hdlr->fetch_socket(res_name, key);
        if (rc >= 0){
            char ip[32];
            get_socket_peer_ip(rc, ip, 32);
            fprintf(stdout, "fd:%d, ip:%s\n", rc, ip);
        }
        else
            fprintf(stdout, "rc:%d\n", rc);

        rc = read(0, tmpbuff, 0);
        if (0 == rc){
            fprintf(stderr, "ok\n");
        }
        else{
            fprintf(stderr, "rc:%d, err:%m\n", rc);
        }
    }
    else if (startsWith(line, "free_socket")){
        
        char *ptr = line;
        char *ptr_end = line;
        char res_name[GALILEO_DEFAULT_PATH_LEN];

        ptr = strchr(line, ' ');
        if (NULL == ptr) goto ERROR_EXIT;

        ptr += 1;
        while(*ptr == ' ')
            ptr += 1;

        ptr_end = strchr(ptr, ' ');
        if (NULL == ptr_end) goto ERROR_EXIT;

        strncpy(res_name, ptr, ptr_end-ptr);
        res_name[ptr_end-ptr] = '\0';
        fprintf(stdout, "res_name:%s\n", res_name);

        ptr = ptr_end + 1;
        while(*ptr == ' ')
            ptr += 1;
        
        int fd = atoi(ptr);
        rc = respool_hdlr->free_socket(res_name, fd, 0);
        fprintf(stdout, "rc:%d\n", rc);

    }
    else {
        fprintf(stdout, "Error command found\n");
    }

    return;

ERROR_EXIT:
    fprintf(stdout, "Error command found\n");
}

int main()
{
    int rc = 0;
    int len = 0;
    int buff_off = 0;
    char buff[4096];

    ul_logstat_t admin_log_stat = {UL_LOG_DEBUG, 0, 0};
    ul_openlog("./log/", "testpool.", &admin_log_stat, 1500);

    int ret = respool_hdlr->init("./conf/", "lleo.conf");
    if (0 != ret){
        fprintf(stderr, "initalization failed, ret:%d\n", ret);
        exit(-1);
    }
    fprintf(stderr, "initalized!\n");

    respool_hdlr->show_request_resource();
    respool_hdlr->show_register_resource();

    fprintf(stderr, "Waiting for command\n");
    fprintf(stdout, "$ ");
    fflush(stdout);

    while (!shutdownit){
        len = sizeof(buff) - buff_off - 1;
        if (len <= 0){
            fprintf(stderr, "Can't handle lines that long!\n");
            exit(-2);
        }

        rc = read(0, buff+buff_off, len);
        if (rc <= 0){
            buff[0] = '\0';
            buff_off = 0;
            continue;
        }

        buff_off += rc;
        buff[buff_off] = '\0';
        while(strchr(buff, '\n')){
            char *ptr = strchr(buff, '\n');
            *ptr = '\0';
            processline(buff);
            ptr++;
            memmove(buff, ptr, strlen(ptr)+1);
            buff_off = 0;
            fprintf(stdout, "$ ");
            fflush(stdout);
        }
    }

    fprintf(stderr, "Every thing is done\n");

    
    
    

    
    

    
    

    
    
    
    
    
 
    
    

    
    
    
    
    

    
    
    

    return 0;
}
