

#ifndef  __MC_PACK_DEF_H_
#define  __MC_PACK_DEF_H_

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif
    typedef char mc_int8_t;
    typedef short mc_int16_t;
    typedef int mc_int32_t;
    typedef long long mc_int64_t;

    typedef unsigned char mc_uint8_t;
    typedef unsigned short mc_uint16_t;
    typedef unsigned int mc_uint32_t;

    typedef unsigned long long mc_uint64_t;

#ifndef __cplusplus
    typedef char mc_bool_t;
#define MC_TRUE 1
#define MC_FALSE 0
#else
    typedef bool mc_bool_t;
#define MC_TRUE true
#define MC_FALSE false
#endif

    enum mc_pack_version {
        VERSION_1 = 1,
        VERSION_2 = 2
    };

    enum mc_pack_error_t {
        MC_PE_SUCCESS = 0,			  
        MC_PE_NOT_FOUND = -1,		  
        MC_PE_BAD_PARAM = -2,		  
        MC_PE_BAD_NAME = -3,		  
        MC_PE_BAD_TYPE = -4,		  
        MC_PE_NO_SPACE = -5,		  
        MC_PE_NAME_TOO_LONG = -6,	  
        MC_PE_ALREADY_EXIST = -7,	  
        MC_PE_BAD_DATA = -8,		  
        MC_PE_NULL_POINTER = -9,	  
        MC_PE_FOUND_BUT_TYPE = -10,	  
        MC_PE_NO_TEMP_SPACE = -11,    
        MC_PE_WRONG_OPEN_MODE = -12,   
        MC_PE_WRONG_PACK_TYPE = -13,  
        MC_PE_PROTOCOL_NOT_SUPPOT_OPERATION = -14,  
        MC_PE_SYSTEM_ERROR = -15,  
        MC_PE_NULL_VALUE = -16,    
        MC_PE_UNKNOWN = -255		  
    };

    enum mc_v1pack_type_t {
        
        MC_PT_BAD = 0x00,		  
        MC_PT_PCK = 0x01,		  
        MC_PT_OBJ = 0x02,		  
        MC_PT_ARR = 0x04,		  

        
        MC_IT_UNKNOWN = 0x05,		  
        MC_IT_BAD = 0x00,		  
        MC_IT_BIN = 0x10,		  
        MC_IT_TXT = 0x20,		  

        MC_IT_SGN = 0x11,		  
        MC_IT_UNS = 0x12,		  
        MC_IT_32B = 0x14,		  
        MC_IT_64B = 0x18,		  

        MC_IT_BOOL = 0x30,       
        MC_IT_NULL = 0x40,       
        MC_IT_FLOAT = 0x50,      
        MC_IT_DOLCLE = 0x51,     

        MC_IT_I32 = MC_IT_SGN | MC_IT_32B,		  
        MC_IT_U32 = MC_IT_UNS | MC_IT_32B,		  
        MC_IT_I64 = MC_IT_SGN | MC_IT_64B,		  
        MC_IT_U64 = MC_IT_UNS | MC_IT_64B		  
    };

    enum mc_pack_type_id {
        MCPACKV2_INVALID = 0x0,
        MCPACKV2_OBJECT = 0x10,
        MCPACKV2_ARRAY = 0x20,
        MCPACKV2_STRING = 0x50,
        MCPACKV2_RAW = 0x60,
        MCPACKV2_INT_8 = 0x11,
        MCPACKV2_INT_16 = 0x12,
        MCPACKV2_INT_32 = 0x14,
        MCPACKV2_INT_64 = 0x18,
        MCPACKV2_UINT_8 = 0x21,
        MCPACKV2_UINT_16 = 0x22,
        MCPACKV2_UINT_32 = 0x24,
        MCPACKV2_UINT_64 = 0x28,
        MCPACKV2_BOOL = 0x31,
        MCPACKV2_FLOAT = 0x44,
        MCPACKV2_DOLCLE = 0x48,
        MCPACKV2_DATE = 0x58,
        MCPACKV2_NULL = 0x61,
        MCPACKV2_SHORT_ITEM = 0x80,
        MCPACKV2_FIXED_ITEM = 0xf,
        MCPACKV2_DELETED_ITEM = 0x70
    };

    typedef struct {} mc_pack_t;
    typedef struct {
        const mc_pack_t * pack; 
        char * internal;      
        int type;			  
        size_t value_size;	  
        const char *key;			  
        const char *value;		  
    } mc_pack_item_t;

    typedef struct {                
        char *buf;                  
        int buflen;                 
        int bufsize;                
    } full_key_t;


#define MC_PACK_OBJDELIM ','
#define MC_PACK_ARRDELIM ':'
#define MC_PACK_FIELD_NAME_LEN_MAX (1024)




#ifdef __cplusplus
}
#endif









#endif  


