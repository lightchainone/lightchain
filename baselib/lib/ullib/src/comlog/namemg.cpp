
#include "namemg.h"
#include "xutils.h"
#include "category.h"
#include "layout.h"
#include "appender/appender.h"
#include "sendsvr/sendsvr.h"
#include "logstat.h"

//category tag
#define CG_TAG "CG_"
//appender tag
#define AP_TAG "AP_"
//layout tag
#define LA_TAG "LA_"
#define SVR_TAG "SVR_"
#define LS_TAG "LS_"


namespace comspace
{
void NameMg :: __delete_node__(const NameMg::node_t &t, void *)
{
	switch (t.type) {
		case type_category:
			delete (Category *)(t.ptr);
			break;
		case type_appender:
			delete (Appender *)(t.ptr);
			break;
		case type_layout:
			delete (Layout *)(t.ptr);
			break;
		case type_sendsvr:
			//delete (SendSvr *)(t.ptr);
			break;
		case type_logstat:
			delete (LogStat *)(t.ptr);
			break;
		default:
			break;
	}
}

void NameMg :: __delete_sendsvr__(const NameMg::node_t &t, void *){
	if(t.type == type_sendsvr){
		delete (SendSvr *)(t.ptr);
	}
}

void NameMg :: __close_appender__(const NameMg::node_t &t, void *){
	if(t.type == type_appender){
		((Appender *)(t.ptr))->stop();
	}
}

void NameMg :: __reset_appender__(const NameMg::node_t &t, void *){
	if(t.type == type_appender){
		((Appender *)(t.ptr))->reset();
	}
}

unsigned int nkeyhash(const unsigned long long &key)
{
	unsigned int *p = (unsigned int *)&key;
	return p[0]+p[1];
}

int NameMg::create()
{
	pthread_mutex_init(&_lock, NULL);
	int ret = _hash.create(10241, nkeyhash);
	return ret;
}

int NameMg::closeAppender(){
	_hash.list(__close_appender__, NULL);
	return 0;
}

int NameMg::resetAppender(){
	_hash.list(__reset_appender__, NULL);
	return 0;
}

int NameMg::destroy()
{
	_hash.list(__delete_sendsvr__, NULL);
	_hash.list(__delete_node__, NULL);
	//便利数据以后delete
	pthread_mutex_destroy(&_lock);
	_hash.clear();
	return 0;
}

unsigned long long NameMg::getKey(int tag, const char *name)
{
	char buf[2048];
	int ret = snprintf(buf, sizeof(buf), "%d:%s", tag, name);
	if (ret >= (int)sizeof(buf)) {
		ret = sizeof(buf)-1;
	}
	unsigned long long key;
	unsigned int *p = (unsigned int *)&key;
	x_creat_sign_f64(buf, ret, p, p+1);
	return key;
}

void *NameMg::get(int tag, const char *name)
{
	node_t node;
	int ret = _hash.get(getKey(tag, name), &node);
	if (ret != 0) return NULL;
	return node.ptr;
}

int NameMg::set(const char *name, Appender *app)
{
	return set(type_appender, name, app);
}

#if 0
int NameMg::get(const char *name, Appender * &app)
{
	return _hash.get(getKey(type_appender, name), app);
}
#endif

int NameMg::set(const char *name, SendSvr *svr)
{
	return set(type_sendsvr, name, svr);
}

#if 0
int NameMg::get(const char *name, SendSvr * &svr)
{
	return _hash.get(getKey(type_sendsvr, name), svr);
}
#endif

int NameMg::set(const char *name, LogStat *logstat)
{
	return set(type_logstat, name, logstat);
}

#if 0
int NameMg::get(const char *name, LogStat * &logstat)
{
	return _hash.get(getKey(type_logstat, name), logstat);
}
#endif

int NameMg::set(const char *name, Category *cate)
{
	return set(type_category, name, cate);
}

#if 0
int NameMg::get(const char *name, Category * &cate)
{
	return _hash.get(type_category, name, cate);
}
#endif

int NameMg::set(const char *name, Layout *layout)
{
	return set(type_layout, name, layout);
}

#if 0
int NameMg::get(const char *name, Layout * &layout)
{
	return _hash.get(type_layout, name, layout);
}
#endif

//
int NameMg::set(int tag, const char *name, void *val)
{
	xAutoLock lock(&_lock);
	node_t node;
	node.ptr = val;
	node.type = tag;
	int ret =  _hash.set(getKey(tag, name), node);
	return ret;
}

//NameMg g_nameMg;
static NameMg * g_nameMg = NULL;
NameMg * comlog_get_nameMg() {
	if (NULL == g_nameMg) {
		g_nameMg = new NameMg();
	}
	return g_nameMg;
}
void comlog_del_nameMg() {
	if (g_nameMg) {
		delete g_nameMg;
		g_nameMg = NULL;
	}
}


}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
