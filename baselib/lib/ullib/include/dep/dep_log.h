
#ifndef __DEP_LOG_H_
#define __DEP_LOG_H_

#define LOG_BUFF_SIZE 640     /**< 一条日志buff大小,支持老版本共享内存而保留       */

#define MAX_ATTACH_NUM  1000
#define NAMENUM_INDATA  100
#define NAMELEN_INDATA  32
#define BUFSIZE_INDATA  640
#define SLEEP_INVAL     300

#define SVSEM_MODE      0666
#define SVSHM_MODE      (SHM_R | SHM_W | SHM_R>>3 | SHM_R>>6)
#define PRO_NAME_LEN    256
#define TIME_LEN        20

struct ul_log_ctrl {
    char  processname[PRO_NAME_LEN]; /**< 包含进程名和参数 , 并用"_" 连接。 如 ： start_-d_/usr/mydir_c_myconf */
    int  processid;
    char start_time[TIME_LEN];
    ul_log_ctrl *next;
};

typedef struct ul_log_ctrl  Ul_log_ctrl;

typedef struct {
    char  name[NAMELEN_INDATA];
    int  value;
} Ul_log_item;

typedef struct {
    Ul_log_item  item_str[NAMENUM_INDATA]; // the process information
    char    buff[BUFSIZE_INDATA] ; // record the WARNING and FATAL log
} Ul_log_data;


//typedef struct Ul_log_self;
typedef struct Ul_log_self Ul_log_self;

/**
 *
 * 这是为兼容老接口而保留，现在已经不使用了
**/
extern int ul_threadlog_sup();

/*
 * In order to monitor the state of the process when it is running.
 * we need to export some status information.share memory is a good
 * choise.
 * when we open the log, we will attach a piece of share memory(
 * about 100 name-value pairs and a 640 byte buffer for this process
 * to export the information.
 * when exporting information,
 * use ul_log_assign to assign a name-value pair,
 * use ul_log_getid to get the id of the name-value pair,
 * use ul_log_setval by the id and value to set the value,
 * use ul_log_getval by the id and value to increment the value.
 * when monitor the information,
 * use ul_log_monproc to choise the process data
 * use ul_log_getval to get the value
 * use ul_log_getbuf to get the buffer content
 *
 */




/**
 *
**/
extern int ul_log_assign(char *name, int ini_val);


/**
 *
**/
extern int ul_log_getid(char *name);

/**
 *
 */
extern int ul_log_setval( int rel_loc ,int val);

/**
 *
 */
extern int ul_log_addval( int rel_loc, int val);

/*
 *
 */
extern int ul_log_monproc(char * processname);

/*
 *
 */
extern int ul_log_getval(char * name,int *pval);

/*
 *
 */
extern int ul_log_getbuf(char * buff,int buflen);

/*
 *
 */
extern int ul_set_namenum(int namenum);

/**
 *
**/
extern int ul_log_createshm(void);

/**
 *
**/
extern int ul_log_attachshm(void);

/**
 * 
**/
extern int ul_log_deattachshm(void);

/**
 * 
**/

extern int ul_log_cleanshm(char *proc_name, char *item_name, bool clean_buff);

/**
 * 
**/


extern int ul_log_listproc(char *proc_name, Ul_log_ctrl *proc_ctrl);
/**
 * 
**/


extern int ul_log_listitem(char *proc_name, Ul_log_item *item_str, char *buff, int buff_len);

#endif // __DEP_LOG_H_
/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
