#ifndef WNETAPI_H_H
#define WNETAPI_H_H
#include "m_w_base_api.h"
#include "Tools.h"
#include <lm.h>
#pragma comment(lib, "Netapi32.lib")

/*
�����5���ܾ����� �� �ܿ�����ʹ�õ��û����ǹ���ԱȨ�޵ģ�������Ȩ�ޣ�
�����51��Windows �޷��ҵ�����·�� : ���������⣻
�����53���Ҳ�������·�� �� ip��ַ����Ŀ��δ������Ŀ��lanmanserver����δ������Ŀ���з���ǽ���˿ڹ��ˣ���
�����67���Ҳ��������� �� ���lanmanworkstation����δ������Ŀ��ɾ����ipc$��
�����1219���ṩ��ƾ�����Ѵ��ڵ�ƾ�ݼ���ͻ �� ���Ѿ��ͶԷ�������һ��ipc$����ɾ��������
�����1326��δ֪���û������������ �� ԭ��������ˣ�
�����1792����ͼ��¼�����������¼����û������ �� Ŀ��NetLogon����δ��������������ػ���ִ������
�����2242�����û��������Ѿ����� �� Ŀ�����ʺŲ��ԣ�ǿ�ƶ���Ҫ��������롣
*/

enum IPC_ERROR{
	IPC_SUCCESS = 0x00,
	IPC_PRIVILEGE_ERROR = 0x05,
	IPC_NETWORK_ERROR = 0x43,
	IPC_USER_PASS_ERROR = 0x52E,
	IPC_PASS_EXPIRE = 0X8C2
};
	
class WNET_API : public m_w_base_api
{
private:
	string username;
	string password;
public:
	WNET_API();
	DWORD openConnectBySelf(string& serverName);
	DWORD openConnectByUserPass(string serverName, string username, string password);
	void getLoggedUsers(wstring serverName);
	static vector<wstring> getDomainGroupMembers(wstring serverName, wstring groupName);
	static vector<wstring> getLocalGroupMembers(wstring serverName, wstring groupName);
	DWORD closeConnection(string lpRemoteName);
	~WNET_API();
};
#endif
