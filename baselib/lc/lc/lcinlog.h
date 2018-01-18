


#ifndef  __LCINLOG_H_
#define  __LCINLOG_H_
#include "lc_log.h"
#include <basiclogadapter.h>


namespace lc
{
	


	
	BasicLogAdapter * lc_get_logadapter();
	
	void lc_set_logadapter(BasicLogAdapter * adp);
};

#define LC_IN_FATAL(fmt, arg...) \
do { \
	::lc::lc_get_logadapter()->write(COMLOG_FATAL, "[%s:%d]"fmt, __FILE__, __LINE__, ##arg); \
} while (0)

#define LC_IN_WARNING(fmt, arg...) \
do { \
	::lc::lc_get_logadapter()->write(COMLOG_WARNING, "[%s:%d]"fmt, __FILE__, __LINE__, ##arg); \
} while (0)

#define LC_IN_NOTICE(fmt, arg...) \
do { \
	::lc::lc_get_logadapter()->write(COMLOG_NOTICE, fmt, ##arg); \
} while (0)

#ifdef LC_NO_DEBUG
#define LC_IN_TRACE(fmt, arg...) {;}
#else
#define LC_IN_TRACE(fmt, arg...) \
do { \
	::lc::lc_get_logadapter()->write(COMLOG_TRACE, "[%s:%d]"fmt, __FILE__, __LINE__, ##arg); \
} while (0)
#endif

#ifdef LC_NO_DEBUG
#define LC_IN_DEBUG(fmt, arg...) {;}
#else
#define LC_IN_DEBUG(fmt, arg...) \
do { \
	::lc::lc_get_logadapter()->write(COMLOG_DEBUG, "[%s:%d]"fmt, __FILE__, __LINE__, ##arg); \
} while (0)
#endif




#define LC_IN_OPENLOG_R { \
	lc_log_initthread("lctask");\
}
#define LC_IN_CLOSELOG_R { \
	lc_log_closethread(); \
}

#define LC_IN_ASSERT(op, fmt, arg...) \
	do { \
		if (!(op)) {\
			::lc::lc_get_logadapter()->write(COMLOG_FATAL, "[%s:%d][%s]"fmt, __FILE__, __LINE__, #op, ##arg); \
			exit(-1);\
		}\
	} while (0)

#endif  


