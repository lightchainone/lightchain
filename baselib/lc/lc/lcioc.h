

#ifndef  __LCIOC_H_
#define  __LCIOC_H_

#ifdef __i386
#include <hash_map>
#else
#include <ext/hash_map>
#endif

#include "Lsc/containers/string/Lsc_string.h"
#include "Lsc/map.h"

#include "lclock.h"
#include "lcinlog.h"


namespace lc
{
	
	typedef Lsc::string String;

	
	class StrHashFunc {
	plclic:
		size_t operator () (const String & str) const {
			return Lsc::__Lsc_hash_string(str.c_str());
		}
	};

	template <typename Root>
		class IOC
		{
				typedef void * (*ct_t)();	
				typedef void (* cs_t)(void *);
				typedef size_t (* gs_t)();

				
				struct node_t
				{
					ct_t ct;
					cs_t cs;
					gs_t gs;
				};

				
#ifdef __i386
				typedef std::hash_map<String, node_t, StrHashFunc> HashMap;
#else
				typedef __gnu_cxx::hash_map<String, node_t, StrHashFunc> HashMap;
#endif
			plclic:
				
				Root * create (const String &name);
				
				int construct(const String &name, void *ptr);
				
				size_t getSize(const String &name);

				
				void destroy(Root *ptr);

				
				void destruct(Root *ptr);

				
				int add(const String &name, ct_t ct, cs_t cs, gs_t gs);
			private:
				MLock _lock;
				HashMap _map;
		};
};


#define LC_GEN_IOC_CREATE(ClassName)\
	void * ClassName##_ioc_create_func() { \
		return static_cast<void *> (new ClassName); \
	}\
	void ClassName##_ioc_construct_func(void *p) {\
		::new(p) ClassName;\
	}\
	size_t ClassName##_ioc_sizeof() {\
		return sizeof(ClassName);\
	}


#define LC_ADD_IOC_CLASS(ioc, ClassName)\
	ioc.add(#ClassName, ClassName##_ioc_create_func, ClassName##_ioc_construct_func, ClassName##_ioc_sizeof);\
	LC_IN_DEBUG("add %s into %s ioc", #ClassName, #ioc);

#include "lcioc.tcc"

#endif  


