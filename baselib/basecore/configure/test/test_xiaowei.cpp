#include "Configure.h"
#include <iostream>
int main(){
	
	comcfg :: Configure conf;
	conf.load(".", "xiaowei.conf", NULL);
	conf.print();
	std::cout<<"conf size = "<<conf.size()<<std::endl;

	for(int i = 0; i < 3; i++){
		std::cout<<conf["server"][i]["ip"].to_Lsc_string()<<std::endl;
		std::cout<<conf["server"][i]["port"].to_Lsc_string()<<std::endl;
	}
	try{
		std::cout<<"error "<<conf["server"].to_int32()<<std::endl;
	}catch(comcfg :: NoSuchKeyException){
		std::cout<<"-------catch NoSuchKey---------"<<std::endl;
	}
	return 0;
}





















