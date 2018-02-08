
#include "Configure.h"

#define ASSERT(x) do{\
	if(!(x))printf("Assert failed %s:%d\n", __FILE__, __LINE__);\
	else printf("Assertion Successful. %s:%d\n", __FILE__, __LINE__);\
}while(0)


int main(){
	comcfg::Configure  conf;
	int ret = conf.load(".", "t1.conf", "t1.cons");
	ASSERT(ret == comcfg::CONSTRAINT_ERROR);
	system("touch t1.conf -d '19870121 12:35'");
	
	
	time_t t = conf.lastConfigModify();
	ASSERT(t == time_t(538202100) );
	return 0;
}




















