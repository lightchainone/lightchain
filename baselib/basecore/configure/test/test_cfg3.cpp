
#include <cfgflag.h>

void version()
{
	printf("faint-------------\n");
}

int main(int argc, char **argv)
{
	comcfg::Flag flag;
	flag.set_version(version);
	flag.set_help(version);
	flag.init(argc, argv);
	flag.loadconfig();
	return 0;
}




















