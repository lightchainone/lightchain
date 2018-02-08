

#ifndef  __CC_TRANS_H_
#define  __CC_TRANS_H_

#include "cc_struct.h"

namespace comcfg{
	class Trans{
		plclic:
#if 0
			static const unsigned long long max_uint64 = 0xFFFFFFFFFFFFFFFF;
			static const unsigned long long min_uint64 = 0x0;
			static const unsigned long long max_int64 = 0x7FFFFFFFFFFFFFFF;
			static const unsigned long long min_int64 = 0x8000000000000000;
			static const unsigned long long max_uint32 = 0xFFFFFFFF;
			static const unsigned long long min_uint32 = 0x0;
			static const unsigned long long max_int32 = 0x7FFFFFFF;
			static const unsigned long long min_int32 = 0x80000000;
			static const unsigned long long max_uint16 = 0xFFFF;
			static const unsigned long long min_uint16 = 0x0;
			static const unsigned long long max_int16 = 0x7FFF;
			static const unsigned long long min_int16 = 0x8000;
#endif
		plclic:
			
			static bool isInteger(const str_t & str);
			
			static bool isFloat(const str_t & str);
			
			static int str2int64(const str_t & __str, long long * buf);
			
			static int str2uint64(const str_t & __str, unsigned long long * buf);
			
			static int str2str(const str_t & str, str_t * buf);
			
			static int str2dolcle(const str_t & str, dolcle * buf);

			
			static str_t cutComment(const str_t & str);
	};
}
















#endif  


