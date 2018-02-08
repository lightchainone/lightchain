
#include "reader/RawData.h"
#include <stdio.h>

int main(){
	comcfg::RawData rd;
	rd.load("raw.conf");
	for(int i = 0; (size_t)i < rd.lineNum(); ++i){
		printf("%s\n", rd.getLine(i));
	}
	return 0;
}











