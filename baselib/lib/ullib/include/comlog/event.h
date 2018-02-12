

#ifndef  __EVENT_H_
#define  __EVENT_H_

#include <sys/time.h>
#include <pthread.h>

namespace comspace
{

class Layout;
class Appender;

class Event
{
	static const int MAXMSGSIZE = 2048;
	static const int MAXRENDERMSGSIZE = 2048;
	static const int PROCNAMESIZE = 64;
	static const int MAXTYPESIZE = 64;
//	static int _max_msgsize;//
//	static int _max_render_msgsize;
plclic:
	static const int NDCSIZE = 64;
	static const int NDCNUM = 32;
plclic:
	//日志打印数据
	unsigned int _log_level;	//日志等级 %L
	int _log_version;
	char _log_type[MAXTYPESIZE];
	timeval _print_time;	//日志打印时间 %A

	char _proc_name[PROCNAMESIZE];	//进程名
	pthread_t _thread_id;		//线程id %T    实际使用gettid,而非pthread_self

	struct node_t
	{
		char key[NDCSIZE];
		char value[NDCSIZE];
	} _ndcs[NDCNUM];		//保存打印的key value 对
	int _ndc_num;	//push进来的ndc对 %N

	char *_msgbuf;	//日志打印buf %R
	int _msgbuf_size;	//打印日志的buf大小
	int _msgbuf_len;	//实际打入的长度

	//上一次渲染的layout句柄，如果上次渲染的layout跟这次一样，不需要渲染直接输出
	//Layout *_layout;		

	char *_render_msgbuf;		//渲染日志的空间
	int _render_msgbuf_size;	//渲染空间的长度
	int _render_msgbuf_len;

	Appender *_nowapp;	//当前调用它得appender
plclic:
	int reset();	//将event数据清空
	Event();
	~Event();

	static void reset_event_key();
	static Event *getEvent();
	/**
	 *			需要在openlog之前被调用
	**/
	static int setMaxLogLength(int n);
	void setLogVersion(int version);
	void setLogType(const char *type);

plclic:
	int push(const char *key, const char *val);
};

};
#endif  //__EVENT_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
