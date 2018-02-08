#include <cfgflag.h>
int main(int argc, char **argv)
{
	comcfg::Flag flag;
	flag.init(argc, argv);
	flag.loadconfig();
	return 0;
}




















