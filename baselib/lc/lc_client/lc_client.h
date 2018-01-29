#ifndef __LC_CLIENT_H__
#define __LC_CLIENT_H__


#include "ul_def.h"
#include "lc_conf.h"
#include "lc_log.h"
#include "nshead.h"

#ifdef __cplusplus
extern "C"{
#endif 

enum {
  LC_CLIENT_LINGER_OPEN = 1		  
};


typedef struct _lc_client_t			lc_client_t;


typedef struct _lc_client_attr_t	lc_client_attr_t;



typedef struct _lc_client_mod_attr_t{
	u_int					nomagiccheck;		
}lc_client_mod_attr_t;

typedef struct _lc_client_mod_attr_t lc_client_mod_attr_t;



typedef int (* lc_callback_check)(void *res);



typedef struct _lc_parallel_t{
	char				name[WORD_SIZE/16];	
	
	u_int				hash_key;			
	nshead_t			*reqhead;			
	
	void				*reqbuff;			
	nshead_t			*reshead;			
	
	void				*resbuff;			
	u_int				maxreslen;			
	bool				successed;			
	lc_callback_check	checkcallback;		

	u_int				reserv_int[8];		
	u_int				lc_client_reserved;	
}lc_parallel_t;



extern lc_client_t*
lc_client_init(lc_client_attr_t *attr = NULL);



extern int
lc_client_close(lc_client_t *client);



extern int
lc_client_add(lc_client_t *client, const lc_request_svr_t *svr,
		const char *name, const lc_client_mod_attr_t *attr=NULL);



extern int
	lc_client_orginvite(lc_client_t *client, const char *name, u_int hash_key,
		        nshead_t *reqhead, void *reqbuff, nshead_t *reshead, void *resbuff,
		        u_int maxreslen, lc_callback_check checkcallback = NULL);


extern int
lc_client_invite(lc_client_t *client, const char *name,  u_int hash_key,
		nshead_t *reqhead, nshead_t *reshead,
		u_int maxreslen, lc_callback_check checkcallback = NULL);



extern int
lc_client_basicgroupinvite(lc_client_t *client, lc_parallel_t *parallel, u_int count, u_int ms_timeout = 0);



extern int
lc_client_groupinvite(lc_client_t *client, lc_parallel_t *parallel, u_int ms_timeout = 0);


typedef struct _lc_callback_alloc_t {
	u_int size;		      
	void *arg;		  
	void *buff;		  
} lc_callback_alloc_t;


typedef void *(*lc_callback_alloc)(lc_callback_alloc_t *arg);


typedef void (*lc_callback_clear)(lc_callback_alloc_t *arg);


extern int
lc_client_basicgroupinvite_async(lc_client_t *client, lc_parallel_t* parallel,
		u_int count, u_int ms_timeout = 0, lc_callback_alloc alloccallback = NULL,
		lc_callback_clear clearcallback = NULL, void *alloc_arg = NULL);


extern int
lc_client_groupinvite_async(lc_client_t *client, lc_parallel_t* parallel,
		u_int ms_timeout = 0, lc_callback_alloc alloccallback = NULL,
		lc_callback_clear clearcallback = NULL, void *alloc_arg = NULL);


#ifdef __cplusplus
}
#endif 

#endif 


