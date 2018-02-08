
#include "EnvGet.h"
#include "ul_conf.h"
#include "Configure.h"
#include "stdio.h"
#include <string>

using namespace comcfg;
ENV_CHAR_DECL(MAC, 0);
ENV_UCHAR_DECL(MAC, 0);
ENV_INT16_DECL(MAC, 0);
ENV_UINT16_DECL(MAC, 0);
ENV_INT32_DECL(MAC, 0);
ENV_UINT32_DECL(MAC, 0);
ENV_INT64_DECL(MAC, 0);
ENV_UINT64_DECL(MAC, 0);
ENV_FLOAT_DECL(MAC, 0);
ENV_DOLCLE_DECL(MAC, 0);
ENV_BSLSTR_DECL(MAC, 0);

int main(){
	comcfg::Configure conf;
	
	conf.load("./", "new.conf");
	comcfg::str_t str;
	if(conf["COMLOG"]["FILE"]["TYPE"].get_Lsc_string(&str) == 0){
		printf("read = [%s]\n", str.c_str());
	}else{
		printf("No such key: %s\n", conf.getErrKeyPath());
	}
	if(conf["COMLOG"]["FILEx"]["TYPE"][2].get_Lsc_string(&str) == 0){
		printf("read = [%s]\n", str.c_str());
	}else{
		printf("No such key: %s\n", conf.getErrKeyPath());
	}
	try{
		str = conf["COMLOG"]["FILE"]["TYPE"].to_Lsc_string();
		printf("to Lsc string: %s\n", str.c_str());
	}
	catch(Lsc::Exception e){
		printf("%s\n", e.what());
		printf("No such key: %s\n", conf.getErrKeyPath());
	}
	try{
		conf["COMLOG"]["FILE"]["abc"].to_char();
	}
	catch(Lsc::Exception e){
		printf("%s\n", e.what());
		printf("No such key: %s\n", conf.getErrKeyPath());
	}
	int err;
	int tmp;
	try {
	tmp = conf["COMLOG"]["FILE"]["SIZE"].to_int32();
	printf("read = [%d]\n", tmp);}
	catch (Lsc::Exception e) {
		printf("%s\n", e.what());
	}

	
	if(conf.deepGet("COMLOG.FILE.TYPE").get_Lsc_string(&str) == 0){
		printf("deepGet(\"COMLOG.FILE.TYPE\") = [%s]\n", str.c_str());
	}else{
		printf("No such key: %s\n", conf.getErrKeyPath());
	}
	if(conf.deepGet("COMLOG.DEVICE[2]").get_Lsc_string(&str) == 0){
		printf("deepGet(\"COMLOG.DEVICE[2]\") = [%s]\n", str.c_str());
	}else{
		printf("No such key: %s\n", conf.getErrKeyPath());
	}

	printf("dump:%s\n%s\n\n", conf.dump(NULL));

	comcfg::Configure nc;
	Lsc::ResourcePool rp;
	nc.loadIVar(conf.to_IVar(&rp,&err));
	printf("new dump:%s\n%s\n\n", nc.dump(NULL));

	
	printf("ENV_CHAR:  MAC= %c\n", g_char_MAC);
	printf("ENV_UCHAR:  MAC= %c\n", g_uchar_MAC);
	printf("ENV_INT16:  MAC= %d\n", g_int16_MAC);
	printf("ENV_UINT16:  MAC= %d\n", g_uint16_MAC);
	printf("ENV_INT32:  MAC= %d\n", g_int32_MAC);
	printf("ENV_UINT32:  MAC= %d\n", g_uint32_MAC);
	printf("ENV_INT64:  MAC= %lld\n", g_int64_MAC);
	printf("ENV_UINT64:  MAC= %lld\n", g_uint64_MAC);
	printf("ENV_FLOAT:  MAC= %f\n", g_float_MAC);
	printf("ENV_DOLCLE:  MAC= %f\n", g_dolcle_MAC);
	printf("ENV_BSLSTR:  MAC= %s\n", g_Lscstr_MAC.c_str());

	Lsc::string a;
	printf("%s\n", a.c_str());
	if (a == "") printf("NULL STR\n");

	


	return 0;
}


















