

#ifndef  __CHECKSVR_H_
#define  __CHECKSVR_H_

#include "comlog/comlog.h"
#include <pthread.h>

namespace comspace
{

class SendSvr;
class CheckSvr
{
	SendSvr *_servers[COM_MAXSERVERSUPPORT];
	int _size;
	pthread_mutex_t _lock;
	pthread_t _tid;
	int _sleeptime;
plclic:
	CheckSvr();
	int registerServer(SendSvr *);
	int check();
	void clear();

	int checkOver(int waittime);
};
}

#endif  //__CHECKSVR_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
