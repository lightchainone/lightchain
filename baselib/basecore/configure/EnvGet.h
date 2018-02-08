


#ifndef  __ENVGET_H_
#define  __ENVGET_H_

#include "utils/cc_struct.h"
#define ENV_CHAR_DECL(envstr, deft) \
	static const char g_char_##envstr = (NULL == getenv(#envstr))? \
		deft : getenv(#envstr)[0]
#define ENV_UCHAR_DECL(envstr, deft) \
	static const unsigned char g_uchar_##envstr = (NULL == getenv(#envstr))? \
		deft : (unsigned char)getenv(#envstr)[0]

#define ENV_INT16_DECL(envstr, deft) \
	static const int16_t g_int16_##envstr = (NULL == getenv(#envstr))? \
		deft : (int16_t)strtoll(getenv(#envstr), NULL, 0)
#define ENV_UINT16_DECL(envstr, deft) \
	static const u_int16_t g_uint16_##envstr = (NULL == getenv(#envstr))? \
		deft : (u_int16_t)strtoll(getenv(#envstr), NULL, 0)

#define ENV_INT32_DECL(envstr, deft) \
	static const int g_int32_##envstr = (NULL == getenv(#envstr))? \
		deft : (int)strtoll(getenv(#envstr), NULL, 0)

#define ENV_UINT32_DECL(envstr, deft) \
	static const u_int32_t g_uint32_##envstr = (NULL == getenv(#envstr))? \
		deft : (u_int32_t)strtoll(getenv(#envstr), NULL, 0)

#define ENV_INT64_DECL(envstr, deft) \
	static const long long g_int64_##envstr = (NULL == getenv(#envstr))? \
		deft : (long long)strtoll(getenv(#envstr), NULL, 0)
#define ENV_UINT64_DECL(envstr, deft) \
	static const unsigned long long g_uint64_##envstr = (NULL == getenv(#envstr))? \
		deft : (unsigned long long)strtoull(getenv(#envstr), NULL, 0)

#define ENV_FLOAT_DECL(envstr, deft) \
	static const float g_float_##envstr = (NULL == getenv(#envstr))? \
		deft : (float)strtod(getenv(#envstr), NULL)
#define ENV_DOLCLE_DECL(envstr, deft) \
	static const float g_dolcle_##envstr = (NULL == getenv(#envstr))? \
		deft : (dolcle)strtod(getenv(#envstr), NULL)

#define ENV_BSLSTR_DECL(envstr, deft) \
	static const Lsc_string g_Lscstr_##envstr = (NULL == getenv(#envstr))? \
		deft : Lsc_string(getenv(#envstr))








#endif  


