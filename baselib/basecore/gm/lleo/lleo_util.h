
#ifndef __GALILEO_UTIL_H__
#define __GALILEO_UTIL_H__

#include <openssl/sha.h>

#ifdef __cplusplus
extern "C" {
#endif



void lleo_safe_exit();


unsigned int string_hash_djb2(void *str);



int string_equal(void *key1,void *key2);



int get_localhost_ip(char *ip, int len);



int get_local_conn_ip(char *ipstr, int len);

char *base64Encode(char *b, unsigned int len, char *sb, unsigned int sb_len);
char *sha1Encode(const char *password, unsigned int len, char *buf, unsigned int buf_len);

#ifdef __cplusplus
}
#endif

#endif

