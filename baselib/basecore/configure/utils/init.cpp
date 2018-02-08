
#include "cc_log.h"
#include "ul_log.h"

namespace comcfg{
	const int NO_LOG = 0x7fff;
	int Log::_fatal = UL_LOG_FATAL;
	int Log::_warning = UL_LOG_WARNING;
	int Log::_notice = NO_LOG;
	int Log::_trace = NO_LOG;
	int Log::_debug = NO_LOG;
}




















