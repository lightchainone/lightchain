
#include "Configure.h"
#include <iostream>

int main(){
	comcfg :: Configure conf;
	conf.load(".", "str.conf");
	int err;
	conf["xxx"].to_int32(&err);
	std::cout<<conf.seeError(err)<<std::endl;
	conf["str1"].to_int32(&err);
	std::cout<<conf.seeError(err)<<std::endl;
	return 0;
}

















