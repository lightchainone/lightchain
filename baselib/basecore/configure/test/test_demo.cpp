
#include "Configure.h"
#include <iostream>

int main(){
	
	comcfg :: Configure conf;
	int ret = conf.load(".", "test.conf");
	if(ret != 0){
		return 0;
	}

	try{
		std::cout<< "Module Name : " << conf["module"].to_Lsc_string() << std::endl;
		size_t num = conf["server"].size();
		std::cout<< "We have " << num << " servers." << std::endl;
		for(size_t i = 0; i < num; ++i){
			printf("Server[%d] is : ip = %s, port = %d\n", 
					(int)i,
				conf["server"][i]["ip"].to_cstr(), 
				conf["server"][i]["port"].to_int32() );
		}
	}
	catch(comcfg :: ConfigException e){
		std::cerr << "Something error  "<< e.what() << std::endl; 
	}
	catch(...){
		std::cerr << "What's wrong?" << std::endl;
	}
	size_t sz;
	printf("\n-------dump-------\n%s\n--------print---------\n", conf.dump(&sz));
	conf.print();
	comcfg::Configure nxt;
	char * b = nxt.getRebuildBuffer(sz);
	memcpy(b, conf.dump(NULL), sz);
	nxt.rebuild();
	printf("----nxt.print----\n");
	nxt.print();
	return 0;
}



















