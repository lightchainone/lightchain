
#ifndef __LC_PROCCMD_H__
#define __LC_PROCCMD_H__

#include "ul_def.h"
#include "nshead.h"
#include "lc_log.h"


typedef int (*proc_cmd_callback_t) (int, nshead_t*, lc_buff_t*, nshead_t*, lc_buff_t*);



typedef struct _lc_cmd_map_t {
	int                  cmd_no;            
	proc_cmd_callback_t  proc;              
} lc_cmd_map_t;


static inline int lc_process_cmdmap(const lc_cmd_map_t *cmd_map, int cmd_no, nshead_t *req_head, lc_buff_t *req_buf,
		nshead_t *res_head, lc_buff_t *res_buf)
{
	

	while (cmd_map->proc) {
		if (cmd_map->cmd_no==cmd_no) {
			return cmd_map->proc(cmd_no, req_head, req_buf, res_head, res_buf);
		}
		cmd_map++;
	}
	LC_LOG_WARNING("unsupport [cmd_no:%d] received!", cmd_no);

	return -1;
}

#endif

