#include "s_weak_scanner.h"


WeakScanner::WeakScanner()
{

}


WeakScanner::~WeakScanner()
{

}

void WeakScanner::check(string& serverName, string& domainName, string& domainUsername, string& domainPassowrd, string& personalPassword){
	DWORD dwRet;
	ULONG dstMac[2] = { 0 };
	ULONG MacAddr[2] = { 0 };
	ULONG PhysAddrLen = 6;

	// ====== ��ȡ����������
	/*
	TCHAR hostName[MAX_PATH] = { 0 };
	gethostname(hostName, sizeof(hostName));
	cout << hostName << endl;*/

	// domainName ��Сд����


	// ====== ��ȡ��������IP
	HOST_ENT* remoteHost = NULL;		// ��Ž����������
	remoteHost = gethostbyname(serverName.c_str());
	if (remoteHost != NULL){
		if (remoteHost->h_addrtype == AF_INET){
			DWORD i;
			for (i = 0; remoteHost->h_addr_list[i]; i++){
				string ip = inet_ntoa(*(IN_ADDR*)remoteHost->h_addr_list[i]);
				// ===== �ж��Ƿ���
				dwRet = SendARP(inet_addr(ip.c_str()), 0, &MacAddr, &PhysAddrLen);
				if (dwRet == NO_ERROR){
#ifdef DEBUG
					cout << "[+] " << ip << " is alive" << endl;
#else
#endif
					DWORD j;
					vector<wstring> vLocalMembers = WNET_API::getLocalGroupMembers(string2Wstring(ip), L"administrators");
					for (j = 0; j<vLocalMembers.size(); j++){

						string accountString = string(wchar2Char((WCHAR*)vLocalMembers[j].c_str()));

						// ��ǰaccountStringΪ����������
						if (accountString.find(R"(\)") != string::npos && accountString.find(domainName) != string::npos){
							if (accountString.find("Domain Users") != string::npos
								|| accountString.find("domain users") != string::npos){
								// cout << "[+] " << ip << " Domain Users " << endl;
								// domain users �������
								// ���������һ���ṩ�����룬һ��û���ṩ����
								DWORD dwRet;
								DWORD dwSuccess = 0;
								WNET_API wnetApi;
								if (!domainPassowrd.empty()){
									// �ṩ���˺ţ�����������
									dwRet = wnetApi.openConnectByUserPass(ip, domainUsername, domainPassowrd);
									if (dwRet == IPC_SUCCESS){
										dwSuccess = 1;
										cout << "[+] " << ip << " " << accountString << " " << domainUsername << "/" << domainPassowrd << " Successed" << endl;
									}
									if (dwSuccess == 1)
										break;
								}
								else if (domainPassowrd.empty()){
									// �ṩ���˺ţ������������£�����Ĭ��ƾ֤ȥ�������ֲ�������϶����ԣ���ΪĬ��ƾ֤����Ĭ�������ӳɹ���
									dwRet = wnetApi.openConnectBySelf(ip);
									if (dwRet == IPC_SUCCESS){
										dwSuccess = 1;
										cout << "[+] " << ip << " " << accountString << " " << domainUsername << "/(self)" << " Successed" << endl;
									}
									else{
#ifdef DEBUG
										cout << "[+] " << ip << " " << accountString << " " << domainUsername << "/(self)" << " Failed" << endl;
#else
#endif
									}
									if (dwSuccess == 1)
										break;
								}
							}
							else if (accountString.find("Domain Admins") != string::npos){
								// Ĭ����صı������û�����������
								cout << "[+] " << ip << " Domain Admins -> " << "Skip" << endl;
								continue;
							}
							else if (accountString.find("Enterprise Admins") != string::npos){
								// Ĭ����صı������û�����������
								cout << "[+] " << ip << " Enterprise Admins -> " << "Skip" << endl;
								continue;
							}
							else if (accountString.find("Administrator") != string::npos){
								// Ĭ����� HENGGE\administrator���������Ҳ�Ǳ�����Ĺ���Ա���������Ҳ��Ҫ��ö�ٲ���
								DWORD dwRet;
								DWORD dwSuccess = 0;
								DWORD k;
								cout << "[+] " << "Domain\\Administrator -> " << accountString << endl;
								WNET_API wnetApi;
								// R"(pass!@#QWE)" R"(123456)" R"(P@ssw0rd)"
								vector<string> vPasswordArray = {
									R"(admin@123)", R"(pass!@#QWE)" R"(123456)", R"(P@ssw0rd)", personalPassword.c_str() };
								for (k = 0; k < vPasswordArray.size(); k++){
									dwRet = wnetApi.openConnectByUserPass(ip, "Administrator", vPasswordArray[k]);
									if (dwRet == IPC_SUCCESS){
										dwSuccess = 1;
										cout << "[+] " << ip << " " << accountString << " " << vPasswordArray[k] << " Successed" << endl;
										break;
									}
									else{
#ifdef DEBUG
										cout << "[+] " << ip << " " << accountString << " " << vPasswordArray[k] << " Failed" << endl;

#else
#endif
									}
								}
								if (dwSuccess == 1)
									break;
							}
							else if (accountString.find(domainUsername) != string::npos){
								// administrators������ hengge\ske �������û���������domain users�������飬�������û��Ļ�Ҳ��Ҫö�ٲ���
								// ͬ����Ϊ�������

								// ���û�Ϊ��ǰ hengge\ske ����Ϊ��ǰ�û�
								// ���û�Ϊ��ǰ hengge\ske ����Ϊ��ǰ�û�������û�б�Ҫ
								DWORD dwRet;
								DWORD dwSuccess = 0;
								WNET_API wnetApi;
								dwRet = wnetApi.openConnectBySelf(ip);
								if (dwRet == IPC_SUCCESS){
									dwSuccess = 1;
									cout << "[+] " << ip << " " << domainUsername << "/(self)" << " Successed" << endl;
								}
								else{
#ifdef DEBUG
									cout << "[+] " << ip << " " << domainUsername << "/(self)" << " Failed" << endl;

#else
#endif
								}
								if (dwSuccess == 1)
									break;
							}
							else if (accountString.find(" ") != string::npos){
								// ���ڿո�δ֪domain Group�������
								cout << "[+] " << ip << " Unknown Groups -> " << accountString << endl;
							}
						}
						else{
							// ��ǰaccountStringΪ������������
							if (accountString.find("Administrator") != string::npos){
								// �����������administrator��Ա
								DWORD dwRet;
								DWORD dwSuccess = 0;
								DWORD k;
								// cout << "[+] " << accountString << " -> Administrator...." << endl;
								WNET_API wnetApi;
								vector<string> vPasswordArray = { R"(admin@123)", R"(pass!@#QWE)" R"(123456)",
									R"(P@ssw0rd)", personalPassword.c_str() };
								for (k = 0; k < vPasswordArray.size(); k++){
									dwRet = wnetApi.openConnectByUserPass(ip, "Administrator", vPasswordArray[k]);
									if (dwRet == IPC_SUCCESS){
										dwSuccess = 1;
										cout << "[+] " << ip << " " << accountString << "/" << vPasswordArray[k] << " Successed" << endl;
										break;
									}
									else{
#ifdef DEBUG
										cout << "[+] " << ip << " " << accountString << "/" << vPasswordArray[k] << " Failed" << endl;
#else
#endif
									}
								}
								if (dwSuccess == 1)
									break;
							}
							else if (accountString.find(domainName) == string::npos){
								DWORD dwRet;
								DWORD dwSuccess = 0;
								DWORD p;
								// ���������Ĭ����administrators���µĲ���administrator���û�
								// cout << "[+] " << ip << " " << accountString << " -> not Administrator...." << endl;
								vector<string> vString = split(accountString, R"(\)");
								if (vString.size() == 2){
									WNET_API wnetApi;
									vector<string> vPasswordArray = { vString[1] + R"(@123)", vString[1],
										R"(admin@123)", R"(pass!@#QWE)", R"(123456)", R"(P@ssw0rd)", personalPassword.c_str() };
									for (p = 0; p<vPasswordArray.size(); p++){
										dwRet = wnetApi.openConnectByUserPass(ip, vString[1], vPasswordArray[p]);
										if (dwRet == IPC_SUCCESS){
											dwSuccess = 1;
											cout << "[+] " << ip << " " << accountString << "/" << vPasswordArray[p] << " Successed" << endl;
											break;
										}
										else{
#ifdef DEBUG
											cout << "[+] " << ip << " " << accountString << "/" << vPasswordArray[p] << " Failed" << endl;
#else
#endif
										}
									}
									if (dwSuccess == 1)
										break;
								}
							}
						}
					}
				}
			}
		}
	}
}
