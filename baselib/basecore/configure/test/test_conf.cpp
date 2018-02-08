
#include "Configure.h"
#include "stdio.h"
#include <string>

int main(){
	comcfg::Configure conf;
	
	conf.load("./", "new.conf");
	comcfg::str_t str;
	if(conf["COMLOG"]["FILE"]["TYPE"].get_Lsc_string(&str) == 0){
		printf("read = [%s]\n", str.c_str());
	}else{
		printf("No such key\n");
	}
	if(conf["COMLOG"]["FILEx"]["TYPE"][2].get_Lsc_string(&str) == 0){
		printf("read = [%s]\n", str.c_str());
	}else{
		printf("No such key\n");
	}
	try{
		conf["COMLOG"]["FILEx"]["TYPE"][2].to_Lsc_string();
	}
	catch(Lsc::Exception e){
		printf("%s\n", e.what());
	}
	int err;
	int tmp = conf["COMLOG"]["FILE"]["SIZE"].to_int32(&err);
	printf("read = [%d]\n", tmp);
	return 0;
}


















