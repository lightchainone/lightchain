
#include "Configure.h"

int main(){
	
	comcfg::Configure conf;
	conf.load(".", "t1.conf");
	conf.print();
	Lsc::ResourcePool rp;
	Lsc::var::IVar& ivar = conf.to_IVar(&rp);
	

	comcfg::Configure cf2;
	printf("\nloadIvar ===> %d \n", cf2.loadIVar(ivar));
	printf("\n=== conf2.dump ===\n%s\n\n", cf2.dump(NULL));
	printf("\n=== conf2.print ===\n");
	cf2.print();

	return 0;
}


















