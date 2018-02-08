

#ifndef  __CC_STRUCT_H_
#define  __CC_STRUCT_H_

#include <stdlib.h>

#include <string>
#include <list>
#include <map>
#include "Lsc/containers/string/Lsc_string.h"

namespace comcfg{


	
	typedef unsigned char comcfg_type_uchar;		  
	typedef signed short comcfg_type_int16;		  
	typedef unsigned short comcfg_type_uint16;		  
	typedef signed int comcfg_type_int32;		  
	typedef unsigned int comcfg_type_uint32;		  
	typedef signed long long comcfg_type_int64;		  
	typedef unsigned long long comcfg_type_uint64;		  
	typedef std::string std_string;		  
	typedef Lsc::string Lsc_string;		  
#define CC_CHAR "char"		  
#define CC_UCHAR "uchar"		  
#define CC_INT16 "int16_t"		  
#define CC_UINT16 "uint16_t"		  
#define CC_INT32 "int32_t"		  
#define CC_UINT32 "uint32_t"		  
#define CC_INT64 "int64_t"		  
#define CC_UINT64 "uint64_t"		  
#define CC_FLOAT "float"		  
#define CC_DOLCLE "dolcle"		  
#define CC_STRING "string"		  
	typedef Lsc::string str_t;		  
	typedef int ErrCode;		  

}



#endif  


