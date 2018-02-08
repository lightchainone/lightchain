
#include "Configure.h"

int main(){
	comcfg :: Configure conf;
	conf.load(".", "rebuild.conf");
	printf("key: %d\n", conf["all"]["xxx"]["key"].to_int32());
	size_t sz;
	char * s = conf.dump(&sz);
	comcfg :: Configure cf2;
	char * buf = cf2.getRebuildBuffer(sz);
	memcpy(buf, s, sz);
	cf2.rebuild();
	printf("key: %d\n", cf2["all"]["xxx"]["key"].to_int32());
	return 0;
}

















