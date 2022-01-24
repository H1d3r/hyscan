#ifndef WMIAPI_H_H
#define WMIAPI_H_H
#include "m_w_base_api.h"
#include <WbemIdl.h>
#pragma comment(lib,"WbemUuid.lib")
class WMIAPI : public m_w_base_api
{
private:
	IEnumWbemClassObject* pEnumClsObj; // ���� IWbemClassObject ���͵Ķ����ö������ �������ڱ�׼ COM ö�������� IEnumVariant��
	IWbemClassObject* pWbemClsObj; // 	�����������ඨ��������ʵ���� ������Ա����Ҫʵ�ִ˽ӿ�;WMI �ṩ��ʵ�֡�
	IWbemServices* pWbemSvc; // �ͻ��˺��ṩ�������ڷ��� WMI ���� �ӿڽ��� WMI ʵ�֣����� WMI �ӿ�
	IWbemLocator* pWbemLoc; // ��ȡָ���ض���������� WMI �� IWbemServices �ӿڵĳ�ʼ�����ռ�ָ��
public:
	WMIAPI();
	HRESULT init();
	HRESULT connect(string& ipAddr, string& username, string& password);
	HRESULT connect(string& ipAddr);
	void exec(string& command);
	void release();
	~WMIAPI();
};

#endif