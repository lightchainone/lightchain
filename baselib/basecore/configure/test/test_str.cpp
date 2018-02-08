
#include "Configure.h"
#include <iostream>

int main(){
	comcfg :: Configure conf;
	char buf[128];
	conf.load(".", "str.conf");

	std::cout<<"str1 : to raw : "<<conf["str1"].to_raw_string()<<std::endl;
	std::cout<<"str1 : to std : "<<conf["str1"].to_Lsc_string()<<std::endl;
	conf["str1"].get_cstr(buf, 128);
	std::cout<<"str1 : to cstr: "<<buf<<std::endl;


	std::cout<<"str2 : to raw : "<<conf["str2"].to_raw_string()<<std::endl;
	std::cout<<"str2 : to std : "<<conf["str2"].to_Lsc_string()<<std::endl;
	conf["str2"].get_cstr(buf, 16);
	std::cout<<"str2 : to cstr: "<<buf<<std::endl;

	return 0;
}


















