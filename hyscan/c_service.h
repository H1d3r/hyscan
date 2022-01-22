#ifndef SERVICE_H_H
#define SERVICE_H_H
#include "public.h"

class PortService{
public:
	PortService();
	PortService(string serviceNameString, string serviceIpAddr, DWORD dwServicePort, DWORD dwPortStatus, DWORD dwServiceStatus);
	~PortService();
public:
	string serviceNameString;
	string serviceIpAddr;
	DWORD dwServicePort;
	DWORD dwPortStatus;
	DWORD dwServiceStatus;
};

class ServiceVuln{

public:
	ServiceVuln();
	ServiceVuln(string serviceName, BOOL vulnerable, BOOL bIsBug, DWORD vulnPort, string bugInformation);
	~ServiceVuln();
public:
	BOOL vulnerable; //�Ƿ���Խ���©������
	BOOL bIsBug; // ©���Ƿ����
	DWORD vulnPort; // ©���˿�
	string serviceName; // ��������
	string bugInformation; // ���������Ϣ
};

#endif

