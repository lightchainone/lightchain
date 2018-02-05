
#ifndef __LC_MISCDEF_H__
#define __LC_MISCDEF_H__

#include "ul_def.h"






#define LC_MONITOR_PROVIDER "__MONITOR__"



#define EMSG_NET    "__E_NET__  "       
#define EMSG_IO     "__E_IO__  "        
#define EMSG_MEM    "__E_MEM__  "       
#define EMSG_AUTH   "__E_AUTH__  "      
#define EMSG_DATA   "__E_DATA__  "      



#ifndef _TEST
#define STATIC  static
#else
#define STATIC
#endif



#define LC_CLOSE(fd)  do { if (fd>0) { close(fd); fd=-1;} } while(0)
#define LC_FCLOSE(fd)  do { if (fd) { fclose(fd); fd=NULL;} } while(0)








typedef struct _lc_buff_t {
	char    *buf;                   
	size_t  size;                   
	size_t  used;                   
} lc_buff_t;







#endif

