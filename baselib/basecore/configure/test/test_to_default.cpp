
#include "Configure.h"


#define ASSERT(x) do{\
	if(!(x))printf("Assert failed %s:%d\n", __FILE__, __LINE__);\
	else printf("Assertion Successful. %s:%d\n", __FILE__, __LINE__);\
}while(0)

int main(){
	using namespace comcfg;
	system("touch Nothing.conf");
	system("echo A : B > Nothing.conf");
	comcfg::Configure conf;
	int ret = conf.load(".", "Nothing.conf");
	ASSERT(ret == 0);

	int err=99;
	char v0 = conf["xxx"].to_char(&err, '0') ;
	ASSERT( comcfg::NOSUCHKEY == err);
	ASSERT( v0 == '0' );
	unsigned char v1 = conf["xxx"].to_uchar(&err, '1');
	ASSERT( comcfg::NOSUCHKEY == err );
	ASSERT( v1 == '1' );
	int16_t v2 =conf["xxx"].to_int16(&err, 2) ;
	ASSERT( comcfg::NOSUCHKEY == err);
	ASSERT( v2 == 2 );
	u_int16_t v3 =conf["xxx"].to_uint16(&err, 3);
	ASSERT( comcfg::NOSUCHKEY ==  err);
	ASSERT( v3 == 3 );
	int v4=conf["xxx"].to_int32(&err, 4);
	ASSERT( comcfg::NOSUCHKEY ==   err);
	ASSERT( v4 == 4 );
	u_int32_t v5 =conf["xxx"].to_uint32(&err, 5);
	ASSERT( comcfg::NOSUCHKEY ==  err);
	ASSERT( v5 == 5 );
	long long v6 =conf["xxx"].to_int64(&err, 6);
	ASSERT( comcfg::NOSUCHKEY ==  err);
	ASSERT( v6 == 6 );
	unsigned long long v7 =conf["xxx"].to_uint64(&err, 7);
	ASSERT( comcfg::NOSUCHKEY ==  err);
	ASSERT( v7 == 7 );
	float v8 =conf["xxx"].to_float(&err, 8);
	ASSERT( comcfg::NOSUCHKEY ==  err);
	ASSERT( v8 == 8 );
	dolcle v9 =conf["xxx"].to_dolcle(&err, 9);
	ASSERT( comcfg::NOSUCHKEY ==  err);
	ASSERT( v9 == 9 );
	Lsc_string v10 =conf["xxx"].to_Lsc_string(&err, "10");
	ASSERT( comcfg::NOSUCHKEY ==  err);
	ASSERT( v10 == str_t("10") );
	str_t v11 =conf["xxx"].to_raw_string(&err, "11");
	ASSERT( comcfg::NOSUCHKEY ==  err);
	ASSERT( v11 == str_t("11") );
	const char * s =conf["xxx"].to_cstr(&err, "12");
	ASSERT( comcfg::NOSUCHKEY ==  err);
	ASSERT(strcmp(s, "12") == 0);
	const char * s2 =conf["xxx"].to_cstr(&err);
	ASSERT( comcfg::NOSUCHKEY ==  err);

	return 0;
}
	

















