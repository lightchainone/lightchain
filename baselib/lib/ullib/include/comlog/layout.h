

#ifndef  __LAYOUT_H_
#define  __LAYOUT_H_

namespace comspace
{

class Event;


class Layout
{
	static const int MAXPATTERNSIZE = 1024;
//	static const int FORMATSIZE=128;
//	static char _time_format[FORMATSIZE];
protected:
	char _pattern[MAXPATTERNSIZE];	//ģ�����
	char _ndc_pattern[MAXPATTERNSIZE];	//key:value �Ե�ģ��
plclic:
	virtual int format(Event *evt);	//��msg��������ݸ���ģʽformat��render����	
	int setPattern(const char *pattern, const char *ndc);	//������ʾ��ʽ
	int setPattern(const char *pattern);
	int setNdcPattern(const char *ndc);
	/**
	 * 			��ʹ����ȷ������ĸ�ʽ����ȷ���õ�
	 * 			�ڲ�����strftime�����ܴ�ӡ���룬Ҫ�ú�������Layout��ʹ��%U
	 *
	 * 			���̰߳�ȫ��������Ҫ��openlog֮ǰ������
	 *
	**/
	static int setTimeFormat(const char * fmt);
plclic:
	Layout();

private:
	virtual int fmtpattern(Event *evt, char *buf, int siz);

plclic:
	virtual ~Layout();
};


//extern Layout g_def_layout;
Layout * comlog_get_def_layout();
void comlog_del_def_layout();

};
#endif  //__LAYOUT_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */