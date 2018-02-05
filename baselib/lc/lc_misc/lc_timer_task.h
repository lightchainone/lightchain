

#ifndef  __LC_TIMER_TASK_H_
#define  __LC_TIMER_TASK_H_


struct lc_timer_task_t;


lc_timer_task_t * lc_create_timer_task();

void lc_destroy_timer_task(lc_timer_task_t *);

int lc_run_timer_task(lc_timer_task_t *t, int num=1);

int lc_join_timer_task(lc_timer_task_t *t);

int lc_stop_timer_task(lc_timer_task_t *t);

int lc_add_timer_task(lc_timer_task_t *t, int (*)(void *), void *param, int msec);

#endif  


