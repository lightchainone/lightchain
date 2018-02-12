/**
 */
#include <stdio.h>
#include <vector>
#include <string>
#include <map>

#ifndef __UL_URL_H__
#define __UL_URL_H__
#define  UL_MAX_URL_LEN_EX 1024
#define UL_MAX_SITE_LEN_EX 128
#define UL_MAX_PATH_LEN_EX 800
#define  UL_MAX_URL_LEN      256
#define  UL_MAX_SITE_LEN     48
#define  UL_MAX_PORT_LEN     7 //64k 5+:+1
#define UL_MAX_PATH_LEN 203
#define UL_MAX_FRAG_LEN 203

#define UL_MAX_URL_LEN_EX2 2048
#define UL_MAX_SITE_LEN_EX2 256
#define UL_MAX_PATH_LEN_EX2 1600


/**
 * 根据url解析出其中的各个部分
 *
 */
int ul_parse_url(const char *input,char *site,char *port,char *path);

/**
 * 归一化URL路径
 *
 */
int ul_single_path(char *path);

/**
 * 根据url解析出其中的路径部分
 *
 */
char *ul_get_path(const char *url, char *path);

/**
 * 根据url解析出其中的站点名部分
 *
 */
char *ul_get_site(const char *url, char *site);

/**
 * 规范化路径的形式\n
 * 对'\\', './', '/.', '../', '/..', '//'等形式进行规范化
 *
 */
void ul_normalize_path(char *path);

/**
 * 从url中获取端口信息
 *
 */
int ul_get_port(const char* url,int* pport);

/**
 * 从url中获取静态的部分（?或者;之前的部分）
 *
 */
void ul_get_static_part(const char *url, char *staticurl);

/*
 * to compitable old version lib, still keep the non-const functions
 * see the above functions for detail
 */
int ul_parse_url(char *input,char *site,char *port,char *path);
char *ul_get_path(char *url, char *path);
char *ul_get_site(char *url, char *site);
int ul_get_port(char* url,int* pport);
void ul_get_static_part(char *url, char *staticurl);

/**
 * 判断url是否是动态url
 *
 */
int ul_isdyn(char* str);

/**
 * 判断url是否合法
 *
 */
int ul_check_url(char* url);



/**
 * 从站点名中获取主干部分\n
 *
 */
int ul_fetch_trunk(const char* site,char *trunk,int size);

/**
 * 检查站点名是否是IP地址
 *
 */
int ul_is_dotip(const char *sitename);

/**
 *
 */
const char* ul_fetch_maindomain(const char* site,char *domain,int size);

/**
 * 检查url是否规范化
 *
 */
int ul_isnormalized_url(const char *url);

/**
 * 将url转化为统一的形式\n
 *
 */
int ul_normalize_url(const char* url, char* buf);

/**
 * 将站点名进行规范化（大写转为小写）
 *
 */
int ul_normalize_site(char *site);

/**
 * 将端口字符串进行规范化（检查端口范围合法性，并去掉80端口的字符串）
 *
 */
int ul_normalize_port(char *port);



 
 
/**
 *  根据url解析出其中的各个部分,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *  
 *  - 1   表示正常
 *  - 0  无效url格式
 */ 
int ul_parse_url_ex(const char *input, char *site,size_t site_size,
		                char *port, size_t port_size,char *path, size_t max_path_size);
                                
                                
                                
                                

/**
 *  根据url解析出其中的路径部分,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *  
 *  - 非NULL   指向路径的指针
 *  - NULL     表示失败
 */
char *ul_get_path_ex(const char *url, char *path,size_t path_size);





/**
 *  根据url解析出其中的站点名部分,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *  
 *  - 非NULL   指向site的指针
 *  - NULL     表示失败
 */ 
char *ul_get_site_ex(const char *url, char *site,  size_t site_size);




/**
 *  从url中获取端口信息,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *  
 *  - 1   表示成功
 *  - 0   表示失败
 */ 
int ul_get_port_ex(const char* url, int* pport);








/**
 *  将url转化为统一的形式\n,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *
 *  - 1   成功
 *  - 0   无效url
 */ 
int ul_normalize_url_ex(const char* url, char* buf,size_t buf_size);






/**
 *  从url中获取静态的部分（?或者;之前的部分）,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *
 */
void ul_get_static_part_ex(const char *url , char *staticurl ,size_t staticurl_size);







/**
 *  检查url是否规范化,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *
 *  - 1   是
 *  - 0   不是
 */
int ul_isnormalized_url_ex(const char *url);




/**
 *  规范化路径的形式\n,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 * 对'\\', './', '/.', '../', '/..', '//'等形式进行规范化
 *
 */ 
void ul_normalize_path_ex(char *path);




/**
 *  归一化URL路径,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *
 *  - 1   正常
 *  - 0   无效url格式路径
 */
int ul_single_path_ex(char *path);


/**
  *  判断url是否合法，支持对"#segment"部分的检查通过，同时确保原串不被修改
  *
  * 1 合法
  * 0 不合法
  */
int ul_check_url_vhplink(const char *url);

/**
 * 根据url解析出其中的各个部分,添加对fragment部分的解析
 *
 */
int ul_parse_url_vhplink(const char *input,char *site,char *port,char *path,char *frag);

/**
 * 将path部分归一化，但是对path里包含"http://"或者"HTTP://"的部分保留原样，
 * 其它策略跟老的接口一致
 *
 */
 void ul_normalize_path_vhplink(char *path) ;



/**
 *  判断url是否合法,支持加长的url，最高可支持到1024，path最长可到800，site最长可到128
 *
 *  - 1   合法
 *  - 0   不合法
 */
int ul_check_url_ex(char *url);

/////////////////////////////////////////////////////////////////////
 
/**
 *  根据url解析出其中的各个部分,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *  
 *  - 1   表示正常
 *  - 0  无效url格式
 */ 
int ul_parse_url_ex2(const char *input, char *site,size_t site_size,
		     char *port, size_t port_size,char *path, size_t max_path_size);
                                
                                
                                
                                

/**
 *  根据url解析出其中的路径部分,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *  
 *  - 非NULL   指向路径的指针
 *  - NULL     表示失败
 */
char *ul_get_path_ex2(const char *url, char *path,size_t path_size);





/**
 *  根据url解析出其中的站点名部分,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *  
 *  - 非NULL   指向site的指针
 *  - NULL     表示失败
 */ 
char *ul_get_site_ex2(const char *url, char *site,  size_t site_size);




/**
 *  从url中获取端口信息,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *  
 *  - 1   表示成功
 *  - 0   表示失败
 */ 
int ul_get_port_ex2(const char* url, int* pport);





/**
 *  将url转化为统一的形式\n,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *
 *  - 1   成功
 *  - 0   无效url
 */ 
int ul_normalize_url_ex2(const char* url, char* buf,size_t buf_size);






/**
 *  从url中获取静态的部分（?或者;之前的部分）,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *
 */
void ul_get_static_part_ex2(const char *url , char *staticurl ,size_t staticurl_size);







/**
 *  检查url是否规范化,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *
 *  - 1   是
 *  - 0   不是
 */
int ul_isnormalized_url_ex2(const char *url);



/**
 *  规范化路径的形式\n,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 * 对'\\', './', '/.', '../', '/..', '//'等形式进行规范化
 *
 */ 
void ul_normalize_path_ex2(char *path);




/**
 *  归一化URL路径,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *
 *  - 1   正常
 *  - 0   无效url格式路径
 */
int ul_single_path_ex2(char *path);



/**
 *  判断url是否合法,支持加长的url，最高可支持到2048，path最长可到1600，site最长可到256
 *
 *  - 1   合法
 *  - 0   不合法
 */
int ul_check_url_ex2(char *url);


//============================================================
/** 
 * -1 非法IP
 * -2 result的buffer size过小
 */
int ul_normalize_site_ip(const char* site,char* result,int result_size);
/** 
 * ==false的话那么返回0而trunk不确定。
 */
const char* ul_fetch_maindomain_ex(const char* site,char* trunk,int trunk_size,
                                   bool recoveryMode=true);

class DomainDict{
  plclic:
    typedef std::vector< std::string > DomainList;
    typedef std::map< std::string, DomainList > DomainListMap;
  private:
    DomainList top_domain_;
    DomainList top_country_;
    DomainList general_2nd_domain_;
    DomainListMap country_domain_map_;
  plclic:
    /** 
     */
    void clear();
    /** 
     */
    void load_default();
    /** 
     */
    int load_file(const char* path,const char* file_name);
    /** 
     */
    void add_top_domain(const char* s);
    /** 
     */
    void add_top_country(const char* s);
    /** 
     */
    void add_general_2nd_domain(const char* s);
    /** 
     */
    void add_country_domain(const char* s,const char* w);
  plclic:
    // 下面这些接口内部ul_url内部使用
    const DomainList& get_top_domain() const{
        return top_domain_;
    }
    const DomainList& get_top_country() const{
        return top_country_;
    }
    const DomainList& get_general_2nd_domain() const{
        return general_2nd_domain_;
    }
    const DomainListMap& get_country_domain_map() const{
        return country_domain_map_;
    }
  private:
    bool readline(const std::string& str);
};

/** 
 * ==false的话那么返回0而trunk不确定。
 */
const char* ul_fetch_maindomain_ex2(const char* site,char* trunk,int trunk_size,
                                    const DomainDict* dict,bool recoveryMode=true);


#endif
