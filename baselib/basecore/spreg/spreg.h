

#ifndef  __SPREG_H_
#define  __SPREG_H_

#include "pcre.h"


enum {
    SPREG_NOMATCH          = PCRE_ERROR_NOMATCH,		  
    SPREG_NULL             = PCRE_ERROR_NULL,			  
    SPREG_BADOPTION        = PCRE_ERROR_BADOPTION,		  
    SPREG_NOMEMORY         = PCRE_ERROR_NOMEMORY,		  
    SPREG_MATCHLIMIT       = PCRE_ERROR_MATCHLIMIT,		  
    SPREG_BADUTF8         = PCRE_ERROR_BADUTF8,			  
    SPREG_INTERNAL        = PCRE_ERROR_INTERNAL,		  
    SPREG_BADNEWLINE      = PCRE_ERROR_BADNEWLINE,		  
	SPREG_BADBUFFER       = -99		                      
		                                                      
															  
};


const int SPREG_DEFAULT       = 0;		                  
const int SPREG_CASELESS      = PCRE_CASELESS;		      
const int SPREG_MULTILINE	  = PCRE_MULTILINE;		      
const int SPREG_DOTALL        = PCRE_DOTALL;		      
const int SPREG_NOTBOL		  = PCRE_NOTBOL;		      
const int SPREG_NOTEOL		  = PCRE_NOTEOL;		      
const int SPREG_UNGREEDY	  = PCRE_UNGREEDY;		      
const int SPREG_NOTEMPTY      = PCRE_NOTEMPTY;            
const int SPREG_UTF8		  = PCRE_UTF8;			      
const int SPREG_NO_UTF8_CHECK = PCRE_NO_UTF8_CHECK;	      




typedef struct _spreg_match_t {
	int match_begin;		  
	int match_end;		      
} spreg_match_t;


typedef struct _spreg_t spreg_t;




typedef struct _spreg_callback_param_t {
	spreg_match_t *rmatch;		  
	const char *src;				  
	void *arg;						  
	int nrmatch;		              
	int src_size;		              
} spreg_callback_param_t;


typedef int spreg_callback_t(spreg_callback_param_t *callback_match);


spreg_match_t *spreg_match_init (int n);


spreg_t * spreg_init(const char *reg,  
		const char **errinfo = NULL,
		int option = SPREG_DEFAULT);


int spreg_search(const spreg_t *re,
		const char *src,  
		int src_size,
		spreg_match_t *rmatch, 
		int n, 
		int option = SPREG_DEFAULT);


int spreg_match(const spreg_t *re, 
		const char *src, 
		int src_size,
		spreg_match_t *rmatch = NULL, 
		int n = 0,
		int option = SPREG_DEFAULT);



int spreg_search_all(const spreg_t *re,
		const char *src,  
		int src_size,
		spreg_callback_t *callback,
		void *arg = NULL,
		int option = SPREG_DEFAULT);



int spreg_split(const spreg_t *re,
		const char *src,  
		int src_size,
		spreg_callback_t *callback,
		void *arg = NULL,
		int option = SPREG_DEFAULT);



int spreg_replace(const spreg_t *re,
		const char *src,
		int src_size,
		const char *replace_string,
		char *dst,
		int dst_size,
		int limit = 0,
		int option = SPREG_DEFAULT);



int spreg_destroy(spreg_t *re);



int spreg_match_destroy(spreg_match_t *rmatch);



const char *spreg_error(int err);

#endif  


