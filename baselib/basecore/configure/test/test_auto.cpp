
#include "cfgext.h"
#include "Configure.h"
int main(int argc, char ** argv){
	if(argc == 1){
		printf("Usage : %s constraint_file\n\n", argv[0]);
		return 0;
	}
	try{
		
		Lsc::string str = comcfg::autoConfigGen(argv[1]);
		printf("%s", str.c_str());
#if 1
		comcfg::Configure conf;
		char * s = conf.getRebuildBuffer(str.size());
		strcpy(s, str.c_str());
		int ret = conf.rebuild();
		printf("================= rebuild ret = %d ===================\n", ret);
		conf.print();
#endif
	}catch(comcfg::ConfigException e){
		fprintf(stderr, "Catch : %s\n", e.what());
	}catch(...){
		fprintf(stderr, "Catch : Unknown\n");
	}
		
	return 0;
}




















