
#include "CmdOption.h"

int showVersion(comcfg::CmdOption * cmd){
	printf("Version : %g\n", (*cmd)["v"].to_float());
	return 0;
}

int main(){
	char *s[] = {"-a", "-h", "-v1.0", "--port=80", "roba", "cnhawk", "-ccode", "wtommy", "-f"};

	comcfg:: CmdOption cmd;
	cmd.init(7, s, "a:hv::c::");
	cmd.registHook("v", showVersion);
	cmd.runHook();
	return 0;
}



















