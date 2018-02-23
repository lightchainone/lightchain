#include "ul_sign.h"
#include "ul_url.h"

int create_url_sign(const char* urlstr,
                    unsigned int site_sign[2],
                    unsigned int url_sign[2]) {
    // 归一化URL，为计算128位的URL签名做准备，包括64位的site签名，以及64位的slcurl签名
    //URL长度为MAX_URL_LEN_EX2
    char normalized_url[UL_MAX_URL_LEN_EX2];
    normalized_url[0] = '\0';
    if (!(ul_normalize_url_ex2(urlstr, normalized_url, UL_MAX_URL_LEN_EX2))) {
        return -1;
    }
    //归一化最后为0.
    normalized_url[UL_MAX_URL_LEN_EX2-1]='\0';

    //然后得到site部分的内容..
    //SITE长度为MAX_SITE_LEN_EX2
    char site[UL_MAX_SITE_LEN_EX2];
    site[0] = '\0';
    if (!ul_get_site_ex2(normalized_url, site, UL_MAX_SITE_LEN_EX2)) {
        return -1;
    }
    //保证最后面依然为0.
    site[UL_MAX_SITE_LEN_EX2 - 1] = '\0';

    creat_sign_murmur64(site,strlen(site),site_sign,site_sign+1,0);
    creat_sign_murmur64(normalized_url,strlen(normalized_url),url_sign,url_sign+1,0);
    return 0;
}

int create_url_sign(const char *urlstr,
                    unsigned long long &site_sign,
                    unsigned long long &url_sign) {
    unsigned int x_site_sign[2];
    unsigned int x_url_sign[2];
    if(create_url_sign(urlstr,x_site_sign,x_url_sign)==-1) {
        return -1;
    }
    site_sign=(((unsigned long long)(x_site_sign[0]))<<32) +
              (unsigned long long)(x_site_sign[1]);
    url_sign=(((unsigned long long)(x_url_sign[0]))<<32) +
             (unsigned long long)(x_url_sign[1]);
    return 0;
}


//======================================================================
//为pssearcher刘成城提供20101026.
//需要配合ullib3.1.37.0使用.
int ul_normalize_url_ex2_special_for_ps_20101026(const char* url, char* buf,size_t buf_size);
int create_url_sign2(const char* urlstr,
                     unsigned int site_sign[2],
                     unsigned int url_sign[2]) {
    // 归一化URL，为计算128位的URL签名做准备，包括64位的site签名，以及64位的slcurl签名
    //URL长度为MAX_URL_LEN_EX2
    char normalized_url[UL_MAX_URL_LEN_EX2];
    normalized_url[0] = '\0';
    if (!(ul_normalize_url_ex2_special_for_ps_20101026(urlstr, normalized_url, UL_MAX_URL_LEN_EX2))) {
        return -1;
    }
    //归一化最后为0.
    normalized_url[UL_MAX_URL_LEN_EX2-1]='\0';

    //然后得到site部分的内容..
    //SITE长度为MAX_SITE_LEN_EX2
    char site[UL_MAX_SITE_LEN_EX2];
    site[0] = '\0';
    if (!ul_get_site_ex2(normalized_url, site, UL_MAX_SITE_LEN_EX2)) {
        return -1;
    }
    //保证最后面依然为0.
    site[UL_MAX_SITE_LEN_EX2 - 1] = '\0';

    creat_sign_murmur64(site,strlen(site),site_sign,site_sign+1,0);
    creat_sign_murmur64(normalized_url,strlen(normalized_url),url_sign,url_sign+1,0);
    return 0;
}

int create_url_sign2(const char *urlstr,
                    unsigned long long &site_sign,
                    unsigned long long &url_sign) {
    unsigned int x_site_sign[2];
    unsigned int x_url_sign[2];
    if(create_url_sign2(urlstr,x_site_sign,x_url_sign)==-1) {
        return -1;
    }
    site_sign=(((unsigned long long)(x_site_sign[0]))<<32) +
              (unsigned long long)(x_site_sign[1]);
    url_sign=(((unsigned long long)(x_url_sign[0]))<<32) +
             (unsigned long long)(x_url_sign[1]);
    return 0;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
