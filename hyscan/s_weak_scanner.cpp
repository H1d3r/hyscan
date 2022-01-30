#include "s_weak_scanner.h"
#define DEBUG 1

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
	// domainName ��Сд����
	string domainNameCompareString = domainName;
	// ���ڱȽϵ�ʱ��ͳͳ��ת����Сд�����бȽϣ���Ϊfind�������Сд��
	transform(domainNameCompareString.begin(), domainNameCompareString.end(), domainNameCompareString.begin(), ::tolower);

	// ====== ��ȡ��������IP
	HOST_ENT* remoteHost = NULL;		// ��Ž����������
	DWORD i, j;

	/////////////////////
	WNET_API wnetApi;
	DWORD dwSuccess = 0;
	remoteHost = gethostbyname(serverName.c_str());
	if (remoteHost != NULL){
		if (remoteHost->h_addrtype == AF_INET){
			for (i = 0; remoteHost->h_addr_list[i]; i++){
				string ip = inet_ntoa(*(IN_ADDR*)remoteHost->h_addr_list[i]);
				// ===== �ж��Ƿ���
				dwRet = SendARP(inet_addr(ip.c_str()), 0, &MacAddr, &PhysAddrLen);
				if (dwRet == NO_ERROR){
#ifdef DEBUG
						//	cout << "[+] " << ip << " is alive" << endl;
#endif
					vector<wstring> vLocalMembers = WNET_API::getLocalGroupMembers(string2Wstring(ip), L"administrators");
					for (j = 0; j<vLocalMembers.size(); j++){
						string accountString = string(wchar2Char((WCHAR*)vLocalMembers[j].c_str()));
						string accountCompareString = accountString; // ���ڱȽϵ�ʱ��ͳͳ��ת����Сд�����бȽϣ���Ϊfind�������Сд��
						transform(accountCompareString.begin(), accountCompareString.end(), accountCompareString.begin(), ::tolower);
						printf("[+] %s Current Account: %s\n", ip.data(), accountString.data());
						// ��ǰaccountStringΪ����������
						if (accountCompareString.find("\\") != string::npos 
							&& accountCompareString.find(domainNameCompareString) != string::npos){
#ifdef DEBUGA
							printf("----1\n");
#endif
							if (accountCompareString.find("domain users") != string::npos){
#ifdef DEBUGA
								printf("----1-1\n");
#endif
								printf("\t[+] %s Domain Users\n", ip.data());
								// domain users �������
								// ���������һ���ṩ�����룬һ��û���ṩ����
								if (!domainPassowrd.empty()){
									// �ṩ���˺ţ�����������
									dwRet = wnetApi.openConnectByUserPass(ip, domainUsername, domainPassowrd);
									if (dwRet == IPC_SUCCESS){
										dwSuccess = 1;
										printf("\t[+] %s %s %s/%s Successed\n", ip.data(), accountString.data(), domainUsername.data(), domainPassowrd.data());
									}else{
#ifdef DEBUG
										printf("\t[-] %s %s %s/%s Failed\n", ip.data(), accountString.data(), domainUsername.data(), domainPassowrd.data());
#else
#endif
									}
									if (dwSuccess == 1)
										break;
								}
								else if (domainPassowrd.empty()){
#ifdef DEBUGA
									printf("----1-2\n");
#endif
									// �ṩ���˺ţ������������£�����Ĭ��ƾ֤ȥ�������ֲ�������϶����ԣ���ΪĬ��ƾ֤����Ĭ�������ӳɹ���
									dwRet = wnetApi.openConnectBySelf(ip);
									if (dwRet == IPC_SUCCESS){
										dwSuccess = 1;
										printf("\t[+] %s %s %s/(self) Successed\n", ip.data(), accountString.data(), domainUsername.data());
									}else{
#ifdef DEBUG
										printf("\t[-] %s %s %s/(self) Failed\n", ip.data(), accountString.data(), domainUsername.data());
#endif
									}
									if (dwSuccess == 1)
										break;
								}
							}
							else if (accountCompareString.find("domain admins") != string::npos){
#ifdef DEBUGA
								printf("----1-3\n");
#endif
								// Ĭ����صı������û�����������
								printf("\t[+] %s Domain Admins -> Skip\n", ip.data());
								continue;
							}
							else if (accountCompareString.find("enterprise admins") != string::npos){
#ifdef DEBUGA
								printf("----1-4\n");
#endif
								// Ĭ����صı������û�����������
								printf("\t[+] %s Enterprise Admins -> Skip\n", ip.data());
								continue;
							}
							else if (accountCompareString.find("administrator") != string::npos){
#ifdef DEBUGA
								printf("----1-5\n");
#endif
								// Ĭ����� HENGGE\administrator���������Ҳ�Ǳ�����Ĺ���Ա���������Ҳ��Ҫ��ö�ٲ���
								DWORD k;
								printf("\t[+] Domain\\Administrator -> %s\n", accountString.data());
								vector<string> vPasswordArray = {
									R"(admin@123)", R"(pass!@#QWE)" R"(123456)", R"(P@ssw0rd)", personalPassword.c_str() };
								for (k = 0; k < vPasswordArray.size(); k++){
									dwRet = wnetApi.openConnectByUserPass(ip, "Administrator", vPasswordArray[k]);
									if (dwRet == IPC_SUCCESS){
										dwSuccess = 1;
										printf("\t[+] %s %s %s Successed\n", ip.data(), accountString.data(), vPasswordArray[k].data());
										break;
									}
									else{
#ifdef DEBUG
										printf("\t[+] %s %s %s Failed\n", ip.data(), accountString.data(), vPasswordArray[k].data());
#endif
									}
								}
								if (dwSuccess == 1)
									break;
							}

							else if (accountCompareString.find(domainNameCompareString) != string::npos){
								// 1��"hengge\www".find("hengge") �������
								// 2��"hengge\abc".find("hengge") �������
								// 3��"hengge\qqq".find("hengge") �������
#ifdef DEBUGA
								printf("----1-6\n");
#endif
								// administrators������ hengge\ske �������û���������domain users�������飬�������û��Ļ�Ҳ��Ҫö�ٲ���
								// ͬ����Ϊ�������

								string domainuserNameCompareString = domainUsername;
								transform(domainuserNameCompareString.begin(), domainuserNameCompareString.end(), 
									domainuserNameCompareString.begin(), ::tolower);
								if (accountCompareString.find(domainuserNameCompareString) != string::npos){
									// 1��"hengge\ske".find("hengge\ske") �������
#ifdef DEBUGA
									printf("----1-7\n");
#endif
									if (domainPassowrd.empty()){
										// ����ṩ�����û����ͱ������û���ͬ����������û���ṩ����ֻ���õ�ǰĬ�ϵ����û�ƾ֤��֤
										dwRet = wnetApi.openConnectBySelf(ip);
										if (dwRet == IPC_SUCCESS){
											dwSuccess = 1;
											printf("\t[+] %s %s/(self) Successed\n", ip.data(), domainUsername.data());
											break;
										}
									}
									else if (!domainPassowrd.empty()){
#ifdef DEBUGA
										printf("----1-8\n");
#endif
										// ����ṩ�����û����ͱ������û���ͬ�����������ṩ���Ǿ��õ�ǰ�����ṩ��������һ����֤
										dwRet = wnetApi.openConnectByUserPass(ip, domainUsername, domainPassowrd);
										if (dwRet == IPC_SUCCESS){
											dwSuccess = 1;
											printf("\t[+] %s %s %s/%s Successed\n", ip.data(), accountString.data(), domainUsername.data(), domainPassowrd.data());
										}
										else{
#ifdef DEBUG
											printf("\t[-] %s %s %s/%s Failed\n", ip.data(), accountString.data(), domainUsername.data(), domainPassowrd.data());
#endif
										}
										if (dwSuccess == 1)
											break;
									}
								}
								else if (accountCompareString.find(domainuserNameCompareString) == string::npos){
									// 1��"hengge\cbd".find("hengge\ske") �������
#ifdef DEBUGA
									printf("----1-9\n");
#endif
									// ���û�Ϊ��ǰ hengge\ske ����Ϊ��ǰ�û�
									// ���־���Ҫ����ö�٣����ǲ���Ҫ������ƾ���ˣ���Ϊ����ƾ�ݶ��Բ��Ϻ�
									if (!domainPassowrd.empty()){
										// ����ṩ�ĵ�ǰ���û����ͱ��������ĵ�ǰ�û���ͬ
										// ���������ṩ���Ǿ�Ĭ�ϵ������ֵ�͵�ǰ�ṩ������������ö�ٲ���
										vector<string> vPasswordArray = { personalPassword.c_str(), domainPassowrd.c_str(),
											R"(admin@123)", R"(pass!@#QWE)", R"(123456)", R"(P@ssw0rd)"};
										for (DWORD k = 0; k < vPasswordArray.size(); k++){
											dwRet = wnetApi.openConnectByUserPass(ip, accountString, vPasswordArray[k]);
											if (dwRet == IPC_SUCCESS){
												dwSuccess = 1;
												printf("\t[+] %s %s/%s Successed\n", ip.data(), accountString.data(), vPasswordArray[k].data());
												break;
											}
											else{
#ifdef DEBUG
												printf("\t[+] %s %s/%s Failed\n", ip.data(), accountString.data(), vPasswordArray[k].data());
#endif
											}
										}
										if (dwSuccess == 1)
											break;
									}
								}
							}
							else if (accountCompareString.find("domain admins") == string::npos
								&& accountCompareString.find("enterprise admins") == string::npos){
#ifdef DEBUGA
								printf("----1-10\n");
#endif
								// ���ڿո�δ֪domain Group�������
								printf("\t[+] %s Unknown Groups -> %s\n", ip.data(), accountString.data());
							}
						}
						else{
							// ��ǰaccountStringΪ������������
							if (accountCompareString.find("administrator") != string::npos){
#ifdef DEBUGA
								printf("----1-11\n");
#endif
								// �����������administrator��Ա
								DWORD k;
								// cout << "[+] " << accountString << " -> Administrator...." << endl;
								printf("\t[+] %s -> Administrator... \n", accountString.data());
								vector<string> vPasswordArray = { R"(admin@123)", R"(pass!@#QWE)", R"(123456)",
									R"(P@ssw0rd)", personalPassword.c_str() };
								for (k = 0; k < vPasswordArray.size(); k++){
									dwRet = wnetApi.openConnectByUserPass(ip, "Administrator", vPasswordArray[k]);
									// cout << "===" << dwRet << endl;
									if (dwRet == IPC_SUCCESS){
										dwSuccess = 1;
										printf("\t[+] %s %s/%s Successed\n", ip.data(), accountString.data(), vPasswordArray[k].data());
										break;
									}
									else{
#ifdef DEBUG
										printf("\t[+] %s %s/%s Failed\n", ip.data(), accountString.data(), vPasswordArray[k].data());
#endif
									}
								}	
								if (dwSuccess == 1)
									break;
							}
							else if (accountCompareString.find(domainNameCompareString) == string::npos){
#ifdef DEBUGA
								printf("----1-12\n");
#endif
								DWORD p;
								// ���������Ĭ����administrators���µĲ���administrator���û�
								// cout << "[+] " << ip << " " << accountString << " -> not Administrator...." << endl;
								printf("\t[+] %s %s -> not Administrator\n", ip.data(), accountString.data());
								vector<string> vString = split(accountString, R"(\)");
								if (vString.size() == 2){
									vector<string> vPasswordArray{ vString[1], vString[1] + R"(@123)",
										R"(admin@123)", R"(pass!@#QWE)", R"(123456)", R"(P@ssw0rd)", personalPassword.c_str() };
									for (p = 0; p<vPasswordArray.size(); p++){
										dwRet = wnetApi.openConnectByUserPass(ip, accountString, vPasswordArray[p]);
										if (dwRet == IPC_SUCCESS){
											dwSuccess = 1;
											printf("\t[+] %s %s/%s Successed\n", ip.data(), accountString.data(), vPasswordArray[p].data());
											break;
										}
										else{
#ifdef DEBUG
											printf("\t[+] %s %s/%s Failed\n", ip.data(), accountString.data(), vPasswordArray[p].data());
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
