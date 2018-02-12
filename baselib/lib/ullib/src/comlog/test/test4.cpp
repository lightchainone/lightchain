
#include <stdio.h>
#include <assert.h>
#include "com_log_if.h"

#define TEST_CONF_FILEDIR  "."
#define TEST_CONF_FILENAME "test_change_log_level.conf"

// 写配置文件中的配置信息
static void write_conf_file(int new_log_level)
{
    FILE* conf = fopen(TEST_CONF_FILEDIR "/" TEST_CONF_FILENAME, "wb");
    assert((FILE *)NULL != conf);
    
    char buffer[1024];
    snprintf(buffer, sizeof buffer, "COMLOG_PROCNAME : test_change_log_level\n");
    fwrite(buffer, 1, strlen(buffer), conf);
    snprintf(buffer, sizeof buffer, "COMLOG_LEVEL : %d\n", new_log_level);
    fwrite(buffer, 1, strlen(buffer), conf);
    snprintf(buffer, sizeof buffer, "COMLOG_DEVICE_NUM : 1\n");
    fwrite(buffer, 1, strlen(buffer), conf);
    snprintf(buffer, sizeof buffer, "COMLOG_DEVICE0 : TTY\n");
    fwrite(buffer, 1, strlen(buffer), conf);
    snprintf(buffer, sizeof buffer, "TTY_TYPE : TTY\n");
    fwrite(buffer, 1, strlen(buffer), conf);
    snprintf(buffer, sizeof buffer, "TTY_OPEN : 1\n");
    fwrite(buffer, 1, strlen(buffer), conf);
    
    fclose(conf);
}

// 删除测试文件
static void delete_conf_file()
{
    unlink(TEST_CONF_FILEDIR "/" TEST_CONF_FILENAME);
}

// 显示各等级的日志
static void write_some_log()
{
    com_writelog(COMLOG_FATAL,   "COMLOG_FATAL = %d",   COMLOG_FATAL);
	com_writelog(COMLOG_WARNING, "COMLOG_WARNING = %d", COMLOG_WARNING);
	com_writelog(COMLOG_NOTICE,  "COMLOG_NOTICE = %d",  COMLOG_NOTICE);
	com_writelog(COMLOG_TRACE,   "COMLOG_TRACE = %d",   COMLOG_TRACE);
	com_writelog(COMLOG_DEBUG,   "COMLOG_DEBUG = %d",   COMLOG_DEBUG);
}

// 动态变更日志等级测试
int main(int argc, char **argv)
{
	int ret;
    
    // 加载配置 <- 这里原本没有Bug
    write_conf_file(16);
    ret = com_loadlog(TEST_CONF_FILEDIR, TEST_CONF_FILENAME);
	assert(0 == ret);
	fprintf(stderr, "= 测试1：等级16。接下来你应该看到5条日志\n");
	write_some_log();

    // 人肉变更日志等级 <- 这里曾有一个Bug
    com_logstat_t logstat;
	logstat.sysevents = 16;
    ret = com_changelogstat(&logstat);
    assert(0 == ret);
	fprintf(stderr, "= 测试2：等级16。接下来你应该看到5条日志\n");
	write_some_log();
    
    // 允许日志监控 <- 这里曾有一个Bug
    ret = com_reglogstat(TEST_CONF_FILEDIR, TEST_CONF_FILENAME);
    assert(0 == ret);
    com_changelogstat();
    fprintf(stderr, "= 测试3：等级16。接下来你应该看到5条日志\n");
	write_some_log();
	
	// 变更日志等级
	write_conf_file(4);
	com_changelogstat();
    fprintf(stderr, "= 测试4：等级4。接下来你应该看到3条日志\n");
	write_some_log();
	
	// 测试结束
	delete_conf_file();
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
