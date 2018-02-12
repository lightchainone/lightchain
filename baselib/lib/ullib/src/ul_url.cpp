#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "ul_log.h"
#include "ul_char.h"
#include "ul_url.h"
/*
// LETTER DIGIT
static unsigned char UL_LET_DIGIT[256]={ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0         };

//LETTER DIGIT HYPHEN
static unsigned char UL_LET_DIG_HY[256]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,
1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0         };




static char UL_DIGIT[256] = {    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0        };

static char UL_UP_LET[256] = {    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0        };


//uchar = unreserved | escape
static char UL_UCHAR[256] = {     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,0,0,1,1,0,1,1,1,1,0,1,1,1,0,
    1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1        };

*/
//add ' ' '"' '<' '>' 32 34 60 62
static char UL_PCHAR[256] = {    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,
                                 1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
                            };

//uchar | reserved
//add ' ' '"' '<' '>' 32 34 60 62
static char UL_UCH_RES[256] = {    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                   1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
                                   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
                              };

//pchar | "/"
//add ' ' '"' '<' '>' 32 34 60 62
static char UL_PARAM[256] = {    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
                            };

//rfc2396
//define following chars:
static char UL_RESERVED[256] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,1,1,1,0,0,0,0,1,1,0,0,1,
                                 0,0,0,0,0,0,0,0,0,0,1,1,0,1,0,1,
                                 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                               };

// '\r' '\n'
static char UL_LFCR[256] = {
    0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};


//delete \r \n in string
static int ul_delete_inter(char * str) {
    if(NULL==str)
        return 0;
    size_t len = strcspn(str, "\r\n");

    char *p1 = str + len;
    char *p2 = str + len;

    while(*p2 !='\0') {
        if (UL_LFCR[(unsigned char )*p2]) {
            p2++;

        } else *p1++ = *p2++;
    }
    *p1 = '\0';
    return p1-str;
}

static int ul_space_escape(char * url,int max_url_len, int max_site_len, int max_path_len) {
    char * pin = url ;
    int  pout = 0 ;
    char url_tmp[UL_MAX_URL_LEN_EX] ;
    while(*pin!='\0') {
        if(*pin!=' ') {
            if(pout>=max_url_len -1)
                return 0 ;
            url_tmp[pout++] = *pin++ ;
        } else { //space
            if(pout>=max_url_len-3)
                return 0 ;
            url_tmp[pout++]='%' ;
            url_tmp[pout++]='2' ;
            url_tmp[pout++]='0' ;
            pin++ ;
        }
    }
    if(pout>=max_url_len)
        return 0 ;
    url_tmp[pout]='\0' ;
    //detect site len and path len
    if(strncmp(url_tmp,"http://",7)==0) {
        pin = url_tmp+7 ;
    } else {
        pin = url_tmp ;
    }
    char * path = strchr(pin,'/') ;
    if(path==pin)
        return 0 ;
    if(path!=NULL && strlen(path)>=(u_int)max_path_len)
        return 0 ;
    char * port = strchr(pin,':') ;
    char * psite_end = NULL ;
    if(port==pin)
        return 0 ;
    if(NULL!=port&&NULL!=path&&port<path) {
        psite_end=port-1 ;
    } else if(path!=NULL) {
        psite_end = path -1 ;
    }
    if(psite_end!=NULL && psite_end-pin>=max_site_len) {
        return 0 ;
    } else if (psite_end==NULL && strlen(pin) >= (u_int)max_site_len) {
        return 0 ;
    }
    strncpy(url,url_tmp,pout+1) ;
    return 1 ;
}
static int ul_space_escape_path(char * path,int max_path_len) {
    char * pin = path ;
    int  pout = 0 ;
    char url_tmp[UL_MAX_URL_LEN_EX] ;
    while(*pin!='\0') {
        if(*pin!=' ') {
            if(pout>=max_path_len -1)
                return 0 ;
            url_tmp[pout++] = *pin++ ;
        } else { //space
            if(pout>=max_path_len-3)
                return 0 ;
            url_tmp[pout++]='%' ;
            url_tmp[pout++]='2' ;
            url_tmp[pout++]='0' ;
            pin++ ;
        }
    }
    if(pout>=max_path_len)
        return 0 ;
    url_tmp[pout]='\0' ;

    strncpy(path,url_tmp,pout+1) ;
    return 1 ;
}
// {{{ ul_to_char

//translate the HEX to DEC
//return the DEC
//A-F:65-70   a-f:97-102 ->10-15 '0'-'9':48-57 -> 0-9
static char UL_TO_CHAR[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,
    0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
int ul_to_char(char hex[2]) {

    int r;
    int h[2];
    if((UL_TO_CHAR[(unsigned char)hex[0]]==0 && hex[0]!=48) ||  (UL_TO_CHAR[(unsigned char)hex[1]]==0 && hex[1]!=48))
        return 0;
    h[0] = UL_TO_CHAR[(unsigned char)hex[0]];
    h[1] = UL_TO_CHAR[(unsigned char)hex[1]];
    r = 16*h[0]+h[1];
    return r;
}

static int ul_single_path_nointer(char *path) {
    char *pin;
    int   pout;
    char  hex[2];
    char  tmp_hex[2];
    char  escape_char;
    int space_flag = 0;
    pin = path;
    pout = 0;
    //check abs_path
    if(*pin == '/') {
        path[pout++] = *pin ++;
    }

    //path
    while(UL_PCHAR[(unsigned char)*pin]) {
        tmp_hex[0] = '\0';
        if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
            strncpy(hex,++pin,2);
            strncpy(tmp_hex,hex,2);
            pin ++;
            escape_char = ul_to_char(hex);
            if(escape_char == 0) {
                pin--;
                path[pout++]='%';
                continue;
            }
            *pin = escape_char;
        }
        if(UL_PCHAR[(unsigned char)*pin] &&
                (tmp_hex[0]=='\0' || !UL_RESERVED[(unsigned char)*pin])) {
            if(*pin == ' ') {
                space_flag = 1;
            }
            path[pout++] = *pin++;
        } else {
            path[pout++]='%';
            strncpy(&path[pout],tmp_hex,2);
            pout=pout+2;
            pin++;
        }
    }

    while(*pin == '/') {
        path[pout++] = *pin ++;
        while(UL_PCHAR[(unsigned char)*pin]) {
            tmp_hex[0] = '\0';
            if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
                strncpy(hex,++pin,2);
                strncpy(tmp_hex,hex,2);
                pin ++;
                escape_char = ul_to_char(hex);
                if(escape_char == 0) {
                    pin--;
                    path[pout++]='%';
                    continue;
                }
                *pin = escape_char;
            }

            if(UL_PCHAR[(unsigned char)*pin] &&
                    (tmp_hex[0]=='\0' || !UL_RESERVED[(unsigned char)*pin])) {
                if(*pin == ' ') {
                    space_flag = 1;
                }
                path[pout++] = *pin++;
            } else {
                path[pout++]='%';
                strncpy(&path[pout],tmp_hex,2);
                pout=pout+2;
                pin++;
            }
        }
    }

    // ";" params  == *(";" param)
    while(*pin == ';') {
        path[pout++] = *pin ++;
        while(UL_PARAM[(unsigned char)*pin]) {
            tmp_hex[0] = '\0';
            if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
                strncpy(hex,++pin,2);
                strncpy(tmp_hex,hex,2);
                pin ++;
                escape_char = ul_to_char(hex);
                if(escape_char == 0) {
                    pin--;
                    path[pout++]='%';
                    continue;
                }
                *pin = escape_char;
            }
            if(UL_PARAM[(unsigned char)*pin] &&
                    (tmp_hex[0]=='\0' || !UL_RESERVED[(unsigned char)*pin])) {
                if(*pin == ' ') {
                    space_flag = 1;
                }
                path[pout++] = *pin++;
            } else {
                path[pout++]='%';
                strncpy(&path[pout],tmp_hex,2);
                // snprintf(&path[pout],4,"%%%s",tmp_hex);
                pout=pout+2;
                pin++;
            }
        }
    }

    //"?" query
    while(*pin == '?') {
        path[pout++] = *pin ++;
        while(UL_UCH_RES[(unsigned char)*pin]) {
            tmp_hex[0] = '\0';
            if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
                strncpy(hex,++pin,2);
                strncpy(tmp_hex,hex,2);
                pin ++;
                escape_char = ul_to_char(hex);
                if(escape_char == 0) {
                    pin--;
                    path[pout++]='%';
                    continue;
                }
                *pin = escape_char;
            }
            if(UL_UCH_RES[(unsigned char)*pin] &&
                    (tmp_hex[0]=='\0' || !UL_RESERVED[(unsigned char)*pin])) {
                if(*pin == ' ') {
                    space_flag = 1;
                }
                path[pout++] = *pin++;
            } else {
                path[pout++]='%';
                strncpy(&path[pout],tmp_hex,2);
                pout=pout+2;
                pin++;
            }
        }
    }
    //if(*pin != 0)
    if(*pin != 0 && *pin != '#')
        return 0;
    path[pout] = 0;
    if ( pout > 256 )
        return 0;
    if(space_flag==1) {
        return ul_space_escape_path(path,UL_MAX_PATH_LEN);
    } else {
        return 1;
    }
    //check abs_path over
}

// {{{ ul_single_path
/*
 * function : convert the escape squence to original one
 */
int ul_single_path(char *path) {
    ul_delete_inter(path);
    return ul_single_path_nointer(path);
}
// }}}


static int ul_single_path_ex_nointer(char *path, int len) {
    char *pin;
    int   pout;
    char  hex[2];
    char  tmp_hex[2];
    char  escape_char;
    int space_flag = 0;
    //len = strlen(path);
    pin = path;
    pout = 0;
    //check abs_path
    if(*pin == '/') {
        path[pout++] = *pin ++;
    }

    //path
    while(UL_PCHAR[(unsigned char)*pin]) {
        tmp_hex[0] = '\0';
        if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
            strncpy(hex,++pin,2);
            strncpy(tmp_hex,hex,2);
            pin ++;
            escape_char = ul_to_char(hex);
            if(escape_char == 0) {
                pin--;
                path[pout++]='%';
                continue;
            }
            *pin = escape_char;
        }
        if(UL_PCHAR[(unsigned char)*pin] &&
                (tmp_hex[0]=='\0' || !UL_RESERVED[(unsigned char)*pin])) {
            if(*pin == ' ') {
                space_flag = 1;
            }
            path[pout++] = *pin++;
        } else {
            path[pout++]='%';
            strncpy(&path[pout],tmp_hex,2);
            pout=pout+2;
            pin++;
        }
    }

    while(*pin == '/') {
        path[pout++] = *pin ++;
        while(UL_PCHAR[(unsigned char)*pin]) {
            tmp_hex[0] = '\0';
            if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
                strncpy(hex,++pin,2);
                strncpy(tmp_hex,hex,2);
                pin ++;
                escape_char = ul_to_char(hex);
                if(escape_char == 0) {
                    pin--;
                    path[pout++]='%';
                    continue;
                }
                *pin = escape_char;
            }

            if(UL_PCHAR[(unsigned char)*pin] &&
                    (tmp_hex[0]=='\0' || !UL_RESERVED[(unsigned char)*pin])) {
                if(*pin == ' ') {
                    space_flag = 1;
                }
                path[pout++] = *pin++;
            } else {
                path[pout++]='%';
                strncpy(&path[pout],tmp_hex,2);
                pout=pout+2;
                pin++;
            }
        }
    }

    // ";" params  == *(";" param)
    while(*pin == ';') {
        path[pout++] = *pin ++;
        while(UL_PARAM[(unsigned char)*pin]) {
            tmp_hex[0] = '\0';
            if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
                strncpy(hex,++pin,2);
                strncpy(tmp_hex,hex,2);
                pin ++;
                escape_char = ul_to_char(hex);
                if(escape_char == 0) {
                    pin--;
                    path[pout++]='%';
                    continue;
                }
                *pin = escape_char;
            }
            if(UL_PARAM[(unsigned char)*pin] &&
                    (tmp_hex[0]=='\0' || !UL_RESERVED[(unsigned char)*pin])) {
                if(*pin == ' ') {
                    space_flag = 1;
                }
                path[pout++] = *pin++;
            } else {
                path[pout++]='%';
                strncpy(&path[pout],tmp_hex,2);
                // snprintf(&path[pout],4,"%%%s",tmp_hex);
                pout=pout+2;
                pin++;
            }
        }
    }

    //"?" query
    while(*pin == '?') {
        path[pout++] = *pin ++;
        while(UL_UCH_RES[(unsigned char)*pin]) {
            tmp_hex[0] = '\0';
            if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
                strncpy(hex,++pin,2);
                strncpy(tmp_hex,hex,2);
                pin ++;
                escape_char = ul_to_char(hex);
                if(escape_char == 0) {
                    pin--;
                    path[pout++]='%';
                    continue;
                }
                *pin = escape_char;
            }
            if(UL_UCH_RES[(unsigned char)*pin] &&
                    (tmp_hex[0]=='\0' || !UL_RESERVED[(unsigned char)*pin])) {
                if(*pin == ' ') {
                    space_flag = 1;
                }
                path[pout++] = *pin++;
            } else {
                path[pout++]='%';
                strncpy(&path[pout],tmp_hex,2);
                pout=pout+2;
                pin++;
            }
        }
    }
    //if(*pin != 0)
    if(*pin != 0 && *pin != '#')
        return 0;
    path[pout] = 0;
    if (pout > len )
        return 0;
    if(space_flag == 1) {
        return ul_space_escape_path(path,UL_MAX_PATH_LEN_EX);;
    } else {
        return 1;
    }
    //check abs_path over
}

int ul_single_path_ex(char *path) {
    int len = ul_delete_inter(path); //delete '\r' '\n';
    return ul_single_path_ex_nointer(path, len);
}

void replace_slash(char *path) {
    char *p, *pend;

    pend = strchr(path, '?');
    if (pend == NULL) {
        pend = strchr(path, ';');
    }
    if (pend == NULL) {
        pend = path + strlen(path);
    }

    for (p = path; p != pend; p++) {
        if (*p == '\\')
            *p = '/';
    }
}


void ul_normalize_path(char *path) {
    char *p, *q;
    char tmp[UL_MAX_URL_LEN];
    char *pend;

    assert(*path == '/');

    replace_slash(path);

    strcpy(tmp, path);

    pend = strchr(tmp, '?');
    if (pend == NULL) {
        pend = strchr(tmp, ';');
    }
    if (pend == NULL) {
        pend = tmp + strlen(tmp);
    }

    p = tmp+1;
    q = path;
    while (p != pend) {
        /* normal */
        if (*q != '/') {
            q++;
            *q = *p++;
            continue;
        }

        /* /./ */
        if (strncmp(p, "./", 2)==0) {
            p += 2;
            continue;
        }

        /* /../ */
        if (strncmp(p, "../", 3)==0) {
            p += 3;
            if (q == path) {
                continue;
            }
            /* change q to prev '/' */
            q--;
            while (*q != '/')
                q--;
            continue;
        }

        /* // */
        if (*p=='/') {
            p++;
            continue;
        }

        /* "/." */
        if (strncmp(p, ".", pend-p)==0) {
            p++;
            continue;
        }

        /* "/.." */
        if (strncmp(p, "..", pend-p)==0) {
            p += 2;
            if (q == path) {
                continue;
            }
            /* change q to prev '/' */
            q--;
            while (*q != '/')
                q--;
            continue;
        }

        q++;
        *q = *p++;

    }
    q++;
    strcpy(q, p);
    return ;
}


//parse a url and extract site name, port and others.
int ul_parse_url(const char *input,char *site,char *port,char *path) {
    char              tmp[UL_MAX_URL_LEN];
    char              *pin = tmp;
    char              *p,*q;
    char              *p_query = NULL;

    if(strlen(input) >= UL_MAX_URL_LEN) {
        ul_writelog(UL_LOG_DEBUG,"ul_parse_url: url is too long");
        return 0;
    }

    strcpy(tmp,input);
    ul_delete_inter(tmp);

    if(strncasecmp(pin,"http://",7) == 0)
        pin+=7;

    //get path
    p = strchr(pin,'/');
    p_query = strchr(pin,'?');
    if(NULL==p) {
        p = p_query;
    }
    if ((NULL!=p_query) && (p_query<=p)) {
        if(path!=NULL) {
            path[0]='\0';
            if(strlen(p_query)<UL_MAX_PATH_LEN-1) {
                ul_strlcpy(path,"/",2);
                strcat(path,p_query);
            } else
                return 0;
        }
        *p_query='\0' ;
    } else {
        if(p != NULL) {
            if (path != NULL) {
                path[0] = '\0';
                if(strlen(p) < UL_MAX_PATH_LEN)
                    strcpy(path,p);
                else
                    return 0;
            }
            *p = '\0';
        } else {
            if (path != NULL) {
                strcpy(path,"/");
            }
        }
    }

    q = strchr(pin,':');
    //get port

    if(q != NULL) {
        if (port != NULL) {
            port[0] = '\0';
            if(strlen(q) < UL_MAX_PORT_LEN && atoi(q+1) > 0) {
                strcpy(port,q+1);
            } else
                return 0;
        }
        *q = '\0';
    } else {
        if (port != NULL)
            port[0] = '\0';

    }
    //check if the default port
    if((port!=NULL)&&(strncmp(port,"80",3))==0) {
        port[0]=0;
    }
    //get site
    if (site != NULL) {
        site[0] = '\0';
        if(pin[0] != '\0' && strlen(pin) < UL_MAX_SITE_LEN) {
            strcpy(site,pin);
        } else {
            ul_writelog(UL_LOG_DEBUG,"ul_parse_url: site name too long or empty url[%s]",pin);
            return 0;
        }

    }
    return 1;

}
char *ul_get_path(const char *url, char *path) {
    if (ul_parse_url(url, NULL, NULL, path) == 0)
        return NULL;
    return path;

}

char *ul_get_site(const char *url, char *site) {
    if (ul_parse_url(url, site, NULL, NULL) == 0)
        return NULL;
    return site;

}

int ul_get_port(const char* url,int* pport) {
    char strport[UL_MAX_PORT_LEN];
    if(ul_parse_url(url,NULL,strport,NULL)== 1) {
        if(strlen(strport) == 0) {
            *pport=80;
            return 1;
        } else {
            *pport=atoi(strport);
            return 1;
        }
    } else
        return 0;
}

void ul_get_static_part(const char *url, char *staticurl) {
    char *p;
    char buffer[UL_MAX_URL_LEN];

    assert(strlen(url) < UL_MAX_URL_LEN);
    strcpy(buffer, url);

    p = strchr(buffer, '?');
    if (p != NULL) {
        *p = '\0';
    }
    p = strchr(buffer, ';');
    if (p != NULL) {
        *p = '\0';
    }

    strcpy(staticurl, buffer);
}

const int ul_dyn_char_table[256]= {
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,0,1,1,1,0,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,1,0,1,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

int ul_isdyn(char* str) {
    unsigned char* pch=(unsigned char*)str;

    while(ul_dyn_char_table[*pch++]) {
    }
    return *(--pch);
}

#define MAX_URL_LEN 256

static char UL_URL_DIGIT[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
static unsigned char UL_URL_LET_DIGIT[256]= {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

//LETTER DIGIT HYPHEN
static unsigned char UL_URL_LET_DIG_HY[256]= {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,
    1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};



static char UL_URL_UP_LET[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};


//uchar = unreserved | escape
/*
   static char UL_URL_UCHAR[256] = {
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,1,0,0,1,1,0,1,1,1,1,0,1,1,1,0,
   1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
   0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 };
 */
//add ' ' '"' '<' '>' 32 34 60 62
static char UL_URL_PCHAR[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,0,
    1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

//uchar | reserved
//add ' ' '"' '<' '>' 32 34 60 62
static char UL_URL_UCH_RES[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

//pchar | "/"
//add ' ' '"' '<' '>' 32 34 60 62
static char UL_URL_PARAM[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1
};

int ul_check_url(char *url) {
    char *pin;
    char *pout;
    //char  hex[2];  //comment by Chen Jingkai at 2002:04:05
    char  port[6];
    int   k = 0;
    int dotcount=0;
    char *url_start;
    int len;
    int space_flag = 0;
    len = ul_delete_inter(url); //delete \r\n;
    if(strncasecmp(url,"http://",7) == 0) {
        pin = url + 7;
    } else {
        pin = url;
    }
    pout = pin;
    url_start = pin;
    //check host name
    while(UL_URL_LET_DIG_HY[(unsigned char)*pin] ||*pin=='%') {
        if(UL_URL_UP_LET[(unsigned char)*pin])
            *pin += 32;
        else {
            if(*pin=='%') {
                if(pin[1]!='\0' && pin[2]!='\0') {
                    unsigned char escape_char = ul_to_char(pin+1) ;
                    if(UL_URL_LET_DIG_HY[escape_char]) {
                        if (UL_URL_UP_LET[escape_char]) {
                            pin[2] = escape_char + 32;
                        } else {
                            pin[2] = escape_char;
                        }
                        pin+=2 ;
                    } else {
                        return 0 ;
                    }
                } else {
                    return 0 ;
                }
            }
        }
        *pout++=*pin++;
    }
    if(pin == url_start)
        return 0;

    pin --;
    if(!UL_URL_LET_DIGIT[(unsigned char)*pin++])
        return 0;
    while(*pin == '.') {
        dotcount++;
        *pout++=*pin++;
        while(UL_URL_LET_DIG_HY[(unsigned char)*pin] ||(*pin=='%')) {
            if(UL_URL_UP_LET[(unsigned char)*pin])
                *pin += 32;
            else {
                if(*pin=='%') {
                    if(pin[1]!='\0' && pin[2]!='\0') {
                        unsigned char escape_char = ul_to_char(pin+1) ;
                        if(UL_URL_LET_DIG_HY[escape_char]==1) {
                            if (UL_URL_UP_LET[escape_char]) {
                                pin[2] = escape_char + 32;
                            } else {
                                pin[2] = escape_char;
                            }
                            pin+=2 ;
                        } else {
                            return 0 ;
                        }
                    } else {
                        return 0 ;
                    }
                }
            }
            *pout++=*pin++;
        }
        pin --;
        if(!UL_URL_LET_DIGIT[(unsigned char)*pin++])
            return 0;
    }
    if(dotcount == 0) {
        return 0;
    }
    //check host name over

    //check port
    if(*pin == ':') {
        pin ++;
        if(*pin != '/') {
            while(UL_URL_DIGIT[(unsigned char)*pin]) {
                port[k++] = *pin++;
                if(k>5)
                    return 0;
            }
            port[k] = 0;
            if(*pin != '/')
                return 0;
            if(strcmp(port,"80") != 0) {
                memcpy(pout, pin-k-1, k+1);
                pout += (k+1);
            }
        }
    } else if( (*pin != '/') && (*pin !='\0' ))
        return 0;
    //check port over

    //check abs_path
    if(*pin == '/') {
        *pout++=*pin++;
        //path
        while(UL_URL_PCHAR[(unsigned char)*pin]) {
            space_flag|=((*pout++=*pin++)==' ');
        }

        while(*pin == '/') {
            *pout++=*pin++;
            while(UL_URL_PCHAR[(unsigned char)*pin]) {
                space_flag|=((*pout++=*pin++)==' ');
            }
        }

        // ";" params  == *(";" param)
        while(*pin == ';') {
            *pout++=*pin++;
            while(UL_URL_PARAM[(unsigned char)*pin]) {
                space_flag|=((*pout++=*pin++)==' ');
            }
        }

        //"?" query
        while(*pin == '?') {
            *pout++=*pin++;
            while(UL_URL_UCH_RES[(unsigned char)*pin]) {
                space_flag|=((*pout++=*pin++)==' ');
            }
        }
        if(*pin != 0)
            return 0;
        *pout = 0;
        if (pout-url > len)
            return 0;
        if(space_flag==1)
            return ul_space_escape(url ,UL_MAX_URL_LEN, UL_MAX_SITE_LEN, UL_MAX_PATH_LEN);
        else
            return 1;
    } else  if (*pin != 0)
        return 0;
    if (space_flag==1)
        return ul_space_escape(url , UL_MAX_URL_LEN, UL_MAX_SITE_LEN, UL_MAX_PATH_LEN);
    else
        return 1;

    //check abs_path over
}

// LETTER DIGIT
static unsigned char UL_LET_DIGIT[256]= { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
                                        0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                        1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
                                        0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
                                        1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                        };

//LETTER DIGIT HYPHEN
static unsigned char UL_LET_DIG_HY[256]= {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,
        1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
        0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,1,
        0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                         };


//      <hostname> ::= <name>*["."<name>]
//      <name>  ::= <let>[*[<let-or-digit-or-hyphen>]<let-or-digit>]

int check_hostname(char *host_name) {
    char *purl;

    if ( host_name == NULL )
        return 0;
    purl = host_name;

    while(UL_LET_DIG_HY[(unsigned char)(*purl++)]);
    purl -= 2;
    if(!UL_LET_DIGIT[(unsigned char)(*purl++)])
        return 0;
    while(*purl++ == '.') {
        while(UL_LET_DIG_HY[(unsigned char)(*purl++)]);
        purl-=2;
        if(!UL_LET_DIGIT[(unsigned char)(*purl++)])
            return 0;
    }

    purl--;
    if(*purl == '\0')
        return 1;
    else
        return 0;
}

int check_URL(char* url) {
    return ul_check_url(url);
}

/*
 * check a sitename is in dotip format
 * input : sitename : the sitename
 * return : non-zero if is
 *        : 0 if not
 */

int ul_is_dotip(const char *sitename) {
    in_addr     l_inp;
    int         ret;

    ret = inet_aton(sitename, &l_inp);
    return ret;
}

const int UL_IGNORE_COUNT=47;

typedef struct ul_ilist {
    char name[36];
    int length;
} ul_ilist_t;
const ul_ilist_t ul_ignore_list[UL_IGNORE_COUNT]= {
    {"ac.",3},
    {"ah.",3},
    {"bj.",3},
    {"co.",3},
    {"com.",4},
    {"cq.",3},
    {"ed.",3},
    {"edu.",4},
    {"fj.",3},
    {"gd.",3},
    {"go.",3},
    {"gov.",4},
    {"gs.",3},
    {"gx.",3},
    {"gz.",3},
    {"ha.",3},
    {"hb.",3},
    {"he.",3},
    {"hi.",3},
    {"hk.",3},
    {"hl.",3},
    {"hn.",3},
    {"jl.",3},
    {"js.",3},
    {"jx.",3},
    {"ln.",3},
    {"mo.",3},
    {"ne.",3},
    {"net.",4},
    {"nm.",3},
    {"nx.",3},
    {"or.",3},
    {"org.",4},
    {"pe.",3},
    {"qh.",3},
    {"sc.",3},
    {"sd.",3},
    {"sh.",3},
    {"sn.",3},
    {"sx.",3},
    {"tj.",3},
    {"tw.",3},
    {"www.",4},
    {"xj.",3},
    {"xz.",3},
    {"yn.",3},
    {"zj.",3}
};
int ul_is_ignore(const char* slip) {
    int head=0;
    int tail=UL_IGNORE_COUNT-1;
    int cur;
    int ret;
    while(head <= tail) {
        cur=(head+tail)/2;
        ret=strncmp(slip,ul_ignore_list[cur].name,ul_ignore_list[cur].length);
        if(ret <0) {
            tail=cur-1;
        } else if (ret >0) {
            head=cur+1;
        } else {
            return 1;
        }
    }
    return 0;
}

/*
 * fetch the trunk from the sitename, if site is a dotip, the ip
 * will be return in trunk
 * in-argu : site : the site name
 * out-argu : trunk : the trunk of the sitename
 * in-argu : size : the buffer size of the trunk
 * return : 1 if get trunk successfully
 *          -2 if the site have no trunk
 *          -3 if the site have no dot
 *          -1 unknown error
 */
int ul_fetch_trunk(const char* site,char *trunk,int size) {
    const char* pfirst;
    const char* ptail;
    int len;

    if(ul_is_dotip(site)) {
        strncpy(trunk,site,size-1);
        trunk[size-1]=0;
        return 1;
    }

    ptail=strrchr(site,'.');
    if(ptail == NULL) {
        return -3;
    }
    pfirst=ptail-1;
    while(pfirst >=site-1) {
        if((pfirst < site)||(*pfirst=='.')) {
            if(ul_is_ignore(pfirst+1)==0) {
                len=size-1>ptail-pfirst-1?ptail-pfirst-1:size-1;
                memcpy(trunk,pfirst+1,len);
                trunk[len]=0;
                return 1;
            } else {
                ptail=pfirst;
            }
        }
        pfirst--;
    }
    return -2;
}

/*
 * fetch the main domain from the sitename, if is a dotip, ip in domain,
 * if have no domain,return the whole sitename.
 * in-argu : site : the site name
 * out-argu : domain : the main domain of the sitename
 * in-argu : size : the buffer size of the trunk
 * return : the pointer to the domain in the site buffer
 *        : NULL if site format error.
 */
const char*  ul_fetch_maindomain(const char* site,char *domain,int size) {
    const char* pfirst;
    const char* ptail;
    int len;

    if(ul_is_dotip(site)) {
        strncpy(domain,site,size-1);
        domain[size-1]=0;
        return site;
    }


    ptail=strrchr(site,'.');
    if(ptail == NULL) {
        goto end;
    }
    pfirst=ptail-1;
    while(pfirst >=site-1) {
        if((pfirst == site - 1 ) || (*pfirst=='.')) {
            if(ul_is_ignore(pfirst+1)==0) {
                len=size-1>ptail-pfirst-1?ptail-pfirst-1:size-1;
                memcpy(domain,pfirst+1,len);
                domain[len]=0;
                return pfirst+1;
            } else {
                ptail=pfirst;
            }
        }
        pfirst--;
    }
end:
    strncpy(domain,site,size-1);
    domain[size-1]=0;
    return site;
}

int ul_isnormalized_url(const char *url) {
    const char *pin;
    //char  hex[2];  //comment by Chen Jingkai at 2002:04:05
    char  port[6];
    int   k = 0;
    unsigned uport;
    int dotcount=0;

    if(strncasecmp(url,"http://",7) == 0)
        return 0; // should not start with "http://"
    else
        pin = url;

    //check host name
    while(UL_URL_LET_DIG_HY[(unsigned char)*pin]) { // [0-9a-zA-Z\-]
        if(UL_URL_UP_LET[(unsigned char)*pin]) //[A-Z]
            return 0; // site should be lower case.
        pin ++;
    }
    pin --;
    if(!UL_URL_LET_DIGIT[(unsigned char)*pin++]) //[0-9a-zA-Z]
        return 0; //not end with "-"
    while(*pin == '.') {
        dotcount++;
        pin ++;
        while(UL_URL_LET_DIG_HY[(unsigned char)*pin]) {
            if(UL_URL_UP_LET[(unsigned char)*pin])
                return 0; // site should be lower case.
            pin ++;
        }
        pin --;
        if(!UL_URL_LET_DIGIT[(unsigned char)*pin++])
            return 0;
    }
    if(dotcount == 0) {
        return 0;
    }
    //check host name over

    //check port
    if(*pin == ':') {
        pin ++;
        if(*pin != '/') {
            while(UL_URL_DIGIT[(unsigned char)*pin]) {
                port[k++] = *pin++;
                if(k>5)
                    return 0;
            }
            port[k] = 0;
            if(*pin != '/')
                return 0;

            uport = atoi(port);
            if (uport <= 0 || uport > 65535)
                return 0;

            if (uport == 80)
                return 0; // should omit port 80
        } else
            return 0;
    } else if( (*pin != '/'))
        return 0;
    //check port over

    //check abs_path
    if(*pin == '/') {
        pin ++;
        //path
        while(UL_URL_PCHAR[(unsigned char)*pin]) {
            pin++;
        }
        while(*pin == '/') {
            pin ++;
            while(UL_URL_PCHAR[(unsigned char)*pin]) {
                pin ++;
            }
        }


        // ";" params  == *(";" param)
        while(*pin == ';') {
            pin ++;
            while(UL_URL_PARAM[(unsigned char)*pin]) {
                pin ++;
            }
        }

        //"?" query
        while(*pin == '?') {
            pin ++;
            while(UL_URL_UCH_RES[(unsigned char)*pin]) {
                pin ++;
            }
        }
        if(*pin != 0)
            return 0;
        if ( pin-url >= UL_MAX_URL_LEN )
            return 0;
        return 1;
    } else
        return 0;

    //check abs_path over
}

int ul_normalize_site(char *site) {
    int dotcount = 0;
    char *pin = site;
    char *pout = site;
    if(!UL_URL_LET_DIG_HY[(unsigned char)*pin]) //[0-9a-zA-Z\-]
        return 0; //must start with "0-9a-zA-Z\-"

    while(UL_URL_LET_DIG_HY[(unsigned char)*pin] || *pin=='%') { // [0-9a-zA-Z\-]
        if(UL_URL_UP_LET[(unsigned char)*pin]) //[A-Z]
            *pin += 32;
        else {
            if(*pin=='%') {
                if(pin[1]!='\0' && pin[2]!='\0') {
                    unsigned char escape_char = ul_to_char(pin+1) ;
                    if(UL_URL_LET_DIG_HY[escape_char]) {
                        if (UL_URL_UP_LET[escape_char]) {
                            pin[2] = escape_char+32;
                        } else {
                            pin[2] = escape_char;
                        }
                        pin+=2 ;
                    } else {
                        return 0 ;
                    }
                } else {
                    return 0 ;
                }
            }
        }
        *pout++ = *pin++;
    }
    if (pin == site)
        return 0;

    pin --;
    if(!UL_URL_LET_DIGIT[(unsigned char)*pin++]) //[0-9a-zA-Z]
        return 0; //not end with "-"
    while(*pin == '.') {
        dotcount++;
        *pout++=*pin ++;
        while(UL_URL_LET_DIG_HY[(unsigned char)*pin] || (*pin=='%')) {
            if(UL_URL_UP_LET[(unsigned char)*pin])
                *pin += 32;
            else {
                if(*pin=='%') {
                    if(pin[1]!='\0' && pin[2]!='\0') {
                        unsigned char escape_char = ul_to_char(pin+1) ;
                        if(UL_URL_LET_DIG_HY[escape_char]==1) {
                            if (UL_URL_UP_LET[escape_char]) {
                                pin[2] = escape_char + 32;
                            } else {
                                pin[2] = escape_char;
                            }
                            pin+=2 ;
                        } else {
                            return 0 ;
                        }
                    } else {
                        return 0 ;
                    }
                }
            }
            *pout++=*pin++;
        }
        pin --;
        if(!UL_URL_LET_DIGIT[(unsigned char)*pin++])
            return 0;
    }
    if(dotcount == 0) {
        return 0;
    }
    if (*pin != 0)
        return 0;
    *pout = '\0';
    return 1;
}

int ul_normalize_site_special_for_ps_20101026(char *site) {
    //加入这个函数是一个很扯淡的原因
    //因为在ul_dict里面的create_url_sign的话依赖了ul_normalize_url_ex2
    //而在normalize_url_ex2里面依赖修改这个函数的行为
    //所以我们需要增加这个接口函数.
    //相对于url_normalize_site部分的变动会使用(MODIFY)标记.
    //要求加入这个功能的接口人是刘成城[PSSearcher]
    int dotcount = 0;
    char *pin = site;
    char *pout = site;
    if(!UL_URL_LET_DIG_HY[(unsigned char)*pin]) //[0-9a-zA-Z\-]
        return 0; //must start with "0-9a-zA-Z\-"

    while(UL_URL_LET_DIG_HY[(unsigned char)*pin] || *pin=='%') { // [0-9a-zA-Z\-]
        if(UL_URL_UP_LET[(unsigned char)*pin]) //[A-Z]
            *pin += 32;
        else {
            if(*pin=='%') {
                if(pin[1]!='\0' && pin[2]!='\0') {
                    unsigned char escape_char = ul_to_char(pin+1) ;
                    if(UL_URL_LET_DIG_HY[escape_char]) {
                        if (UL_URL_UP_LET[escape_char]) {
                            pin[2] = escape_char+32;
                        } else {
                            pin[2] = escape_char;
                        }
                        pin+=2 ;
                    } else {
                        return 0 ;
                    }
                } else {
                    return 0 ;
                }
            }
        }
        *pout++ = *pin++;
    }
    if (pin == site)
        return 0;

    pin --;

    //MODIFY.现在允许最后出现-
    //if(!UL_URL_LET_DIGIT[(unsigned char)*pin++]) //[0-9a-zA-Z]
    //return 0; //not end with "-"
    if(!UL_URL_LET_DIG_HY[(unsigned char)*pin++]){ //[0-9a-zA-Z\-]
        return 0;
    }
    
    while(*pin == '.') {
        dotcount++;
        *pout++=*pin ++;
        while(UL_URL_LET_DIG_HY[(unsigned char)*pin] || (*pin=='%')) {
            if(UL_URL_UP_LET[(unsigned char)*pin])
                *pin += 32;
            else {
                if(*pin=='%') {
                    if(pin[1]!='\0' && pin[2]!='\0') {
                        unsigned char escape_char = ul_to_char(pin+1) ;
                        if(UL_URL_LET_DIG_HY[escape_char]==1) {
                            if (UL_URL_UP_LET[escape_char]) {
                                pin[2] = escape_char + 32;
                            } else {
                                pin[2] = escape_char;
                            }
                            pin+=2 ;
                        } else {
                            return 0 ;
                        }
                    } else {
                        return 0 ;
                    }
                }
            }
            *pout++=*pin++;
        }
        pin --;
        //MODIFY.现在允许最后出现-
        //if(!UL_URL_LET_DIGIT[(unsigned char)*pin++])
        //return 0;
        if(!UL_URL_LET_DIG_HY[(unsigned char)*pin++]){ //[0-9a-zA-Z\-]
            return 0;
        }
    }
    
    if(dotcount == 0) {
        return 0;
    }
    if (*pin != 0)
        return 0;
    *pout = '\0';
    return 1;
}


int ul_normalize_port(char *port) {
    char *pin = port;
    unsigned uport;
    int k = 0;

    if (*port == 0)
        return 1;

    while(UL_URL_DIGIT[(unsigned char)*pin]) {
        k++;
        pin++;
        if(k>5)
            return 0;
    }
    if (*pin != 0)
        return 0;
    uport = atoi(port);
    if (uport <= 0 || uport > 65535)
        return 0;

    if (uport != 80)
        sprintf(port, "%d", uport);
    else
        port[0] = '\0';
    return 1;
}

int ul_normalize_url(const char* url, char* buf) {
    char site[UL_MAX_SITE_LEN];
    char port[UL_MAX_PORT_LEN];
    char path[UL_MAX_PATH_LEN];

    if (ul_parse_url(url, site, port, path) == 0)
        return 0;

    if (ul_normalize_site(site) == 0)
        return 0;
    if (ul_normalize_port(port) == 0)
        return 0;
    //这里调用ul_single_path_nointer而不调用ul_single_path
    //ul_single_path_nointer里没有ul_delete_inter操作,在ul_parse_url
    //里面已经作过一次ul_parse_url这里没必要再作一次
    //ul_single_path为了支持外部程序中直接调用ul_single_path对未经过ul_parse_url
    //处理的path进行处理，加上了ul_delete_inter
    if (ul_single_path_nointer(path) == 0)
        return 0;
    ul_normalize_path(path);

    if (port[0] == '\0') {
        snprintf(buf, UL_MAX_URL_LEN, "%s%s", site, path);
    } else {
        snprintf(buf, UL_MAX_URL_LEN, "%s:%s%s", site, port, path);
    }
    return 1;
}

int ul_parse_url(char *input,char *site,char *port,char *path) {
    return ul_parse_url((const char*)input, site, port, path);
}

char *ul_get_path(char *url, char *path) {
    return ul_get_path((const char *)url, path);
}

char *ul_get_site(char *url, char *site) {
    return ul_get_site((const char *)url, site);
}

int ul_get_port(char* url,int* pport) {
    return ul_get_port((const char* )url, pport);
}

void ul_get_static_part(char *url, char *staticurl) {
    ul_get_static_part((const char *)url, staticurl);
}


int ul_parse_url_ex(const char *input, char *site,size_t site_size,
                    char *port, size_t port_size,char *path, size_t max_path_size) {
    char              tmp[UL_MAX_URL_LEN_EX];
    char              *pin = tmp;
    char              *p,*q;
    char              *p_query;

    if(strlen(input) >= UL_MAX_URL_LEN_EX) {
        ul_writelog(UL_LOG_DEBUG,"ul_parse_url_ex: url is too long");
        return 0;
    }

    strcpy(tmp,input);
    ul_delete_inter(tmp);

    if(strncasecmp(pin,"http://",7) == 0)
        pin+=7;

    /*get path*/
    p = strchr(pin,'/');
    p_query = strchr(pin,'?');
    if(NULL==p) {
        p = p_query;
    }
    if ((NULL!=p_query) && (p_query<=p)) {
        if(path!=NULL) {
            path[0]='\0' ;
            if(strlen(p_query)<max_path_size-1) {
                strcpy(path,"/") ;
                strcat(path,p_query) ;
            } else
                return 0 ;
        }
        *p_query='\0' ;
    } else {
        if(p != NULL) {
            if (path != NULL) {
                path[0] = '\0';
                if(strlen(p) < max_path_size)
                    strcpy(path,p);
                else
                    return 0;
            }
            *p = '\0';
        } else {
            if (path != NULL) {
                strcpy(path,"/");
            }
        }
    }

    q = strchr(pin,':');
    /*get port*/

    if(q != NULL) {
        if (port != NULL) {
            port[0] = '\0';
            if(strlen(q) < port_size && atoi(q+1) > 0) {
                strcpy(port,q+1);
            } else
                return 0;
        }
        *q = '\0';
    } else {
        if (port != NULL)
            port[0] = '\0';

    }
    /*check if the default port*/
    if((port!=NULL)&&(strncmp(port,"80",3))==0) {
        port[0]=0;
    }
    /*get site*/
    if (site != NULL) {
        site[0] = '\0';
        if(pin[0] != '\0' && strlen(pin) < site_size) {
            strcpy(site,pin);
        } else {
            ul_writelog(UL_LOG_DEBUG,"ul_parse_url_ex: site name too long or empty url[%s]",pin);
            return 0;
        }

    }
    return 1;

}

char *ul_get_path_ex(const char *url, char *path,size_t path_size) {
    if (ul_parse_url_ex(url, NULL,0, NULL,0, path,path_size) == 0)
        return NULL;
    return path;

}

char *ul_get_site_ex(const char *url, char *site,  size_t site_size) {
    if (ul_parse_url_ex(url, site,site_size, NULL,0, NULL,0) == 0)
        return NULL;
    return site;

}

int ul_get_port_ex(const char* url, int* pport) {
    char strport[UL_MAX_PORT_LEN];
    if(ul_parse_url_ex(url,NULL,0,strport,UL_MAX_PORT_LEN,NULL,0)== 1) {
        if(strlen(strport) == 0) {
            *pport=80;
            return 1;
        } else {
            *pport=atoi(strport);
            return 1;
        }
    } else
        return 0;
}


int ul_normalize_url_ex(const char* url, char* buf,size_t buf_size) {
    char site[UL_MAX_SITE_LEN_EX];
    char port[UL_MAX_PORT_LEN];
    char path[UL_MAX_PATH_LEN_EX];

    if (ul_parse_url_ex(url, site,UL_MAX_SITE_LEN_EX, port,UL_MAX_PORT_LEN, path,UL_MAX_PATH_LEN_EX) == 0)
        return 0;

    if (ul_normalize_site(site) == 0)
        return 0;
    if (ul_normalize_port(port) == 0)
        return 0;
    //这里调用ul_single_path_ex_nointer而不调用ul_single_path_ex
    //ul_single_path_ex_nointer里没有ul_delete_inter操作,在ul_parse_url_ex
    //里面已经作过一次ul_parse_url_ex这里没必要再作一次
    //ul_single_path_ex为了支持外部程序中直接调用ul_single_path_ex对未经过ul_parse_url_ex
    //处理的path进行处理，加上了ul_delete_inter
    if (ul_single_path_ex_nointer(path, UL_MAX_PATH_LEN_EX) == 0)
        return 0;
    ul_normalize_path_ex(path);

    if (port[0] == '\0') {
        snprintf(buf, buf_size, "%s%s", site, path);
    } else {
        snprintf(buf, buf_size, "%s:%s%s", site, port, path);
    }
    return 1;
}

void ul_get_static_part_ex(const char *url , char *staticurl ,size_t staticurl_size) {
    char *p;
    char buffer[UL_MAX_URL_LEN_EX];

    assert(strlen(url) < UL_MAX_URL_LEN_EX);
    strcpy(buffer, url);

    p = strchr(buffer, '?');
    if (p != NULL) {
        *p = '\0';
    }
    p = strchr(buffer, ';');
    if (p != NULL) {
        *p = '\0';
    }
    snprintf(staticurl, staticurl_size,"%s", buffer);
}
int ul_isnormalized_url_ex(const char *url) {
    const char *pin;
    //char  hex[2];  //comment by Chen Jingkai at 2002:04:05
    char  port[6];
    int   k = 0;
    unsigned uport;
    int dotcount=0;

    if(strncasecmp(url,"http://",7) == 0)
        return 0; // should not start with "http://"
    else
        pin = url;

    //check host name
    while(UL_URL_LET_DIG_HY[(unsigned char)*pin]) { // [0-9a-zA-Z\-]
        if(UL_URL_UP_LET[(unsigned char)*pin]) //[A-Z]
            return 0; // site should be lower case.
        pin ++;
    }
    pin --;
    if(!UL_URL_LET_DIGIT[(unsigned char)*pin++]) //[0-9a-zA-Z]
        return 0; //not end with "-"
    while(*pin == '.') {
        dotcount++;
        pin ++;
        while(UL_URL_LET_DIG_HY[(unsigned char)*pin]) {
            if(UL_URL_UP_LET[(unsigned char)*pin])
                return 0; // site should be lower case.
            pin ++;
        }
        pin --;
        if(!UL_URL_LET_DIGIT[(unsigned char)*pin++])
            return 0;
    }
    if(dotcount == 0) {
        return 0;
    }
    //check host name over

    //check port
    if(*pin == ':') {
        pin ++;
        if(*pin != '/') {
            while(UL_URL_DIGIT[(unsigned char)*pin]) {
                port[k++] = *pin++;
                if(k>5)
                    return 0;
            }
            port[k] = 0;
            if(*pin != '/')
                return 0;

            uport = atoi(port);
            if (uport <= 0 || uport > 65535)
                return 0;

            if (uport == 80)
                return 0; // should omit port 80
        } else
            return 0;
    } else if( (*pin != '/'))
        return 0;
    //check port over

    //check abs_path
    if(*pin == '/') {
        pin ++;
        //path
        while(UL_URL_PCHAR[(unsigned char)*pin]) {
            pin++;
        }
        while(*pin == '/') {
            pin ++;
            while(UL_URL_PCHAR[(unsigned char)*pin]) {
                pin ++;
            }
        }


        // ";" params  == *(";" param)
        while(*pin == ';') {
            pin ++;
            while(UL_URL_PARAM[(unsigned char)*pin]) {
                pin ++;
            }
        }

        //"?" query
        while(*pin == '?') {
            pin ++;
            while(UL_URL_UCH_RES[(unsigned char)*pin]) {
                pin ++;
            }
        }
        if(*pin != 0)
            return 0;
        if ( (pin-url) >= UL_MAX_URL_LEN_EX )
            return 0;
        return 1;
    } else
        return 0;

    //check abs_path over
}

void ul_normalize_path_ex(char *path) {
    char *p, *q;
    char tmp[UL_MAX_URL_LEN_EX];
    char *pend;
    size_t len = 0;

    assert(*path == '/');
    len = strlen(path);
    assert(len < UL_MAX_URL_LEN_EX);

    replace_slash(path);

    strcpy(tmp, path);

    pend = strchr(tmp, '?');
    if (pend == NULL) {
        pend = strchr(tmp, ';');
    }
    if (pend == NULL) {
        pend = tmp + strlen(tmp);
    }

    p = tmp+1;
    q = path;
    while (p != pend) {
        /* normal */
        if (*q != '/') {
            q++;
            *q = *p++;
            continue;
        }

        /* /./ */
        if (strncmp(p, "./", 2)==0) {
            p += 2;
            continue;
        }

        /* /../ */
        if (strncmp(p, "../", 3)==0) {
            p += 3;
            if (q == path) {
                continue;
            }
            /* change q to prev '/' */
            q--;
            while (*q != '/')
                q--;
            continue;
        }

        /* // */
        if (*p=='/') {
            p++;
            continue;
        }

        /* "/." */
        if (strncmp(p, ".", pend-p)==0) {
            p++;
            continue;
        }

        /* "/.." */
        if (strncmp(p, "..", pend-p)==0) {
            p += 2;
            if (q == path) {
                continue;
            }
            /* change q to prev '/' */
            q--;
            while (*q != '/')
                q--;
            continue;
        }

        q++;
        *q = *p++;

    }
    q++;
    strcpy(q, p);
    return ;
}

// add this check interface to let fragment part like "#xxxx" pass the check
// ul_check_url_vhplink_modify_url(static) : check url with modify
// ul_check_url_vhplink(exported) : check url without modifying the org url


static int ul_check_url_vhplink_modify_url(char *url) {

    char *pin;
    char *pout;
    //char  hex[2];  //comment by Chen Jingkai at 2002:04:05
    char  port[6];
    int   k = 0;
    int dotcount=0;
    char *url_start;
    int len;
    int space_flag = 0;
    len = ul_delete_inter(url); //delete \r\n;
    if(strncasecmp(url,"http://",7) == 0) {
        pin = url + 7;
    } else {
        pin = url;
    }
    pout = pin;
    url_start = pin;
    //check host name
    while(UL_URL_LET_DIG_HY[(unsigned char)*pin] ||*pin=='%') {
        if(UL_URL_UP_LET[(unsigned char)*pin])
            *pin += 32;
        else {
            if(*pin=='%') {
                if(pin[1]!='\0' && pin[2]!='\0') {
                    unsigned char escape_char = ul_to_char(pin+1) ;
                    if(UL_URL_LET_DIG_HY[escape_char]) {
                        if (UL_URL_UP_LET[escape_char]) {
                            pin[2] = escape_char + 32;
                        } else {
                            pin[2] = escape_char;
                        }
                        pin+=2 ;
                    } else {
                        return 0 ;
                    }
                } else {
                    return 0 ;
                }
            }
        }
        *pout++=*pin++;
    }
    if(pin == url_start)
        return 0;

    pin --;
    if(!UL_URL_LET_DIGIT[(unsigned char)*pin++])
        return 0;
    while(*pin == '.') {
        dotcount++;
        *pout++=*pin++;
        while(UL_URL_LET_DIG_HY[(unsigned char)*pin] ||(*pin=='%')) {
            if(UL_URL_UP_LET[(unsigned char)*pin])
                *pin += 32;
            else {
                if(*pin=='%') {
                    if(pin[1]!='\0' && pin[2]!='\0') {
                        unsigned char escape_char = ul_to_char(pin+1) ;
                        if(UL_URL_LET_DIG_HY[escape_char]==1) {
                            if (UL_URL_UP_LET[escape_char]) {
                                pin[2] = escape_char + 32;
                            } else {
                                pin[2] = escape_char;
                            }
                            pin+=2 ;
                        } else {
                            return 0 ;
                        }
                    } else {
                        return 0 ;
                    }
                }
            }
            *pout++=*pin++;
        }
        pin --;
        if(!UL_URL_LET_DIGIT[(unsigned char)*pin++])
            return 0;
    }
    if(dotcount == 0) {
        return 0;
    }
    //check host name over

    //check port
    if(*pin == ':') {
        pin ++;
        if(*pin != '/') {
            while(UL_URL_DIGIT[(unsigned char)*pin]) {
                port[k++] = *pin++;
                if(k>5)
                    return 0;
            }
            port[k] = 0;
            if(*pin != '/')
                return 0;
            if(strcmp(port,"80") != 0) {
                memcpy(pout, pin-k-1, k+1);
                pout += (k+1);
            }
        }
    } else if( (*pin != '/') && (*pin !='\0' ) ){
        if( (*pin != '?') && (*pin != '#') ){
			return 0;
		}
	}
    //check port over

    //check abs_path
    if(*pin == '/') {
        *pout++=*pin++;
        //path
        while(UL_URL_PCHAR[(unsigned char)*pin]) {
            space_flag|=((*pout++=*pin++)==' ');
        }

        while(*pin == '/') {
            *pout++=*pin++;
            while(UL_URL_PCHAR[(unsigned char)*pin]) {
                space_flag|=((*pout++=*pin++)==' ');
            }
        }

        // ";" params  == *(";" param)
        while(*pin == ';') {
            *pout++=*pin++;
            while(UL_URL_PARAM[(unsigned char)*pin]) {
                space_flag|=((*pout++=*pin++)==' ');
            }
        }

        //"?" query
        while(*pin == '?') {
            *pout++=*pin++;
            while(UL_URL_UCH_RES[(unsigned char)*pin]) {
                space_flag|=((*pout++=*pin++)==' ');
            }
        }

		//"#" fragment (by wangzhipeng)
		while(*pin == '#'){
			*pout++=*pin++;
			while(UL_URL_UCH_RES[(unsigned char)*pin]) {
				space_flag|=((*pout++=*pin++)==' '); 
			}
		}

		
		
        if(*pin != 0)
            return 0;
        *pout = 0;
        if (pout-url > len)
            return 0;
        if(space_flag==1)
            return ul_space_escape(url ,UL_MAX_URL_LEN, UL_MAX_SITE_LEN, UL_MAX_PATH_LEN);
        else
            return 1;
    }else if( *pin == '?' || *pin == '#' ){ 

        //"?" query
        while(*pin == '?') {
            *pout++=*pin++;
            while(UL_URL_UCH_RES[(unsigned char)*pin]) {
                space_flag|=((*pout++=*pin++)==' ');
            }
        }

		//"#" fragment (by wangzhipeng)
		while(*pin == '#'){
			*pout++=*pin++;
			while(UL_URL_UCH_RES[(unsigned char)*pin]) {
				space_flag|=((*pout++=*pin++)==' '); 
			}
		}
		
        if(*pin != 0)
            return 0;
        *pout = 0;
        if (pout-url > len)
            return 0;
        if(space_flag==1)
            return ul_space_escape(url ,UL_MAX_URL_LEN, UL_MAX_SITE_LEN, UL_MAX_PATH_LEN);
        else
            return 1;
		
		
	}else if(*pin != 0)
        return 0;
    if (space_flag==1)
        return ul_space_escape(url , UL_MAX_URL_LEN, UL_MAX_SITE_LEN, UL_MAX_PATH_LEN);
    else
        return 1;

    //check abs_path over
}

int ul_check_url_vhplink(const char *url){

	char tmp_url[UL_MAX_URL_LEN];
	
	strcpy( tmp_url, url );
	
	return ul_check_url_vhplink_modify_url(tmp_url);
}

// parse the url , add a new param to output fragment independently

int ul_parse_url_vhplink(const char *input,char *site,char *port,char *path,char *frag) {
    char              tmp[UL_MAX_URL_LEN],tmp2[UL_MAX_URL_LEN];
    char              *pin = tmp;
    char              *p,*q;
    char              *p_query = NULL;
	char			  *p_frag = NULL;
	char			  *p_front = NULL;
	
    if(strlen(input) >= UL_MAX_URL_LEN) {
        ul_writelog(UL_LOG_DEBUG,"ul_parse_url: url is too long");
        return 0;
    }

    strcpy(tmp,input);
    ul_delete_inter(tmp);

    if(strncasecmp(pin,"http://",7) == 0)
        pin+=7;

    p = strchr(pin,'/');
    p_query = strchr(pin,'?');
	p_frag = strchr(pin,'#');

	
	if( p_query != NULL ){
		p_front = p_query;
	}
	if( p_frag != NULL ){
		if( (p_front == NULL) || (p_front > p_frag) ){
			p_front = p_frag;
		}
	}

	if( p_front != NULL ){
		if( p == NULL || p_front <= p ){
			p = p_front;
			strcpy(tmp2,p);
			*p = '/';
			strcpy(p+1,tmp2);
		}
	}

	//get path
	p = strchr(pin,'/');	
	p_query = strchr(pin,'?');
	
    if(NULL==p) {
        p = p_query;
    }
    if ((NULL!=p_query) && (p_query<=p)) {
        if(path!=NULL) {
            path[0]='\0';
            if(strlen(p_query)<UL_MAX_PATH_LEN-1) {
                ul_strlcpy(path,"/",2);
                strcat(path,p_query);
            } else
                return 0;
        }
        *p_query='\0' ;
    } else {
        if(p != NULL) {
            if (path != NULL) {
                path[0] = '\0';
                if(strlen(p) < UL_MAX_PATH_LEN)
                    strcpy(path,p);
                else
                    return 0;
            }
            *p = '\0';
        } else {
            if (path != NULL) {
                strcpy(path,"/");
            }
        }
    }

	//if path has fragment like "#xxx", split it
	if( path != NULL && frag != NULL ){
		char *fg;

		fg = strchr(path, '#');
		if( fg != NULL ){ 
			// has fragment, split
			if( strlen(fg) < UL_MAX_FRAG_LEN ){
			    strcpy(frag, fg);
		        *fg = '\0';
			}
			else{
				return 0;
			}	
		}
		else{
		   // no fragment	
			frag[0] = '\0';
		}
	}

    q = strchr(pin,':');
    //get port

    if(q != NULL) {
        if (port != NULL) {
            port[0] = '\0';
            if(strlen(q) < UL_MAX_PORT_LEN && atoi(q+1) > 0) {
                strcpy(port,q+1);
            } else
                return 0;
        }
        *q = '\0';
    } else {
        if (port != NULL)
            port[0] = '\0';

    }
    //check if the default port
    if((port!=NULL)&&(strncmp(port,"80",3))==0) {
        port[0]=0;
    }
    //get site
    if (site != NULL) {
        site[0] = '\0';
        if(pin[0] != '\0' && strlen(pin) < UL_MAX_SITE_LEN) {
            strcpy(site,pin);
        } else {
            ul_writelog(UL_LOG_DEBUG,"ul_parse_url: site name too long or empty url[%s]",pin);
            return 0;
        }

    }
    return 1;

}
// normalize path segment .
// change:  if path has "http://", keep it
void ul_normalize_path_vhplink(char *path) {
    char *p, *q;
    char tmp[UL_MAX_URL_LEN];
    char *pend;
	char *cur_root;//因为path带入了"http:\\"的处理，所以path root可能会变

    assert(*path == '/');

    replace_slash(path);

    strcpy(tmp, path);

    pend = strchr(tmp, '?');
    if (pend == NULL) {
        pend = strchr(tmp, ';');
    }
    if (pend == NULL) {
        pend = tmp + strlen(tmp);
    }
	
    p = tmp+1;
    q = path;
	cur_root = path;
    while (p != pend) {
        /* normal */
        if (*q != '/') {
            q++;
            *q = *p++;
            continue;
        }

        /* /./ */
        if (strncmp(p, "./", 2)==0) {
            p += 2;
            continue;
        }

        /* /../ */
        if (strncmp(p, "../", 3)==0) {
            p += 3;
            if (q == cur_root) {
                continue;
            }
            /* change q to prev '/' */
            q--;
            while (*q != '/')
                q--;
            continue;
        }

        /* // */
        if (*p=='/') {
			// 如果是"http://"里的两个斜线,则保留不减少,
			if( (p-6 >= tmp) && (strncasecmp(p-6, "http://", 7) == 0) ){
				q++;
				*q = *p++;
				cur_root = q;//root path指向新的http://的第二个斜线处
				continue;
			}
			else{
				p++;
				continue;
			}
        }

        /* "/." */
        if (strncmp(p, ".", pend-p)==0) {
            p++;
            continue;
        }

        /* "/.." */
        if (strncmp(p, "..", pend-p)==0) {
            p += 2;
            if (q == path) {
                continue;
            }
            /* change q to prev '/' */
            q--;
            while (*q != '/')
                q--;
            continue;
        }

        q++;
        *q = *p++;

    }
    q++;
    strcpy(q, p);
    return ;
}


int ul_check_url_ex(char *url) {
    char *pin;
    char *pout;
    //char  hex[2];  //comment by Chen Jingkai at 2002:04:05
    char  port[6];
    int   k = 0;
    int dotcount=0;
    char *url_start;
    int len;
    int space_flag = 0;
    len = ul_delete_inter(url); //delete \r\n;
    if(strncasecmp(url,"http://",7) == 0) {
        pin = url + 7;
    } else {
        pin = url;
    }
    pout = pin;
    url_start = pin;
    //check host name
    while(UL_URL_LET_DIG_HY[(unsigned char)*pin] ||*pin=='%') {
        if(UL_URL_UP_LET[(unsigned char)*pin])
            *pin += 32;
        else {
            if(*pin=='%') {
                if(pin[1]!='\0' && pin[2]!='\0') {
                    unsigned char escape_char = ul_to_char(pin+1) ;
                    if(UL_URL_LET_DIG_HY[escape_char]) {
                        if (UL_URL_UP_LET[escape_char]) {
                            pin[2] = escape_char+32;
                        } else {
                            pin[2] = escape_char;
                        }
                        pin+=2 ;
                    } else {
                        return 0 ;
                    }
                } else {
                    return 0 ;
                }
            }
        }
        *pout++=*pin++;
    }
    if(pin == url_start)
        return 0;

    pin --;
    if(!UL_URL_LET_DIGIT[(unsigned char)*pin++])
        return 0;
    while(*pin == '.') {
        dotcount++;
        *pout++=*pin++;
        while(UL_URL_LET_DIG_HY[(unsigned char)*pin] ||(*pin=='%')) {
            if(UL_URL_UP_LET[(unsigned char)*pin])
                *pin += 32;
            else {
                if(*pin=='%') {
                    if(pin[1]!='\0' && pin[2]!='\0') {
                        unsigned char escape_char = ul_to_char(pin+1) ;
                        if(UL_URL_LET_DIG_HY[escape_char]==1) {
                            if (UL_URL_UP_LET[escape_char]) {
                                pin[2] = escape_char + 32;
                            } else {
                                pin[2] = escape_char;
                            }
                            pin+=2 ;
                        } else {
                            return 0 ;
                        }
                    } else {
                        return 0 ;
                    }
                }
            }
            *pout++=*pin++;
        }
        pin --;
        if(!UL_URL_LET_DIGIT[(unsigned char)*pin++])
            return 0;
    }
    if(dotcount == 0) {
        return 0;
    }
    //check host name over

    //check port
    if(*pin == ':') {
        pin ++;
        if(*pin != '/') {
            while(UL_URL_DIGIT[(unsigned char)*pin]) {
                port[k++] = *pin++;
                if(k>5)
                    return 0;
            }
            port[k] = 0;
            if(*pin != '/')
                return 0;
            memcpy(pout, pin-k-1, k+1);
            if(strcmp(port,"80") != 0) {
                pout += (k+1);
            }
        }
    } else if( (*pin != '/') && (*pin !='\0' ))
        return 0;
    //check port over

    //check abs_path
    if(*pin == '/') {
        *pout++=*pin++;
        //path
        while(UL_URL_PCHAR[(unsigned char)*pin]) {
            space_flag|=((*pout++=*pin++)==' ');
        }

        while(*pin == '/') {
            *pout++=*pin++;
            while(UL_URL_PCHAR[(unsigned char)*pin]) {
                space_flag|=((*pout++=*pin++)==' ');
            }
        }

        // ";" params  == *(";" param)
        while(*pin == ';') {
            *pout++=*pin++;
            while(UL_URL_PARAM[(unsigned char)*pin]) {
                space_flag|=((*pout++=*pin++)==' ');
            }
        }

        //"?" query
        while(*pin == '?') {
            *pout++=*pin++;
            while(UL_URL_UCH_RES[(unsigned char)*pin]) {
                space_flag|=((*pout++=*pin++)==' ');
            }
        }
        if(*pin != 0)
            return 0;
        *pout = 0;
        if (pout-url > len)
            return 0;
        if(space_flag==1)
            return ul_space_escape(url ,UL_MAX_URL_LEN_EX, UL_MAX_SITE_LEN_EX, UL_MAX_PATH_LEN_EX);
        else
            return 1;
    } else  if (*pin != 0)
        return 0;
    *pout = '\0';
    if (space_flag==1)
        return ul_space_escape(url , UL_MAX_URL_LEN_EX, UL_MAX_SITE_LEN_EX, UL_MAX_PATH_LEN_EX);
    else
        return 1;

    //check abs_path over
}

//##########################################################
//##########################################################
//##########################################################
//the difference to 'ul_parse_url_ex' is
//UL_MAX_URL_LEN_EX->UL_MAX_URL_LEN_EX2
int ul_parse_url_ex2(const char *input, char *site,size_t site_size,
                     char *port, size_t port_size,char *path, size_t max_path_size) {
    char              tmp[UL_MAX_URL_LEN_EX2];
    char              *pin = tmp;
    char              *p,*q;
    char              *p_query;

    if(strlen(input) >= UL_MAX_URL_LEN_EX2) {
        ul_writelog(UL_LOG_DEBUG,"ul_parse_url_ex2: url is too long");
        return 0;
    }

    strcpy(tmp,input);
    ul_delete_inter(tmp);

    if(strncasecmp(pin,"http://",7) == 0)
        pin+=7;

    /*get path*/
    p = strchr(pin,'/');
    p_query = strchr(pin,'?');
    if(NULL==p) {
        p = p_query;
    }
    if ((NULL!=p_query) && (p_query<=p)) {
        if(path!=NULL) {
            path[0]='\0' ;
            if(strlen(p_query)<max_path_size-1) {
                ul_strlcpy(path,"/",2) ;
                strcat(path,p_query) ;
            } else
                return 0 ;
        }
        *p_query='\0' ;
    } else {
        if(p != NULL) {
            if (path != NULL) {
                path[0] = '\0';
                if(strlen(p) < max_path_size)
                    strcpy(path,p);
                else
                    return 0;
            }
            *p = '\0';
        } else {
            if (path != NULL) {
                strcpy(path,"/");
            }
        }
    }

    q = strchr(pin,':');
    /*get port*/

    if(q != NULL) {
        if (port != NULL) {
            port[0] = '\0';
            if(strlen(q) < port_size && atoi(q+1) > 0) {
                strcpy(port,q+1);
            } else
                return 0;
        }
        *q = '\0';
    } else {
        if (port != NULL)
            port[0] = '\0';

    }
    /*check if the default port*/
    if((port!=NULL)&&(strncmp(port,"80",3))==0) {
        port[0]=0;
    }
    /*get site*/
    if (site != NULL) {
        site[0] = '\0';
        if(pin[0] != '\0' && strlen(pin) < site_size) {
            strcpy(site,pin);
        } else {
            ul_writelog(UL_LOG_DEBUG,"ul_parse_url_ex2: site name too long or empty url[%s]",pin);
            return 0;
        }

    }
    return 1;
}

char *ul_get_path_ex2(const char *url, char *path,size_t path_size) {
    if (ul_parse_url_ex2(url, NULL,0, NULL,0, path,path_size) == 0)
        return NULL;
    return path;

}

char *ul_get_site_ex2(const char *url, char *site,  size_t site_size) {
    if (ul_parse_url_ex2(url, site,site_size, NULL,0, NULL,0) == 0)
        return NULL;
    return site;

}

int ul_get_port_ex2(const char* url, int* pport) {
    char strport[UL_MAX_PORT_LEN];
    if(ul_parse_url_ex2(url,NULL,0,strport,UL_MAX_PORT_LEN,NULL,0)== 1) {
        if(strlen(strport) == 0) {
            *pport=80;
            return 1;
        } else {
            *pport=atoi(strport);
            return 1;
        }
    } else
        return 0;
}

//escape sapce of whole url...and check the site and path length..
static int ul_space_escape_ex2(char * url,int max_url_len, int max_site_len, int max_path_len) {
    char * pin = url ;
    int  pout = 0 ;
    char url_tmp[UL_MAX_URL_LEN_EX2] ;
    while(*pin!='\0') {
        if(*pin!=' ') {
            if(pout>=max_url_len -1)
                return 0 ;
            url_tmp[pout++] = *pin++ ;
        } else { //space
            if(pout>=max_url_len-3)
                return 0 ;
            url_tmp[pout++]='%' ;
            url_tmp[pout++]='2' ;
            url_tmp[pout++]='0' ;
            pin++ ;
        }
    }
    if(pout>=max_url_len)
        return 0 ;
    url_tmp[pout]='\0' ;
    //detect site len and path len
    if(strncmp(url_tmp,"http://",7)==0) {
        pin = url_tmp+7 ;
    } else {
        pin = url_tmp ;
    }
    char * path = strchr(pin,'/') ;
    if(path==pin)
        return 0 ;
    //check path length:-)[including the slash /]
    if(path!=NULL && strlen(path)>=(u_int)max_path_len)
        return 0 ;
    char * port = strchr(pin,':') ;
    char * psite_end = NULL ;
    //ugly but I understand its meaning..:-)
    if(port==pin) //no any site..
        return 0 ;
    if(NULL!=port&&NULL!=path&&port<path) { //with a port
        psite_end=port-1 ;
    } else if(path!=NULL) { //no port just path
        psite_end = path -1 ;
    }
    //here psite_end==NULL...just a site...
    if(psite_end!=NULL && psite_end-pin>=max_site_len) {
        return 0 ;
    } else if (psite_end==NULL && strlen(pin) >= (u_int)max_site_len) {
        return 0 ;
    }
    strncpy(url,url_tmp,pout+1) ;
    return 1 ;
}


static int ul_space_escape_path_ex2(char * path,int max_path_len) {
    char * pin = path ;
    int  pout = 0 ;
    char url_tmp[UL_MAX_URL_LEN_EX2] ;
    while(*pin!='\0') {
        if(*pin!=' ') {
            if(pout>=max_path_len -1)//we have to judge here...
                return 0 ;
            url_tmp[pout++] = *pin++ ;
        } else { //space
            if(pout>=max_path_len-3)
                return 0 ;
            url_tmp[pout++]='%' ;
            url_tmp[pout++]='2' ;
            url_tmp[pout++]='0' ;
            pin++ ;
        }
    }
    if(pout>=max_path_len)
        return 0 ;
    url_tmp[pout]='\0' ;

    strncpy(path,url_tmp,pout+1) ;
    return 1 ;
}

static int ul_single_path_ex2_nointer(char *path, int len) {
    char *pin;
    int   pout;
    char  hex[2];
    char  tmp_hex[2];
    char  escape_char;
    int space_flag = 0;
    //len = strlen(path);
    pin = path;
    pout = 0;
    //check abs_path
    if(*pin == '/') {
        path[pout++] = *pin ++;
    }

    //path
    while(UL_PCHAR[(unsigned char)*pin]) {
        tmp_hex[0] = '\0';
        if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
            strncpy(hex,++pin,2);
            strncpy(tmp_hex,hex,2);
            pin ++;
            escape_char = ul_to_char(hex);
            if(escape_char == 0) {
                pin--;
                path[pout++]='%';
                continue;
            }
            *pin = escape_char;
        }
        if(UL_PCHAR[(unsigned char)*pin] &&
                (tmp_hex[0]=='\0' || !UL_RESERVED[(unsigned char)*pin])) {
            if(*pin == ' ') {
                space_flag = 1;
            }
            path[pout++] = *pin++;
        } else {
            path[pout++]='%';
            strncpy(&path[pout],tmp_hex,2);
            pout=pout+2;
            pin++;
        }
    }

    while(*pin == '/') {
        path[pout++] = *pin ++;
        while(UL_PCHAR[(unsigned char)*pin]) {
            tmp_hex[0] = '\0';
            if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
                strncpy(hex,++pin,2);
                strncpy(tmp_hex,hex,2);
                pin ++;
                escape_char = ul_to_char(hex);
                if(escape_char == 0) {
                    pin--;
                    path[pout++]='%';
                    continue;
                }
                *pin = escape_char;
            }

            if(UL_PCHAR[(unsigned char)*pin] &&
                    (tmp_hex[0]=='\0' || !UL_RESERVED[(unsigned char)*pin])) {
                if(*pin == ' ') {
                    space_flag = 1;
                }
                path[pout++] = *pin++;
            } else {
                path[pout++]='%';
                strncpy(&path[pout],tmp_hex,2);
                pout=pout+2;
                pin++;
            }
        }
    }

    // ";" params  == *(";" param)
    while(*pin == ';') {
        path[pout++] = *pin ++;
        while(UL_PARAM[(unsigned char)*pin]) {
            tmp_hex[0] = '\0';
            if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
                strncpy(hex,++pin,2);
                strncpy(tmp_hex,hex,2);
                pin ++;
                escape_char = ul_to_char(hex);
                if(escape_char == 0) {
                    pin--;
                    path[pout++]='%';
                    continue;
                }
                *pin = escape_char;
            }
            if(UL_PARAM[(unsigned char)*pin] &&
                    (tmp_hex[0]=='\0' || !UL_RESERVED[(unsigned char)*pin])) {
                if(*pin == ' ') {
                    space_flag = 1;
                }
                path[pout++] = *pin++;
            } else {
                path[pout++]='%';
                strncpy(&path[pout],tmp_hex,2);
                pout=pout+2;
                pin++;
            }
        }
    }

    //"?" query
    while(*pin == '?') {
        path[pout++] = *pin ++;
        while(UL_UCH_RES[(unsigned char)*pin]) {
            tmp_hex[0] = '\0';
            if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
                strncpy(hex,++pin,2);
                strncpy(tmp_hex,hex,2);
                pin ++;
                escape_char = ul_to_char(hex);
                if(escape_char == 0) {
                    pin--;
                    path[pout++]='%';
                    continue;
                }
                *pin = escape_char;
            }
            if(UL_UCH_RES[(unsigned char)*pin] &&
                    (tmp_hex[0]=='\0' || !UL_RESERVED[(unsigned char)*pin])) {
                if(*pin == ' ') {
                    space_flag = 1;
                }
                path[pout++] = *pin++;
            } else {
                path[pout++]='%';
                strncpy(&path[pout],tmp_hex,2);
                pout=pout+2;
                pin++;
            }
        }
    }
    //if(*pin != 0)    
    if(*pin != 0 && *pin != '#')
        return 0;    
    path[pout] = 0;
    if (pout > len )
        return 0;
    if(space_flag == 1) {
        return ul_space_escape_path_ex2(path,UL_MAX_PATH_LEN_EX2);;
    } else {
        return 1;
    }
    //check abs_path over
}


//加入这个函数的原因是因为.
//ps想不省去fragment来进行归一化.
static int ul_single_path_ex2_nointer_special_for_ps_20110221(char *path, int len) {
    char *pin;
    int   pout;
    char  hex[2];
    char  tmp_hex[2];
    char  escape_char;
    int space_flag = 0;
    //len = strlen(path);
    pin = path;
    pout = 0;
    //check abs_path
    if(*pin == '/') {
        path[pout++] = *pin ++;
    }

    //path
    while(UL_PCHAR[(unsigned char)*pin]) {
        tmp_hex[0] = '\0';
        if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
            strncpy(hex,++pin,2);
            strncpy(tmp_hex,hex,2);
            pin ++;
            escape_char = ul_to_char(hex);
            if(escape_char == 0) {
                pin--;
                path[pout++]='%';
                continue;
            }
            *pin = escape_char;
        }
        if(UL_PCHAR[(unsigned char)*pin] &&
                (tmp_hex[0]=='\0' || !UL_RESERVED[(unsigned char)*pin])) {
            if(*pin == ' ') {
                space_flag = 1;
            }
            path[pout++] = *pin++;
        } else {
            path[pout++]='%';
            strncpy(&path[pout],tmp_hex,2);
            pout=pout+2;
            pin++;
        }
    }

    while(*pin == '/') {
        path[pout++] = *pin ++;
        while(UL_PCHAR[(unsigned char)*pin]) {
            tmp_hex[0] = '\0';
            if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
                strncpy(hex,++pin,2);
                strncpy(tmp_hex,hex,2);
                pin ++;
                escape_char = ul_to_char(hex);
                if(escape_char == 0) {
                    pin--;
                    path[pout++]='%';
                    continue;
                }
                *pin = escape_char;
            }

            if(UL_PCHAR[(unsigned char)*pin] &&
                    (tmp_hex[0]=='\0' || !UL_RESERVED[(unsigned char)*pin])) {
                if(*pin == ' ') {
                    space_flag = 1;
                }
                path[pout++] = *pin++;
            } else {
                path[pout++]='%';
                strncpy(&path[pout],tmp_hex,2);
                pout=pout+2;
                pin++;
            }
        }
    }

    // ";" params  == *(";" param)
    while(*pin == ';') {
        path[pout++] = *pin ++;
        while(UL_PARAM[(unsigned char)*pin]) {
            tmp_hex[0] = '\0';
            if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
                strncpy(hex,++pin,2);
                strncpy(tmp_hex,hex,2);
                pin ++;
                escape_char = ul_to_char(hex);
                if(escape_char == 0) {
                    pin--;
                    path[pout++]='%';
                    continue;
                }
                *pin = escape_char;
            }
            if(UL_PARAM[(unsigned char)*pin] &&
                    (tmp_hex[0]=='\0' || !UL_RESERVED[(unsigned char)*pin])) {
                if(*pin == ' ') {
                    space_flag = 1;
                }
                path[pout++] = *pin++;
            } else {
                path[pout++]='%';
                strncpy(&path[pout],tmp_hex,2);
                pout=pout+2;
                pin++;
            }
        }
    }

    //"?" query
    while(*pin == '?') {
        path[pout++] = *pin ++;
        while(UL_UCH_RES[(unsigned char)*pin]) {
            tmp_hex[0] = '\0';
            if((*pin == '%') && (*(pin+1) != '\0') && (*(pin+2) !='\0')) {
                strncpy(hex,++pin,2);
                strncpy(tmp_hex,hex,2);
                pin ++;
                escape_char = ul_to_char(hex);
                if(escape_char == 0) {
                    pin--;
                    path[pout++]='%';
                    continue;
                }
                *pin = escape_char;
            }
            if(UL_UCH_RES[(unsigned char)*pin] &&
                    (tmp_hex[0]=='\0' || !UL_RESERVED[(unsigned char)*pin])) {
                if(*pin == ' ') {
                    space_flag = 1;
                }
                path[pout++] = *pin++;
            } else {
                path[pout++]='%';
                strncpy(&path[pout],tmp_hex,2);
                pout=pout+2;
                pin++;
            }
        }
    }
    //if(*pin != 0)    
    if(*pin != 0 && *pin != '#')
        return 0;
    //补齐最后面的fragment.
    while(*pin && pout<len){
        path[pout]=*pin;
        pin++;
        pout++;
    }
    //长度过大的话.
    if(pout>=len){
        return 0;
    }
    path[pout] = 0;
    if(space_flag == 1) {
        return ul_space_escape_path_ex2(path,UL_MAX_PATH_LEN_EX2);;
    } else {
        return 1;
    }
    //check abs_path over
}

int ul_normalize_url_ex2(const char* url, char* buf,size_t buf_size) {
    char site[UL_MAX_SITE_LEN_EX2];
    char port[UL_MAX_PORT_LEN];
    char path[UL_MAX_PATH_LEN_EX2];

    if (ul_parse_url_ex2(url, site,UL_MAX_SITE_LEN_EX2, port,UL_MAX_PORT_LEN, path,UL_MAX_PATH_LEN_EX2) == 0)
        return 0;

    if (ul_normalize_site(site) == 0)
        return 0;
    if (ul_normalize_port(port) == 0)
        return 0;
    //这里调用ul_single_path_ex2_nointer而不调用ul_single_path_ex2
    //ul_single_path_ex2_nointer里没有ul_delete_inter操作,在ul_parse_url_ex2
    //里面已经作过一次ul_parse_url_ex2这里没必要再作一次
    //ul_single_path_ex2为了支持外部程序中直接调用ul_single_path_ex2对未经过ul_parse_url_ex2
    //处理的path进行处理，加上了ul_delete_inter
    if (ul_single_path_ex2_nointer(path, UL_MAX_PATH_LEN_EX2) == 0)
        return 0;
    ul_normalize_path_ex2(path);

    if (port[0] == '\0') {
        snprintf(buf, buf_size, "%s%s", site, path);
    } else {
        snprintf(buf, buf_size, "%s:%s%s", site, port, path);
    }
    return 1;
}

int ul_normalize_url_ex2_special_for_ps_20110221(const char* url, char* buf,size_t buf_size) {
    //加入这个函数的原因
    //可以参看 ul_normalize_url_ex2_special_for_ps_20101026 这个函数.
    char site[UL_MAX_SITE_LEN_EX2];
    char port[UL_MAX_PORT_LEN];
    char path[UL_MAX_PATH_LEN_EX2];

    if (ul_parse_url_ex2(url, site,UL_MAX_SITE_LEN_EX2, port,UL_MAX_PORT_LEN, path,UL_MAX_PATH_LEN_EX2) == 0)
        return 0;

    if (ul_normalize_site_special_for_ps_20101026(site) == 0)
        return 0;
    if (ul_normalize_port(port) == 0)
        return 0;
    //这里调用ul_single_path_ex2_nointer而不调用ul_single_path_ex2
    //ul_single_path_ex2_nointer里没有ul_delete_inter操作,在ul_parse_url_ex2
    //里面已经作过一次ul_parse_url_ex2这里没必要再作一次
    //ul_single_path_ex2为了支持外部程序中直接调用ul_single_path_ex2对未经过ul_parse_url_ex2
    //处理的path进行处理，加上了ul_delete_inter
    //这里需要把fragment也作为归一化的内容里面去.
    if (ul_single_path_ex2_nointer_special_for_ps_20110221(path, UL_MAX_PATH_LEN_EX2) == 0)
        return 0;
    ul_normalize_path_ex2(path);

    if (port[0] == '\0') {
        snprintf(buf, buf_size, "%s%s", site, path);
    } else {
        snprintf(buf, buf_size, "%s:%s%s", site, port, path);
    }
    return 1;
}

void ul_get_static_part_ex2(const char *url , char *staticurl ,size_t staticurl_size) {
    char *p;
    char buffer[UL_MAX_URL_LEN_EX2];

    assert(strlen(url) < UL_MAX_URL_LEN_EX2);
    strcpy(buffer, url);

    p = strchr(buffer, '?');
    if (p != NULL) {
        *p = '\0';
    }
    p = strchr(buffer, ';');
    if (p != NULL) {
        *p = '\0';
    }
    snprintf(staticurl, staticurl_size,"%s", buffer);
}

//actually not modified ...
int ul_isnormalized_url_ex2(const char *url) {
    const char *pin;
    char  port[6];
    int   k = 0;
    unsigned uport;
    int dotcount=0;

    if(strncasecmp(url,"http://",7) == 0)
        return 0; // should not start with "http://"
    else
        pin = url;

    //check host name
    while(UL_URL_LET_DIG_HY[(unsigned char)*pin]) { // [0-9a-zA-Z\-]
        if(UL_URL_UP_LET[(unsigned char)*pin]) //[A-Z]
            return 0; // site should be lower case.
        pin ++;
    }
    pin --;
    if(!UL_URL_LET_DIGIT[(unsigned char)*pin++]) //[0-9a-zA-Z]
        return 0; //not end with "-"
    while(*pin == '.') {
        dotcount++;
        pin ++;
        while(UL_URL_LET_DIG_HY[(unsigned char)*pin]) {
            if(UL_URL_UP_LET[(unsigned char)*pin])
                return 0; // site should be lower case.
            pin ++;
        }
        pin --;
        if(!UL_URL_LET_DIGIT[(unsigned char)*pin++])
            return 0;
    }
    if(dotcount == 0) {
        return 0;
    }
    //check host name over

    //check port
    if(*pin == ':') {
        pin ++;
        if(*pin != '/') {
            while(UL_URL_DIGIT[(unsigned char)*pin]) {
                port[k++] = *pin++;
                if(k>5)
                    return 0;
            }
            port[k] = 0;
            if(*pin != '/')
                return 0;

            uport = atoi(port);
            if (uport <= 0 || uport > 65535)
                return 0;

            if (uport == 80)
                return 0; // should omit port 80
        } else
            return 0;
    } else if( (*pin != '/'))
        return 0;
    //check port over

    //check abs_path
    if(*pin == '/') {
        pin ++;
        //path
        while(UL_URL_PCHAR[(unsigned char)*pin]) {
            pin++;
        }
        while(*pin == '/') {
            pin ++;
            while(UL_URL_PCHAR[(unsigned char)*pin]) {
                pin ++;
            }
        }


        // ";" params  == *(";" param)
        while(*pin == ';') {
            pin ++;
            while(UL_URL_PARAM[(unsigned char)*pin]) {
                pin ++;
            }
        }

        //"?" query
        while(*pin == '?') {
            pin ++;
            while(UL_URL_UCH_RES[(unsigned char)*pin]) {
                pin ++;
            }
        }
        if(*pin != 0)
            return 0;
        if ( (pin-url) >= UL_MAX_URL_LEN_EX2 )
            return 0;
        return 1;
    } else
        return 0;

    //check abs_path over
}

void ul_normalize_path_ex2(char *path) {
    char *p, *q;
    char tmp[UL_MAX_URL_LEN_EX2];
    char *pend;
    size_t len = 0;

    assert(*path == '/');
    len = strlen(path);
    assert(len < UL_MAX_URL_LEN_EX2);

    replace_slash(path);

    strcpy(tmp, path);

    pend = strchr(tmp, '?');
    if (pend == NULL) {
        pend = strchr(tmp, ';');
    }
    if (pend == NULL) {
        pend = tmp + strlen(tmp);
    }

    p = tmp+1;
    q = path;
    while (p != pend) {
        /* normal */
        if (*q != '/') {
            q++;
            *q = *p++;
            continue;
        }

        /* /./ */
        if (strncmp(p, "./", 2)==0) {
            p += 2;
            continue;
        }

        /* /../ */
        if (strncmp(p, "../", 3)==0) {
            p += 3;
            if (q == path) {
                continue;
            }
            /* change q to prev '/' */
            q--;
            while (*q != '/')
                q--;
            continue;
        }

        /* // */
        if (*p=='/') {
            p++;
            continue;
        }

        /* "/." */
        if (strncmp(p, ".", pend-p)==0) {
            p++;
            continue;
        }

        /* "/.." */
        if (strncmp(p, "..", pend-p)==0) {
            p += 2;
            if (q == path) {
                continue;
            }
            /* change q to prev '/' */
            q--;
            while (*q != '/')
                q--;
            continue;
        }

        q++;
        *q = *p++;

    }
    q++;
    strcpy(q, p);
    return ;
}

int ul_single_path_ex2(char *path) {
    int len = ul_delete_inter(path); //delete '\r' '\n';
    return ul_single_path_ex2_nointer(path, len);
}

int ul_check_url_ex2(char *url) {
    char *pin;
    char *pout;
    char  port[6];
    int   k = 0;
    int dotcount=0;
    char *url_start;
    int len;
    int space_flag = 0;
    len = ul_delete_inter(url); //delete \r\n;
    if(strncasecmp(url,"http://",7) == 0) {
        pin = url + 7;
    } else {
        pin = url;
    }
    pout = pin;
    url_start = pin;
    //check host name
    while(UL_URL_LET_DIG_HY[(unsigned char)*pin] ||*pin=='%') {
        if(UL_URL_UP_LET[(unsigned char)*pin])
            *pin += 32;
        else {
            if(*pin=='%') {
                if(pin[1]!='\0' && pin[2]!='\0') {
                    unsigned char escape_char = ul_to_char(pin+1) ;
                    if(UL_URL_LET_DIG_HY[escape_char]) {
                        if (UL_URL_UP_LET[escape_char]) {
                            pin[2] = escape_char+32;
                        } else {
                            pin[2] = escape_char;
                        }
                        pin+=2 ;
                    } else {
                        return 0 ;
                    }
                } else {
                    return 0 ;
                }
            }
        }
        *pout++=*pin++;
    }
    if(pin == url_start)
        return 0;

    pin --;
    if(!UL_URL_LET_DIGIT[(unsigned char)*pin++])
        return 0;
    while(*pin == '.') {
        dotcount++;
        *pout++=*pin++;
        while(UL_URL_LET_DIG_HY[(unsigned char)*pin] ||(*pin=='%')) {
            if(UL_URL_UP_LET[(unsigned char)*pin])
                *pin += 32;
            else {
                if(*pin=='%') {
                    if(pin[1]!='\0' && pin[2]!='\0') {
                        unsigned char escape_char = ul_to_char(pin+1) ;
                        if(UL_URL_LET_DIG_HY[escape_char]==1) {
                            if (UL_URL_UP_LET[escape_char]) {
                                pin[2] = escape_char + 32;
                            } else {
                                pin[2] = escape_char;
                            }
                            pin+=2 ;
                        } else {
                            return 0 ;
                        }
                    } else {
                        return 0 ;
                    }
                }
            }
            *pout++=*pin++;
        }
        pin --;
        if(!UL_URL_LET_DIGIT[(unsigned char)*pin++])
            return 0;
    }
    if(dotcount == 0) {
        return 0;
    }
    //check host name over

    //check port
    if(*pin == ':') {
        pin ++;
        if(*pin != '/') {
            while(UL_URL_DIGIT[(unsigned char)*pin]) {
                port[k++] = *pin++;
                if(k>5)
                    return 0;
            }
            port[k] = 0;
            if(*pin != '/')
                return 0;
            memcpy(pout, pin-k-1, k+1);
            if(strcmp(port,"80") != 0) {
                pout += (k+1);
            }
        }
    } else if( (*pin != '/') && (*pin !='\0' ))
        return 0;
    //check port over

    //check abs_path
    if(*pin == '/') {
        *pout++=*pin++;
        //path
        while(UL_URL_PCHAR[(unsigned char)*pin]) {
            space_flag|=((*pout++=*pin++)==' ');
        }

        while(*pin == '/') {
            *pout++=*pin++;
            while(UL_URL_PCHAR[(unsigned char)*pin]) {
                space_flag|=((*pout++=*pin++)==' ');
            }
        }

        // ";" params  == *(";" param)
        while(*pin == ';') {
            *pout++=*pin++;
            while(UL_URL_PARAM[(unsigned char)*pin]) {
                space_flag|=((*pout++=*pin++)==' ');
            }
        }

        //"?" query
        while(*pin == '?') {
            *pout++=*pin++;
            while(UL_URL_UCH_RES[(unsigned char)*pin]) {
                space_flag|=((*pout++=*pin++)==' ');
            }
        }
        if(*pin != 0)
            return 0;
        *pout = 0;
        if (pout-url > len)
            return 0;
        if(space_flag==1)
            return ul_space_escape_ex2(url ,UL_MAX_URL_LEN_EX2, UL_MAX_SITE_LEN_EX2, UL_MAX_PATH_LEN_EX2);
        else
            return 1;
    } else  if (*pin != 0)
        return 0;
    *pout = '\0';
    if (space_flag==1)
        return ul_space_escape_ex2(url ,UL_MAX_URL_LEN_EX2, UL_MAX_SITE_LEN_EX2, UL_MAX_PATH_LEN_EX2);
    else
        return 1;
    //check abs_path over
}


#ifdef TEST_FUNC
int main() {
    char url[UL_MAX_URL_LEN];
    char buf[UL_MAX_URL_LEN];
    char *p;

    while(fgets(url, UL_MAX_URL_LEN, stdin) != NULL) {
        p = strchr(url, '\n');
        if (p == NULL) {
            fprintf(stderr, "[Too long url] %s\n", url);
            continue;
        } else {
            *p = '\0';
            if (url[strlen(url)-1] == '\r')
                url[strlen(url)-1] = '\0';
        }
        if (ul_normalize_url(url, buf) != 1) {
            fprintf(stderr, "[Normalize url failed] %s\n", url);
            continue;
        }
        if (ul_isnormalized_url(buf) != 1)
            fprintf(stderr, "[Is Normalized url failed???] %s\n", buf);
        else
            fprintf(stdout, "[Normalized url] %s\n", buf);
    }
    return 0;
}
#endif
/* vim: set ts=4 sw=4 sts=4 tw=100 noet: */
