

#ifndef _LC_AUTH_H
#define _LC_AUTH_H

typedef struct _lc_auth_t lc_auth_t;


lc_auth_t * lc_auth_create(unsigned int sup_size=64);


void lc_auth_destroy(lc_auth_t *);


int lc_auth_load_ip(lc_auth_t *auth, const char *file);

int lc_auth_load_ip_ex(lc_auth_t *auth, const char *file, void (*cb)(const char*, void *), void *data);


int lc_auth_push_ip(lc_auth_t *auth, const char *ip);


int lc_auth_compile(lc_auth_t *auth);


int lc_auth_ip_str(lc_auth_t *auth, const char *ip);


int lc_auth_ip_int(lc_auth_t *auth, const unsigned int ip);


int lc_auth_ip_fd(lc_auth_t *auth, const int fd);


int  lc_auth_addref(lc_auth_t *auth);
void lc_auth_delref(lc_auth_t *auth);
int  lc_auth_refcnt(lc_auth_t *auth);

#endif

