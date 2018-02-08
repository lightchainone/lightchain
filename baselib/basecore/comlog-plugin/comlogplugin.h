

#ifndef  __COMLOGPLUGIN_H_
#define  __COMLOGPLUGIN_H_

#include "Configure.h"
#include "com_log.h"


#define CP_KEY_TYPE        "type"          
#define CP_KEY_PATH        "path"          
#define CP_KEY_FILE        "file"          
#define CP_KEY_AUTH        "auth"          
#define CP_KEY_RESERVED1   "reserved1"     
#define CP_KEY_RESERVED2   "reserved2"     
#define CP_KEY_RESERVED3   "reserved3"     
#define CP_KEY_RESERVED4   "reserved4"     
#define CP_KEY_SPLIT_TYPE  "split_type"    
#define CP_KEY_LOG_SIZE    "log_size"      
#define CP_KEY_OPEN        "open"          
#define CP_KEY_COMPRESS    "compress"      
#define CP_KEY_CUTTIME     "cuttime"       
#define CP_KEY_SYSLEVEL    "syslevel"      
#define CP_KEY_SELFLEVEL   "selflevel"     
#define CP_KEY_LAYOUT      "layout"        
#define CP_KEY_LAYOUT_NDC  "layout_ndc"    

#define CP_KEY_PROCNAME    "procname"      
#define CP_KEY_LEVEL       "level"         
#define CP_KEY_SELFDEFINE  "selfdefine"    
#define CP_KEY_LOG_LENGTH  "log_length"    
#define CP_KEY_TIMEFORMAT  "time_format"   
#define CP_KEY_DEVICE      "device"        


int comlog_init(const comcfg::ConfigUnit & logcfg);


int load_device(const comcfg::ConfigUnit & devcfg, com_device_t * dev);









#endif  


