#ifndef __LC_TIMER_H__
#define __LC_TIMER_H__


#define MAX_TIMERR_COUNT   64

#define MAX_TIMERR_NAMELEN 32

#define MAX_LCTIMER_FORMAT_STRLEN 48

#define TOTAL_TIMER_RES "lc_total"


enum lc_timer_status{
	LC_TIMER_START,		
	LC_TIMER_END			
};



typedef struct _lc_timer_t{
	u_int			timer_cur;										
	struct			timeval timevalue[MAX_TIMERR_COUNT];				
	int				mstimer[MAX_TIMERR_COUNT];						
	lc_timer_status	timerstatus[MAX_TIMERR_COUNT];					
	char			timername[MAX_TIMERR_COUNT][MAX_TIMERR_NAMELEN];	
	char			formatedstr[MAX_TIMERR_COUNT * MAX_LCTIMER_FORMAT_STRLEN];	
}lc_timer_t;


typedef struct _lc_timer_t	*plc_timer_t;


int lc_timer_init(plc_timer_t timer);



int lc_timer_reset(plc_timer_t lc_timer);



int lc_timer_settask(plc_timer_t lc_timer, const char* task, int endprev = 1);



int lc_timer_endtask(plc_timer_t lc_timer);



int lc_timer_gettaskstring(plc_timer_t lc_timer, const char* task, char* dest, size_t dest_len);



int lc_timer_gettask(plc_timer_t lc_timer, const char* task);



char* lc_timer_gettaskformat(plc_timer_t lc_timer);

#endif 




