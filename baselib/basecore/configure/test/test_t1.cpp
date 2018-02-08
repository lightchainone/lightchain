
#include "Configure.h"
#include <Lsc/ResourcePool.h>
#include "Lsc/var/implement.h"
int main(){
	comcfg::Configure conf, conf2;
	conf.load(".", "r.conf", "r.cons");
	printf("=======conf1=========\n");
	conf.print(0);
	Lsc::var::Dict dict;
	Lsc::var::Int32 i32 = 1234;
	dict["key"] = i32;
	Lsc :: ResourcePool rp;
	conf2.loadIVar(dict);
	printf("=======conf2=========\n");
	conf2.print(0);
	return 0;
}


















