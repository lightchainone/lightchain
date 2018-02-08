

#ifndef ZOOKEEPER_H_
#define ZOOKEEPER_H_

#include <sys/time.h>
#include <stdio.h>

#include "zookeeper_version.h"
#include "recordio.h"
#include "zookeeper.jute.h"






#ifdef DLL_EXPORT
#    define ZOOAPI __declspec(dllexport)
#else
#  if defined(__CYGWIN__) && !defined(USE_STATIC_LIB)
#    define ZOOAPI __declspec(dllimport)
#  else
#    define ZOOAPI
#  endif
#endif



enum ZOO_ERRORS {
  ZOK = 0, 

  
  ZSYSTEMERROR = -1,
  ZRUNTIMEINCONSISTENCY = -2, 
  ZDATAINCONSISTENCY = -3, 
  ZCONNECTIONLOSS = -4, 
  ZMARSHALLINGERROR = -5, 
  ZUNIMPLEMENTED = -6, 
  ZOPERATIONTIMEOUT = -7, 
  ZBADARGUMENTS = -8, 
  ZINVALIDSTATE = -9, 

  
  ZAPIERROR = -100,
  ZNONODE = -101, 
  ZNOAUTH = -102, 
  ZBADVERSION = -103, 
  ZNOCHILDRENFOREPHEMERALS = -108, 
  ZNODEEXISTS = -110, 
  ZNOTEMPTY = -111, 
  ZSESSIONEXPIRED = -112, 
  ZINVALIDCALLBACK = -113, 
  ZINVALIDACL = -114, 
  ZAUTHFAILED = -115, 
  ZCLOSING = -116, 
  ZNOTHING = -117 
};

#ifdef __cplusplus
extern "C" {
#endif


typedef enum {ZOO_LOG_LEVEL_ERROR=1,ZOO_LOG_LEVEL_WARN=2,ZOO_LOG_LEVEL_INFO=3,ZOO_LOG_LEVEL_DEBUG=4} ZooLogLevel;


extern ZOOAPI const int ZOO_PERM_READ;
extern ZOOAPI const int ZOO_PERM_WRITE;
extern ZOOAPI const int ZOO_PERM_CREATE;
extern ZOOAPI const int ZOO_PERM_DELETE;
extern ZOOAPI const int ZOO_PERM_ADMIN;
extern ZOOAPI const int ZOO_PERM_ALL;

extern ZOOAPI struct Id ZOO_ANYONE_ID_UNSAFE;
extern ZOOAPI struct Id ZOO_AUTH_IDS;

extern ZOOAPI struct ACL_vector ZOO_OPEN_ACL_UNSAFE;
extern ZOOAPI struct ACL_vector ZOO_READ_ACL_UNSAFE;
extern ZOOAPI struct ACL_vector ZOO_CREATOR_ALL_ACL;



extern ZOOAPI const int ZOOKEEPER_WRITE;
extern ZOOAPI const int ZOOKEEPER_READ;




extern ZOOAPI const int ZOO_EPHEMERAL;
extern ZOOAPI const int ZOO_SEQUENCE;




extern ZOOAPI const int ZOO_EXPIRED_SESSION_STATE;
extern ZOOAPI const int ZOO_AUTH_FAILED_STATE;
extern ZOOAPI const int ZOO_CONNECTING_STATE;
extern ZOOAPI const int ZOO_ASSOCIATING_STATE;
extern ZOOAPI const int ZOO_CONNECTED_STATE;





extern ZOOAPI const int ZOO_CREATED_EVENT;

extern ZOOAPI const int ZOO_DELETED_EVENT;

extern ZOOAPI const int ZOO_CHANGED_EVENT;

extern ZOOAPI const int ZOO_CHILD_EVENT;

extern ZOOAPI const int ZOO_SESSION_EVENT;


extern ZOOAPI const int ZOO_NOTWATCHING_EVENT;



typedef struct _zhandle zhandle_t;


typedef struct {
    int64_t client_id;
    char passwd[16];
} clientid_t;


typedef void (*watcher_fn)(zhandle_t *zh, int type, 
        int state, const char *path,void *watcherCtx);


ZOOAPI zhandle_t *zookeeper_init(const char *host, watcher_fn fn,
  int recv_timeout, const clientid_t *clientid, void *context, int flags);


ZOOAPI int zookeeper_close(zhandle_t *zh);


ZOOAPI const clientid_t *zoo_client_id(zhandle_t *zh);

ZOOAPI int zoo_recv_timeout(zhandle_t *zh);

ZOOAPI const void *zoo_get_context(zhandle_t *zh);

ZOOAPI void zoo_set_context(zhandle_t *zh, void *context);


ZOOAPI watcher_fn zoo_set_watcher(zhandle_t *zh,watcher_fn newFn);

#ifndef THREADED

ZOOAPI int zookeeper_interest(zhandle_t *zh, int *fd, int *interest, 
	struct timeval *tv);


ZOOAPI int zookeeper_process(zhandle_t *zh, int events);
#endif


typedef void (*void_completion_t)(int rc, const void *data);


typedef void (*stat_completion_t)(int rc, const struct Stat *stat,
        const void *data);


typedef void (*data_completion_t)(int rc, const char *value, int value_len,
        const struct Stat *stat, const void *data);


typedef void (*strings_completion_t)(int rc,
        const struct String_vector *strings, const void *data);


typedef void
        (*string_completion_t)(int rc, const char *value, const void *data);


typedef void (*acl_completion_t)(int rc, struct ACL_vector *acl,
        struct Stat *stat, const void *data);


ZOOAPI int zoo_state(zhandle_t *zh);


ZOOAPI int zoo_acreate(zhandle_t *zh, const char *path, const char *value, 
        int valuelen, const struct ACL_vector *acl, int flags,
        string_completion_t completion, const void *data);


ZOOAPI int zoo_adelete(zhandle_t *zh, const char *path, int version, 
        void_completion_t completion, const void *data);


ZOOAPI int zoo_aexists(zhandle_t *zh, const char *path, int watch, 
        stat_completion_t completion, const void *data);


ZOOAPI int zoo_awexists(zhandle_t *zh, const char *path, 
        watcher_fn watcher, void* watcherCtx, 
        stat_completion_t completion, const void *data);


ZOOAPI int zoo_aget(zhandle_t *zh, const char *path, int watch, 
        data_completion_t completion, const void *data);


ZOOAPI int zoo_awget(zhandle_t *zh, const char *path, 
        watcher_fn watcher, void* watcherCtx, 
        data_completion_t completion, const void *data);


ZOOAPI int zoo_aset(zhandle_t *zh, const char *path, const char *buffer, int buflen, 
        int version, stat_completion_t completion, const void *data);


ZOOAPI int zoo_aget_children(zhandle_t *zh, const char *path, int watch, 
        strings_completion_t completion, const void *data);


ZOOAPI int zoo_awget_children(zhandle_t *zh, const char *path,
        watcher_fn watcher, void* watcherCtx, 
        strings_completion_t completion, const void *data);



ZOOAPI int zoo_async(zhandle_t *zh, const char *path, 
        string_completion_t completion, const void *data);



ZOOAPI int zoo_aget_acl(zhandle_t *zh, const char *path, acl_completion_t completion, 
        const void *data);


ZOOAPI int zoo_aset_acl(zhandle_t *zh, const char *path, int version, 
        struct ACL_vector *acl, void_completion_t, const void *data);


ZOOAPI const char* zerror(int c);


ZOOAPI int zoo_add_auth(zhandle_t *zh,const char* scheme,const char* cert, 
	int certLen, void_completion_t completion, const void *data);


ZOOAPI int is_unrecoverable(zhandle_t *zh);


ZOOAPI void zoo_set_debug_level(ZooLogLevel logLevel);


ZOOAPI void zoo_set_log_stream(FILE* logStream);


ZOOAPI void zoo_deterministic_conn_order(int yesOrNo);


ZOOAPI int zoo_create(zhandle_t *zh, const char *path, const char *value,
        int valuelen, const struct ACL_vector *acl, int flags, char *realpath,	 
        int max_realpath_len);


ZOOAPI int zoo_delete(zhandle_t *zh, const char *path, int version);



ZOOAPI int zoo_exists(zhandle_t *zh, const char *path, int watch, struct Stat *stat);


ZOOAPI int zoo_wexists(zhandle_t *zh, const char *path,
        watcher_fn watcher, void* watcherCtx, struct Stat *stat);


ZOOAPI int zoo_get(zhandle_t *zh, const char *path, int watch, char *buffer,   
                   int* buffer_len, struct Stat *stat);


ZOOAPI int zoo_wget(zhandle_t *zh, const char *path, 
        watcher_fn watcher, void* watcherCtx, 
        char *buffer, int* buffer_len, struct Stat *stat);


ZOOAPI int zoo_set(zhandle_t *zh, const char *path, const char *buffer,
                   int buflen, int version);


ZOOAPI int zoo_set2(zhandle_t *zh, const char *path, const char *buffer,
                   int buflen, int version, struct Stat *stat);


ZOOAPI int zoo_get_children(zhandle_t *zh, const char *path, int watch,
                            struct String_vector *strings);


ZOOAPI int zoo_wget_children(zhandle_t *zh, const char *path, 
        watcher_fn watcher, void* watcherCtx,
        struct String_vector *strings);


ZOOAPI int zoo_get_acl(zhandle_t *zh, const char *path, struct ACL_vector *acl,
                       struct Stat *stat);


ZOOAPI int zoo_set_acl(zhandle_t *zh, const char *path, int version,
                           const struct ACL_vector *acl);

#ifdef __cplusplus
}
#endif

#endif 
