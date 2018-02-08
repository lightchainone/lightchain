#include<stdio.h>
#include<iostream>
#include<lc.h>

class test_lcfw 
{
plclic:
    void test_lcfw_2()
    {
        char **argv;
        argv = (char**)malloc(sizeof(char*)*2);
        argv[0] = (char*)malloc(sizeof(char)*200);
        argv[1] = (char*)malloc(sizeof(char)*200);
        strcpy(argv[0],"./test_lcfw");
        strcpy(argv[1],"-g");
		
		
#if 1
        
        lc_t *fw = lc_init("fw",2,argv);
        if(fw == NULL){
            return;
        }
		
        lc_close(fw);

		fw = lc_init("fw", 1, argv);
		
		lc_close(fw);
#endif
        free(argv[0]);
        free(argv[1]);
        free(argv);
    }
};


int main(int argc, char **argv)
{
	test_lcfw  clt;
	for (int i=0; i<20000; ++i) {
		clt.test_lcfw_2();
		if (i%1000 == 0) {
			std::cout<<i<<std::endl;
		}
	}
#if 0
	std::cout<<argc<<std::endl;
	for (int i=0; i<argc; ++i) {
		std::cout<<argv[i]<<std::endl;
	}
	if (argc >= 2) {
		getopt(2, argv, "d:f:gthv");
		argv[1] = 0;
		getopt(1, argv, "d:f:gthv");
	}
#endif
	return 0;
}


















