
#include <cassert>
#include "ul_url.h"
static void ul_fetch_maindomain_ex2_check(const char* site,
                                         DomainDict* dict=NULL){
    char normal_site[1024];
    ul_normalize_site_ip(site,normal_site,sizeof(normal_site));
    char trunk[1024];
    const char* domain=ul_fetch_maindomain_ex2(normal_site,trunk,sizeof(trunk),dict);
    printf("site:'%s' domain:'%s' trunk:'%s'\n",site,domain,trunk);
}

static int ul_fetch_maindomain_ex2_test(){
  ul_fetch_maindomain_ex2_check(".sina.com.cn");
  ul_fetch_maindomain_ex2_check("123.111.222.101");
    ul_fetch_maindomain_ex2_check("00027.00023.000000000057.000153");
    ul_fetch_maindomain_ex2_check("abc.bj.cn");
    ul_fetch_maindomain_ex2_check("www.abc.bj.ah");
    ul_fetch_maindomain_ex2_check("abc.org.org");
    ul_fetch_maindomain_ex2_check("0x17.0000000023.0057.0xD6");
    ul_fetch_maindomain_ex2_check("0x17.0x13.0x2F.0xD6");
    ul_fetch_maindomain_ex2_check("00508.cc");
    ul_fetch_maindomain_ex2_check("02.12.04.cc");
    ul_fetch_maindomain_ex2_check("0000027.0000000023.0057.000326");
    ul_fetch_maindomain_ex2_check("02.co.hk");
    ul_fetch_maindomain_ex2_check("00508.cc");
    ul_fetch_maindomain_ex2_check("aaa.com.hk.cn");

    DomainDict dict;
    assert(dict.load_file("","test_ul_url_dict.txt")==0);
    ul_fetch_maindomain_ex2_check("www.fuckmother.xxx",&dict);
    ul_fetch_maindomain_ex2_check("www.fuckmother.fuck",&dict);
    ul_fetch_maindomain_ex2_check("www.fuckmother.xxx.fuck",&dict);
    ul_fetch_maindomain_ex2_check("www.fuckmother.pig.fuck",&dict);
    ul_fetch_maindomain_ex2_check("00508.cc",&dict);

    dict.clear();
    assert(dict.load_file("../src","ul_url_default_dict.txt")==0);
    ul_fetch_maindomain_ex2_check("123.111.222.101",&dict);
    ul_fetch_maindomain_ex2_check("00027.00023.000000000057.000153",&dict);
    ul_fetch_maindomain_ex2_check("abc.bj.cn",&dict);
    ul_fetch_maindomain_ex2_check("www.abc.bj.ah",&dict);
    ul_fetch_maindomain_ex2_check("abc.org.org",&dict);
    ul_fetch_maindomain_ex2_check("0x17.0000000023.0057.0xD6",&dict);
    ul_fetch_maindomain_ex2_check("0x17.0x13.0x2F.0xD6",&dict);
    ul_fetch_maindomain_ex2_check("00508.cc",&dict);
    ul_fetch_maindomain_ex2_check("02.12.04.cc",&dict);
    ul_fetch_maindomain_ex2_check("0000027.0000000023.0057.000326",&dict);
    ul_fetch_maindomain_ex2_check("02.co.hk",&dict);
    ul_fetch_maindomain_ex2_check("00508.cc",&dict);
    ul_fetch_maindomain_ex2_check("aaa.com.hk.cn",&dict);

    return 0;
}
int main(){
    ul_fetch_maindomain_ex2_test();
    return 0;
}
