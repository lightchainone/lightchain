
#include "logadapter.h"
#include <assert.h>

int func(LogAdapter * p){
	p->write(UL_LOG_WARNING, "adapter test");
	p->fatal("test1");
	p->warning("test");
	p->notice("test");
	p->trace("test");
	p->debug("test");
	int a = 123;
	p->write("hawk", "This is Hawk level...%d", a);
	p->write("url", "This is URL level...%d", a);
	int level = comspace::getLogId("hawk");
	p->write(level, "This is also Hawk level....");
	return 0;
}


int main(){
	comcfg::Configure conf;
	assert(conf.load("./", "comlog.conf", "comlog.range") == 0);
	LogAdapter *p = new LogAdapter();
	assert(p->setDevice(conf["comlog"]["device"][0]) == 1);
	comspace::LogStat * ls = comspace::LogStat::createLogStat();
	uint32_t level = 0;
	COMLOG_DEFAULT_MASK(level);
	ls->setLevel(level, "url, hawk");
	assert(p->setLogStat(ls) == 0);
	func(p);
	return 0;
}















