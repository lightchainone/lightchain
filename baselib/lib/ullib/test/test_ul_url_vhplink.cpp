
#include "ul_url.h"
#include <stdio.h>

// test fragment parse 
//char url_str[1024] = "http://www.xxx.com/index.html?a=b&c=d#page=123";
//char url_str[1024] = "http://www.xxx.com/index.html?a=b&c=d#";
//char url_str[1024] = "http://www.xxx.com/index.html?a=b&c=d";

//char url_str[1024] = "http://www.ad.adver.com.tw/adver/Rnams/K~/123/photo_h140070~http:/ad_imp";
//char url_str[1024] = "http://www.ad.adver.com.tw/adver/Rnams/K~/123/photo_h140070~http:////ad_imp";
//char url_str[1024] = "http://www.ad.adver.com.tw/adver/Rnams/K~/123/photo_h140070~http://./../../ad_imp";


char url_str[1024] = "http://www.ixintui.com#frag";


int main(){

	char site[128], port[128], path[128], frag[128];
	
	
	int ret = ul_check_url_vhplink( url_str );
	//int ret = ul_check_url( url_str );
	
	printf("----\ncheck ret = %d\nurl = %s\n", ret, url_str );
	
	//ret = ul_parse_url( url_str, site , port , path );
	ret = ul_parse_url_vhplink( url_str, site , port , path , frag );

	
	printf("----\nparse ret = %d \nurl = %s\nsite = %s\nport = %s\npath = %s\nfrag = %s\n",
			ret, url_str, site, port, path, frag );

	ul_normalize_path_vhplink( path );

	printf("----\nnormalize path \npath = %s\n",path);

	return 0;
}
