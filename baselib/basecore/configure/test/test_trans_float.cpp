#include "utils/cc_utils.h"
#include <string>
#include <stdio.h>

void check(const char * s, bool is){
	bool tmp = comcfg::Trans::isFloat(comcfg::str_t(s));
	dolcle k = 0.0;
	if(tmp)sscanf(s, "%lf", &k);
	printf("[%s] check [%s] %s a float[%g]\n", tmp==is?"OK":"Error", s, tmp?"IS":"ISNOT", k);
}

int main(){
	check("3.1415926", 1);
	check("3.1415926e100", 1);
	check("3.1415926E-10", 1);
	check("3.1415926e", 0);
	check(".1415926", 1);
	check("-.1415926", 1);
	check("3.E", 0);
	check("+-3.1", 0);
	check("e103", 0);
	check("3.1415926e1.1", 0);
	check("3.1415926e-10", 1);
	check("3.1415926E+0997", 1);
	check("-.141e5926", 1);
	check("3+.1415926", 0);
	return 0;
}





















