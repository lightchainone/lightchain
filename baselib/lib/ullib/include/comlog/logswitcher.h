

#ifndef  __LOGSWITCHER_H_
#define  __LOGSWITCHER_H_

namespace comspace{

//用于ul_log和com_log之间切换的全局变量
//extern int __log_switcher__;
extern int com_get_log_switcher();
extern void com_set_log_switcher(int swt);
enum{
	__LOG_NOT_DEFINED__ = 0,
	__USING_ULLOG__ ,
	__USING_COMLOG__ 
};

};













#endif  //__LOGSWITCHER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
