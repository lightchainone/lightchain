
#include "layout.h"
#include "event.h"
#include "xutils.h"
#include "logcore.h"

namespace comspace
{
static const int COMLOG_LAYOUT_FORMATSIZE=128;
//char Layout::_time_format[Layout::FORMATSIZE] = "";
static char comlog_time_format[COMLOG_LAYOUT_FORMATSIZE] = "";

Layout::Layout()
{
	//setPattern("%L: %D:  %P * %T [%N] %R", "%K:%V,");
	setPattern("%L: %D:  %P * %T %R", "%K:%V,");
}

int Layout::setPattern(const char *pattern, const char *ndc)
{
	snprintf(_pattern, sizeof(_pattern), "%s", pattern);
	snprintf(_ndc_pattern, sizeof(_ndc_pattern), "%s", ndc);
	return 0;
}

int Layout::setPattern(const char *pattern)
{
	snprintf(_pattern, sizeof(_pattern), "%s", pattern);
	return 0;
}

int Layout::setNdcPattern(const char *ndc)
{
	snprintf(_ndc_pattern, sizeof(_ndc_pattern), "%s", ndc);
	return 0;
}

int Layout::setTimeFormat(const char * fmt){
	snprintf(comlog_time_format, sizeof(comlog_time_format), "%s", fmt);
	return 0;
}

/*
 *  %L 	日志等级
 *  %D	非毫秒时间，默认的时间格式
 *  %U	毫秒
 *  %A	全部时间，包括毫秒
 *  %T	线程号
 *  %P	进程名
 *  %N	Push对
 *  %R	日志信息
 *  %K	Push一个key value对的时候key的位置
 *  %V	Push一个key value对的时候value的位置
 *  %Y  自定义时间格式
 */

#define CHECKTAIL \
	if (ret + ptr > bufsiz) { \
		ptr = bufsiz; \
		goto end; \
	} 

int Layout::format(Event *evt)
{
	int ptr = 0;
	int ret = 0;
	int bufsiz = evt->_render_msgbuf_size;
	char *buf = evt->_render_msgbuf;
	if(bufsiz < 2){
		return -1;
	}

	if (evt->_log_version >= 0 && *evt->_log_type != 0) {
		ret  = snprintf(buf+ptr, bufsiz-ptr, "%d", evt->_log_version);
		CHECKTAIL;
		ptr += ret;
		buf[ptr++] = ' ';

		ret  = snprintf(buf+ptr, bufsiz-ptr, "%s", evt->_log_type);
		CHECKTAIL;
		ptr += ret;
		buf[ptr++] = ' ';
	}

	//_com_debug("patterns[%s] ndc[%s]", _pattern, _ndc_pattern);

	for (int i=0; _pattern[i]!=0; ++i) {
		if (ptr >= bufsiz) {
			break;
		}
		if (_pattern[i] == '%') {
			++i;
			if (_pattern[i] == 0) {
				break;
			}
			switch (_pattern[i]) {
				case 'L':
					ret  = snprintf(buf+ptr, bufsiz-ptr, "%s", getLogName(evt->_log_level));
					CHECKTAIL;
					ptr += ret;
					break;
				case 'A':
					ret = xfmtime(buf+ptr, bufsiz-ptr, evt->_print_time);
					CHECKTAIL;
					ptr += ret;
					break;
				case 'T':
					ret  = snprintf(buf+ptr, bufsiz-ptr, "%u", (u_int)evt->_thread_id);
					CHECKTAIL;
					ptr += ret;
					break;
				case 'D':
					ret = xfmtime(buf+ptr, bufsiz-ptr, evt->_print_time.tv_sec);
					CHECKTAIL;
					ptr += ret;
					break;
				case 'Y':
					struct tm ftm;
					localtime_r(&evt->_print_time.tv_sec, &ftm);
					ret = strftime(buf+ptr, bufsiz-ptr, comlog_time_format, &ftm);
					CHECKTAIL;
					ptr += ret;
					break;
				case 'U':
					ret = snprintf(buf+ptr, bufsiz-ptr, "%lu", evt->_print_time.tv_usec/1000);
					CHECKTAIL;
					ptr += ret;
					break;
				case 'P':
					ret = snprintf(buf+ptr, bufsiz-ptr, "%s", evt->_proc_name);
					CHECKTAIL;
					ptr += ret;
					break;
				case 'N':
					//_com_debug("check ndc %d", evt->_ndc_num);
					ret = fmtpattern(evt, buf+ptr, bufsiz-ptr);
					CHECKTAIL;
					ptr += ret;
					break;
				/* Add hostname by wangshaoyan, 2012-08-20*/
				case 'H':
					char hostname[128];
					gethostname(hostname, 128);
					ret = xstrcpy(buf+ptr, bufsiz-ptr, hostname, strlen(hostname));
					CHECKTAIL;
					ptr += ret;
					break;
				case 'R':
					/*
					ret = snprintf(buf+ptr, bufsiz-ptr, "%s", evt->_msgbuf);
					CHECKTAIL;
					ptr += ret;
					*/
					ret = xstrcpy(buf+ptr, bufsiz-ptr, evt->_msgbuf, evt->_msgbuf_len);
					CHECKTAIL;
					ptr += ret;
					break;
				case '%':
					//buf[ptr++] = '%';
					break;
				default:
					break;
			}
		} else {
			buf[ptr++] = _pattern[i];
		}
	}
end:
	if (ptr >= bufsiz-1) {
		//bufsiz must be >= 2 !!!
		ptr = bufsiz - 2;
	}
	buf[ptr++] = '\n';
	buf[ptr] = 0;
	evt->_render_msgbuf_len = ptr;
	//_com_debug("render msgbuf[%s] len=%d", buf, ptr);
	return 0;
}

int Layout::fmtpattern(Event *evt, char *buf, int bufsiz)
{
	int ptr = 0;
	int ret = 0;
	//_com_debug("bufsiz=%d", bufsiz);
	for (int i=0; i<evt->_ndc_num; ++i) {
		//_com_debug("format ndc[%d][%s]", i, _ndc_pattern);
		for (int j=0; _ndc_pattern[j]!=0; ++j) {
			if (ptr >= bufsiz) {
				//_com_debug("ptr[%d] >= bufsiz[%d]", ptr, bufsiz);
				return 0;
			}
			if (_ndc_pattern[j] != '%') {
				buf[ptr++] = _ndc_pattern[j];
			} else {
				if (_ndc_pattern[++j] == 0) {
					//_com_debug("find zero in _ndc_pattern[%s]", _ndc_pattern);
					break;
				}
				switch (_ndc_pattern[j]) {
					case 'K':
						ret = snprintf(buf+ptr, bufsiz-ptr, "%s", evt->_ndcs[i].key);
						//_com_debug("push key:%s", buf+ptr);
						CHECKTAIL;
						ptr += ret;
						break;
					case 'V':
						ret = snprintf(buf+ptr, bufsiz-ptr, "%s", evt->_ndcs[i].value);
						//_com_debug("push val:%s", buf+ptr);
						CHECKTAIL;
						ptr += ret;
						break;
					case '%':
						buf[ptr] = '%';
						ret = 1;
						CHECKTAIL;
						ptr += ret;
						break;
					default:
						break;
				}
			}
		}
	}
end:
	if (ptr >= bufsiz) {
		--ptr;
	}
	buf[ptr] = 0;
	return ptr;
}

Layout::~Layout()
{
}

static Layout *g_def_layout=NULL;/**<  glabal layout */
Layout * comlog_get_def_layout(){
	if (NULL== g_def_layout) {
		g_def_layout = new Layout();
	}
	return g_def_layout;
}

void comlog_del_def_layout(){
	if (g_def_layout) {
		delete g_def_layout;
		g_def_layout = NULL;
	}
}

}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
