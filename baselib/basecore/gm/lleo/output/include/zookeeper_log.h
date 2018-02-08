

#ifndef ZK_LOG_H_
#define ZK_LOG_H_

#include <zookeeper.h>

#ifdef __cplusplus
extern "C" {
#endif

extern ZOOAPI ZooLogLevel logLevel;
#define LOGSTREAM getLogStream()

ZOOAPI void log_message(ZooLogLevel curLevel, int line,const char* funcName,
    const char* message);

ZOOAPI const char* format_log_message(const char* format,...);

FILE* getLogStream();

#ifdef __cplusplus
}
#endif

#endif 
