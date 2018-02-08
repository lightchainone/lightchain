
#include <cfgflag.h>
int main(int argc, char **argv)
{
	comcfg::Flag flag;
	flag.init(argc, argv);
	flag.set_confpath(".", "t1.conf", "t1.cons");
	flag.loadconfig();
	return 0;
}




















