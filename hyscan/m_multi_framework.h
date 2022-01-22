#ifndef THREADFRAMEWORK_H_H
#define THREADFRAMEWORK_H_H
#include "public.h"
#include "m_thread_pool.h"
#include "NtlmInfo.h"
#include "ScannerFactory.h"

class m_multi_framework
{
public:
	ThreadPool* threadPool;//			�̳߳�
public:
	////////////////////////////////////
	m_multi_framework(DWORD dwThreadNum);
	~m_multi_framework();
	///////////////////////////////////
	void createThreadPool(DWORD dwMaxThreadNum); //�̳߳س�ʼ��
	void socketInit(); // socket ws���ʼ��
	static void portBannerInit(); // port banner��ʼ��
	////////////////////////////////////
	void addTask(callbackFunc func, map<string, string>* mArgs); // ������� 1param
	void addTask(callbackFunc2param func, map<string, string>* vArgs); // ������� 2param
	void addTask(weakCallbackFunc func, map<string, string>* mArgs); // ������� 5param
	////////////////////////////////////
	void startWork(); // ���������̶߳���
	
	////////////////////////////////////
	void getNtlmInfoResult(); // ntlms type2 �Ľ����ȡ
};
#endif
