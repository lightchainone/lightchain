
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <bits/local_lim.h>
#include "event.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

#ifndef __NR_gettid
#define	__NR_gettid 	224
#endif

#define gettid() syscall(__NR_gettid)

namespace comspace
{

//int Event::_max_msgsize = 2048;
//int Event::_max_render_msgsize = 2048;
static int comlog_event_max_msgsize = 2048;
static int comlog_event_max_render_msgsize = 2048;

int Event::push(const char *key, const char *val)
{
	if (_ndc_num >= NDCNUM) return -1;
	strncpy(_ndcs[_ndc_num].key, key, sizeof(_ndcs[_ndc_num].key));
	strncpy(_ndcs[_ndc_num].value, val, sizeof(_ndcs[_ndc_num].value));
	_ndcs[_ndc_num].key[sizeof(_ndcs[_ndc_num].key)-1] = 0;
	_ndcs[_ndc_num].value[sizeof(_ndcs[_ndc_num].value)-1] = 0;
	++ _ndc_num;
	return 0;
}

int Event::setMaxLogLength(int n){
	if(n <= 8){
		comlog_event_max_msgsize = MAXMSGSIZE;
		comlog_event_max_render_msgsize = comlog_event_max_msgsize;
	}
	else{
		comlog_event_max_msgsize = n;
		comlog_event_max_render_msgsize = n;
	}
	return comlog_event_max_msgsize;
}

void Event::setLogVersion(int version)
{
	_log_version = version;
}

void Event::setLogType(const char *type)
{
	strncpy(_log_type, type, MAXTYPESIZE);
}

Event::Event()
{
	//_msgbuf_size =  MAXMSGSIZE;
	//_render_msgbuf_size = MAXRENDERMSGSIZE;
	_msgbuf_size = comlog_event_max_msgsize;
	_render_msgbuf_size = comlog_event_max_render_msgsize;

	_msgbuf = new char[_msgbuf_size];
	_render_msgbuf = new char[_render_msgbuf_size];

	reset();
}

Event::~Event()
{
#if 1
	delete[] _msgbuf;
	delete[] _render_msgbuf;
#endif
}
int Event::reset()
{
	_ndc_num = 0;
	_msgbuf[0] = 0;
	_msgbuf_len = 0;
	_render_msgbuf[0] = 0;
	_render_msgbuf_len = 0;
	_nowapp = NULL;
	//_layout = NULL;
	_log_version = -1;
	_log_type[0] = 0;
	return 0;
}

static pthread_key_t g_event_key = PTHREAD_KEYS_MAX;
static pthread_once_t g_event_ronce = PTHREAD_ONCE_INIT;
pthread_key_t * event_key(){
	return &g_event_key;
}
static void event_destroy(void *ptr)
{
	if(ptr){
		delete (Event *)ptr;
	}
}
static void event_runonce()
{
	pthread_key_create(&g_event_key, event_destroy);
}

void Event::reset_event_key()
{
	pthread_setspecific(g_event_key, NULL);
	pthread_key_delete(g_event_key);
	event_runonce();
}

Event * Event::getEvent()
{
	pthread_once(&g_event_ronce, event_runonce);
	Event *ptr = (Event *)pthread_getspecific(g_event_key);
	if (ptr == NULL) {
		ptr = new Event;
//		ptr->_thread_id = pthread_self();
		ptr->_thread_id = gettid();
		pthread_setspecific(g_event_key, ptr);
	}
	return ptr;
}

}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
