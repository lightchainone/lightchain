#include "utils/cc_trans.h"
#include <assert.h>
#include <stdio.h>

int main(){
	FILE * fp = fopen("str2str", "r");
	assert(fp);
	char buf[1024];
	while(fgets(buf, 1024, fp) != NULL){
		buf[strlen(buf) - 1] = 0;
		comcfg::str_t str;
		if(comcfg::Trans::str2str(comcfg::str_t(buf), &str) == 0){
			puts("----- Read -----");
			puts(buf);
			puts("----- trans to -----");
			puts(str.c_str());
			puts("------ Finish -----");
		}
		else {
			puts("Error");
		}
	}
	return 0;
}











