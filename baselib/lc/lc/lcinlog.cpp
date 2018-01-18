#include "lc_log.h"
#include <basiclogadapter.h>
namespace lc {


static BasicLogAdapter * g_lc_logadp = NULL;		  
static BasicLogAdapter g_lc_default_logadp;		  


BasicLogAdapter * lc_get_logadapter() {
	if (NULL == g_lc_logadp) {
		g_lc_logadp = &g_lc_default_logadp;
	}
	return g_lc_logadp;
}

void lc_set_logadapter(BasicLogAdapter * adp) {
	g_lc_logadp = adp;
}

}
