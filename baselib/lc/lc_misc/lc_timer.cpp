#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "lc_string.h"
#include "lc_timer.h"


int lc_timer_init(plc_timer_t ptimer)
{
	if(ptimer == NULL){
		return -1;
	}
	else{
		ptimer->timer_cur = 0;
		return 0;
	}

	return -1;
}


int lc_timer_reset(plc_timer_t ptimer)
{
	if(ptimer == NULL)
	{
		return -1;
	}

	ptimer->timer_cur = 0;
	return 0;
}


int lc_timer_settask(plc_timer_t ptimer, const char* task, int endprev)
{
	struct timeval curtimer;
	u_int i;

	if(ptimer == NULL)
	{
		return -1;
	}

	if(NULL == task || task[0] == '\0')
		return -1;

	if(ptimer->timer_cur >= MAX_TIMERR_COUNT - 1){
		return -1;
	}

	
	if(endprev){
		lc_timer_endtask(ptimer);
	}

	
	for(i = 0; i < ptimer->timer_cur; ++i){
		if(0 == strncmp(ptimer->timername[i], task, sizeof(ptimer->timername[0])))
			break;
	}

	
	gettimeofday(&curtimer,NULL);
	memcpy(&ptimer->timevalue[i], &curtimer, sizeof(timeval));
	ptimer->mstimer[i] = 0;
	ptimer->timerstatus[i] = LC_TIMER_START;
	strncpy(ptimer->timername[i], task, sizeof(ptimer->timername[0]));
	ptimer->timername[i][sizeof(ptimer->timername[0]) - 1] = '\0';

	
	if(i == ptimer->timer_cur)
		ptimer->timer_cur++;

	return 0;
}


int lc_timer_endtask(plc_timer_t ptimer)
{
	struct timeval curtimer;
	u_int i;

	if(ptimer == NULL)
	{
		return -1;
	}

	if(ptimer->timer_cur >= MAX_TIMERR_COUNT){
		return -1;
	}

	gettimeofday(&curtimer,NULL);

	for(i = 0; i < ptimer->timer_cur; ++i){
		if(ptimer->timerstatus[i] == LC_TIMER_START){
			ptimer->timerstatus[i] = LC_TIMER_END;
			ptimer->mstimer[i] = (curtimer.tv_sec - ptimer->timevalue[i].tv_sec)*1000 +(curtimer.tv_usec - ptimer->timevalue[i].tv_usec)/1000;
		}
	}

	return 0;
}


int lc_timer_gettask(plc_timer_t ptimer, const char* task)
{
	u_int total = 0;
	u_int i;

	if(NULL == task || task[0] == '\0')
		return 0;

	if(ptimer == NULL){
		return 0;
	}

	if(ptimer->timer_cur >= MAX_TIMERR_COUNT - 1){
		return 0;
	}

	lc_timer_endtask(ptimer);

	for(i = 0; i < ptimer->timer_cur; ++i){
		if(0 == strncmp(ptimer->timername[i], task, sizeof(ptimer->timername[0])))
			break;
	}

	if(i == ptimer->timer_cur){
		
		if(0 != strncasecmp(task, TOTAL_TIMER_RES, sizeof(TOTAL_TIMER_RES)))
			return 0;
		
		for(i = 0; i < ptimer->timer_cur; ++i){
			total += ptimer->mstimer[i];
		}
		return total;
	}

	return ptimer->mstimer[i];
}


int lc_timer_gettaskstring(plc_timer_t ptimer, const char* task, char* dest, size_t dest_len)
{
	if(NULL == task || task[0] == '\0')
		return 0;
	u_int mstimer = lc_timer_gettask(ptimer, task);

	snprintf(dest, dest_len,"%ums", mstimer);
	dest[dest_len - 1] = '\0';

	return 0;
}


char* lc_timer_gettaskformat(plc_timer_t ptimer)
{
	u_int total = 0;
	char temp[MAX_LCTIMER_FORMAT_STRLEN];
	u_int i;

	if(ptimer == NULL){
		return 0;
	}

	if(ptimer->timer_cur >= MAX_TIMERR_COUNT - 1){
		return 0;
	}

	lc_timer_endtask(ptimer);
	ptimer->formatedstr[0] = '\0';

	for(i = 0; i < ptimer->timer_cur; ++i){
		total += ptimer->mstimer[i];
		snprintf(temp, sizeof(temp), "%s:%dms ", ptimer->timername[i], ptimer->mstimer[i]);
		lc_strcat(ptimer->formatedstr, temp, sizeof(ptimer->formatedstr));
	}
	snprintf(temp, sizeof(temp), TOTAL_TIMER_RES ":%dms ", total);
	lc_strcat(ptimer->formatedstr, temp, sizeof(ptimer->formatedstr));

	return ptimer->formatedstr;
}
