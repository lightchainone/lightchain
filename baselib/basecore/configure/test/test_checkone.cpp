
#include "Configure.h"

int main(int argc, char* argv[]){
	if (argc < 3) {
		printf("usage : %s conf range\n",argv[0]);
		return -1;
	}
	comcfg::Configure conf;
	conf.load("./", argv[1]);
	int chk = conf.check_once(argv[2]);
	printf("checkone:%d\n", chk);

	return 0;
}

