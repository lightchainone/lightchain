
#include "appenderfactory.h"
#include "fileappender.h"
#include "asyncfileappender.h"
#include "netappender.h"
#include "ttyappender.h"
#include "ullogappender.h"

namespace comspace{

static AppenderFactory::AppenderList* static_appList=NULL;
static AppenderFactory::AppenderList* static_appTail=NULL;
//pthread_mutex_t AppenderFactory::getLock = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_t AppenderFactory::tryLock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t comlog_appenderfactory_getLock = PTHREAD_MUTEX_INITIALIZER;
//static pthread_mutex_t comlog_appenderfactory_tryLock = PTHREAD_MUTEX_INITIALIZER;

AppenderFactory::AppenderList* AppenderFactory::get_static_appList() {
	return static_appList;
}
void AppenderFactory::set_static_appList(AppenderFactory::AppenderList* al) {
	static_appList = al;
}

AppenderFactory::AppenderList* AppenderFactory::get_static_appTail() {
	return static_appTail;
}
void AppenderFactory::set_static_appTail(AppenderFactory::AppenderList* al) {
	static_appTail = al;
}

Appender* AppenderFactory :: getAppender(com_device_t &dev){
	xAutoLock __lock(&comlog_appenderfactory_getLock);
	/*
	if(dev.log_size < 1 || dev.log_size > COM_MAXLOGFILE){
		dev.log_size = COM_MAXLOGFILE;
	}
	*/
	if (dev.log_size < 1) {
		dev.log_size = COM_MAXLOGFILE;
	}
	//32下还是做限制
#ifdef __i386
    if(dev.log_size > COM_MAXLOGFILE) {
        dev.log_size = COM_MAXLOGFILE;
    }
#endif
	if(dev.cuttime < 0 || dev.cuttime > COM_MAXCUTTIME){
		dev.cuttime = COM_MAXCUTTIME;
	}
	dev.host[COM_MAXHOSTNAME-1] = 0;
	dev.file[COM_MAXFILENAME-1] = 0;
	dev.auth[COM_MODULENAMESIZE-1] = 0;
	dev.type[COM_MAXAPPENDERNAME-1] = 0;
	dev.reserved1[COM_RESERVEDSIZE-1] = 0;
	dev.reserved2[COM_RESERVEDSIZE-1] = 0;
	dev.reserved3[COM_RESERVEDSIZE-1] = 0;
	dev.reserved4[COM_RESERVEDSIZE-1] = 0;

	AppenderList * apdl = searchAppender(dev.type);
	if(NULL == apdl) return NULL;
	Appender* app = apdl->getAppender(dev);	
	if (app != NULL) {
		app->setLayout(dev.layout);
		_core_debug("dev =%lx appender layout=%lx", dev.layout, app->_layout);
		app->_open = dev.open;
	}
	return app;
}

Appender* AppenderFactory :: tryAppender(com_device_t &dev){
	xAutoLock __lock(&comlog_appenderfactory_getLock);
	AppenderList * app = searchAppender(dev.type);
	if(NULL == app) return NULL;
	return app->tryAppender(dev);
}

int AppenderFactory :: registAppender(const char *appenderName, func_type getAppender, func_type tryAppender){
	if(appenderName == NULL) return -1;
	if(strlen(appenderName) >= MAX_NAME_LEN) return -1;
	if(NULL != searchAppender(appenderName)) return -1;
	AppenderList* app = (AppenderList* )malloc(sizeof(AppenderList));
	if(NULL == app) return -1;
	snprintf(app->name, MAX_NAME_LEN, "%s", appenderName);
	app->getAppender = getAppender;
	app->tryAppender = tryAppender;
	insertAppender(app);
	return 0;

}

bool AppenderFactory :: isAppender(const char *appenderName){
	if(appenderName == NULL) return false;
	if(strlen(appenderName) >= MAX_NAME_LEN) return false;
	return NULL != searchAppender(appenderName);
}

AppenderFactory :: AppenderFactory(){
	registAppender("FILE", FileAppender::getAppender, FileAppender::tryAppender);
	registAppender("AFILE", AsyncFileAppender::getAppender, AsyncFileAppender::tryAppender);
	registAppender("NET", NetAppender::getAppender, NetAppender::tryAppender);
	registAppender("TTY", TtyAppender::getAppender, TtyAppender::tryAppender);
	registAppender("ULLOG", UlLogAppender::getAppender, UlLogAppender::tryAppender);
	registAppender("ULNET", UlLogAppender::getAppender, UlLogAppender::tryAppender);
}

AppenderFactory :: ~AppenderFactory(){
	AppenderList* cur;
	AppenderList* tmp;
	cur = get_static_appList();
	while(cur){
		tmp = cur->next;
		free(cur);
		cur = tmp;
	}
//	appList = NULL;
	set_static_appList(NULL);
}


AppenderFactory::AppenderList* AppenderFactory :: searchAppender(const char* appenderName){
	AppenderList* appNode = get_static_appList();
	while(appNode != NULL){
		if(strcmp(appenderName, appNode->name) == 0){
			return appNode;
		}
		appNode = appNode->next;
	}
	return NULL;
}

void AppenderFactory :: insertAppender(AppenderList* node){
	if(NULL == get_static_appList()){
//		appList = node;
		set_static_appList(node);
		get_static_appList()->next = NULL;
		set_static_appTail(get_static_appList());
	}
	else{
		get_static_appTail()->next = node;
		node->next = NULL;
//		appTail = node;
		set_static_appTail(node);
	}
};
}
















/* vim: set ts=4 sw=4 sts=4 tw=100 */
