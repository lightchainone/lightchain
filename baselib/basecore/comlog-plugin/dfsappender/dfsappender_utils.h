



#ifndef  __DFSAPPENDER_UTILS_H_
#define  __DFSAPPENDER_UTILS_H_

#include "comlogproxy_if.h"
#include <comlog/comlog.h>

namespace comspace {
namespace comlogplugin {
namespace dfsappender {



int normalize_file(char * filename);


int handle_sign(char *filename, comlogproxy_handle_t * sign);



int check_slcdir(char *path);



const char* get_proc_name();


const char* get_proc_path();


int get_dev_confnstr(com_device_t *dev, const char* key, char *value, size_t value_size);

int get_dev_confuint32(com_device_t *dev, const char* key, uint32_t *value);

}
}
}

#endif  


