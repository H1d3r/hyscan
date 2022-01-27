#ifndef WNETAPI_H_H
#define WNETAPI_H_H
#include "m_w_base_api.h"
#include "Tools.h"
#include <lm.h>
#pragma comment(lib, "Netapi32.lib")

/*
#include "samlib.h"
#pragma comment(lib, "samlib.lib")
#pragma comment(lib, "ntdll.lib")
*/

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
	IPC_SUCCESS = 0,
	IPC_PRIVILEGE_ERROR = 5,
	IPC_NETWORK_ERROR = 67,
	IPC_USER_PASS_ERROR = 1326,
	IPC_PASS_EXPIRE = 2242
};
	
class WNET_API : public m_w_base_api
{
public:
	WNET_API();
	~WNET_API();
	DWORD openConnectBySelf(string& serverName);
	DWORD openConnectByUserPass(string serverName, string username, string password);
	void getLoggedUsers(string& serverName);
	void getNetSession(wstring& serverName);
	void getNetShare(wstring& serverName);
	static vector<wstring> getDomainGroupMembers(wstring serverName, wstring groupName);
	static vector<wstring> getLocalGroupMembers(wstring serverName, wstring groupName);
	//static vector<wstring> getLocalGroupMembersSamr(wstring serverName);
	DWORD closeConnection(string lpRemoteName);
};
#endif
