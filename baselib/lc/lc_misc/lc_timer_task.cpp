
#include "lc_timer_task.h"
#include "../lc/lctimer.h"
#include "../lc/lcevent.h"

struct lc_timer_task_t
{
	lc::Timer timer;
};



lc_timer_task_t * lc_create_timer_task()
{
	try {
		return new lc_timer_task_t;
	} catch (...) {
		return NULL;
	}
}

void lc_destroy_timer_task(lc_timer_task_t *t)
{
	if (t) {
		delete t;
	}
}

int lc_run_timer_task(lc_timer_task_t *t, int num)
{
	if (t) {
		t->timer.setThread(num);
		return t->timer.run();
	}
	return -1;
}

int lc_join_timer_task(lc_timer_task_t *t)
{
	if (t) {
		return t->timer.join();
	}
	return -1;
}

namespace
{
	struct lc_ttb_t
	{
		int msec;
		void *p;
		int (*cb)(void *);
	};
};

static void timer_callback(lc::IEvent *ev, void *p)
{
	lc_ttb_t *b = (lc_ttb_t *)(p);
	if (b->cb(b->p) == 0) {
		ev->setTimeout(b->msec);
		ev->reactor()->post(ev);
	}
}

int lc_stop_timer_task(lc_timer_task_t *t)
{
	if (t) {
		return t->timer.stop();
	} else {
		return -1;
	}
}

int lc_add_timer_task(lc_timer_task_t *t, int (*callback)(void *),
		void *p, int msec)
{
	if (t == NULL || callback == NULL) {
		return -1;
	}
	lc::EventPtr ev;
	if (ev.ev() == NULL) { 
		return -1; 
	}
	if (ev->setEventBuffer(sizeof(lc_ttb_t)) != 0) {
		return -1;
	}
	lc_ttb_t *e = static_cast<lc_ttb_t *>(ev->getEventBuffer());
	e->msec = msec;
	e->cb = callback;
	e->p = p;
	ev->setTimeout(msec);
	ev->setCallback(timer_callback, e);
	return t->timer.post(ev.ev());
}


