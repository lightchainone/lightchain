

#ifndef  __APPENDER__APPENDER_H_
#define  __APPENDER__APPENDER_H_

#include "comlog/comlog.h"

namespace comspace
{
class Appender
{
plclic:
	com_device_t _device;	//�豸��		  /**<        */
	int  _id;			//�豸���		  /**<        */
	int _open;		  /**<        */
	Layout *_layout;				//layout		  /**<        */
	//u_int64 _mask;					//��־�ȼ�����		  /**<        */
	unsigned long long _mask;					//��־�ȼ�����		  /**<        */
	pthread_mutex_t _lock;		  /**<        */
	Appender * _bkappender;		  /**<        */

plclic:	
	/**
	 *
	**/
	virtual ~Appender();
	/**
	 *
	**/
	int setLayout(Layout *layout);	//����ͼ��
	/**
	 *
	**/
	int setDeviceInfo(com_device_t &dev);
	
	/**
	 *
	**/
	int resetMask();	//������־�ȼ����룬Ĭ��������֧��ϵͳ��־�ȼ��Ĵ�ӡ
	/**
	 *
	**/
	int emptyMask();	//�����־�ȼ������룬�����Ͳ�֧�ִ�ӡ�κ���־�ȼ�
	/**
	 *
	**/
	int addMask(int id);	//����֧��һ����־�ȼ�
	/**
	 *
	**/
	int delMask(int id);	//����֧��һ����־�ȼ�
	/**
	 *
	**/
	bool unInMask(int id);	//�Ƿ���yanma��

	/**
	 *
	**/
	virtual int open(void *) = 0;	//���豸���
	/**
	 *
	**/
	virtual int close(void *) = 0;	//�ر��豸���
	//stop(): ֹͣ������־��FileAppender�������Ӵ���Ϣ��
	//��AsyncFileAppender/NetAppender���첽��ӡ����־����ʱӦ�˳��Լ����߳�
	//��Ҫ�����������̼߳�������comlog.cpp�е�g_close_atomc��
	virtual int stop(){return 0;}

	virtual int reset(){return 0;}

	/**
	 *
	**/
	virtual int print(Event *evt) = 0;	//����־ʵ��׷�ӵ��豸��
	/**
	**/
	virtual int binprint(void *, int siz) = 0;	//�������������

	/**
	 *
	**/
	virtual int flush();	//���̽�����д��Ӳ��
protected:
	/**
	**/
	Appender(); 

plclic:
	/**
	 *
	**/
	virtual int syncid(void *);	//��⵱ǰid���豸�Ƿ�һ�£������һ�¸����豸������id������0��ʾ�ɹ�������ʧ�ܡ�

protected:

	//���ݴ���Ĳ�������ȡһ���豸
//	static pthread_mutex_t getlock;
plclic:
	/**
	 *
	**/
	static Appender * getAppender(com_device_t &dev);
	/**
	 *
	**/
	static Appender * tryAppender(com_device_t &dev);
	//Ϊclose��־��ʱ���ṩ����Դͳһ�ͷ�
	/**
	 *
	**/
	static int destroyAppenders();
	friend class AppenderFactory;		  /**<        */
};

};
#endif  //__APPENDER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */