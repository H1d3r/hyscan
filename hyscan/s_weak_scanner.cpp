#include "s_weak_scanner.h"
//#define DEBUG 1

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
#else
#endif
					vector<wstring> vLocalMembers = WNET_API::getLocalGroupMembers(string2Wstring(ip), L"administrators");
					for (j = 0; j<vLocalMembers.size(); j++){
						string accountString = string(wchar2Char((WCHAR*)vLocalMembers[j].c_str()));
						string accountCompareString = accountString; // ���ڱȽϵ�ʱ��ͳͳ��ת����Сд�����бȽϣ���Ϊfind�������Сд��
						transform(accountCompareString.begin(), accountCompareString.end(), accountCompareString.begin(), ::tolower);
						printf("account: %s\n", accountCompareString.data());
						// ��ǰaccountStringΪ����������
						if (accountCompareString.find("\\") != string::npos 
							&& accountCompareString.find(domainNameCompareString) != string::npos){
							
							if (accountString.find("domain users") != string::npos){
								printf("[+] %s Domain Users\n", ip.data());
								// domain users �������
								// ���������һ���ṩ�����룬һ��û���ṩ����
								if (!domainPassowrd.empty()){
									// �ṩ���˺ţ�����������
									dwRet = wnetApi.openConnectByUserPass(ip, domainUsername, domainPassowrd);
									if (dwRet == IPC_SUCCESS){
										dwSuccess = 1;
										printf("[+] %s %s %s/%s Successed\n", ip.data(), accountString.data(), domainUsername.data(), domainPassowrd.data());
									}else{
#ifdef DEBUG
										printf("[-] %s %s %s/%s Failed\n", ip.data(), accountString.data(), domainUsername.data(), domainPassowrd.data());
#else
#endif
									}
									if (dwSuccess == 1)
										break;
								}
								else if (domainPassowrd.empty()){
									// �ṩ���˺ţ������������£�����Ĭ��ƾ֤ȥ�������ֲ�������϶����ԣ���ΪĬ��ƾ֤����Ĭ�������ӳɹ���
									dwRet = wnetApi.openConnectBySelf(ip);
									if (dwRet == IPC_SUCCESS){
										dwSuccess = 1;
										printf("[+] %s %s %s/(self) Successed\n", ip.data(), accountString.data(), domainUsername.data());
									}else{
#ifdef DEBUG
										printf("[-] %s %s %s/(self) Failed\n", ip.data(), accountString.data(), domainUsername.data());
#else
#endif
									}
									if (dwSuccess == 1)
										break;
								}
							}
							else if (accountCompareString.find("domain admins") != string::npos){
								// Ĭ����صı������û�����������
								printf("[+] %s Domain Admins -> Skip\n", ip.data());
								continue;
							}
							else if (accountCompareString.find("enterprise admins") != string::npos){
								// Ĭ����صı������û�����������
								printf("[+] %s Enterprise Admins -> Skip\n", ip.data());
								continue;
							}
							else if (accountCompareString.find("administrator") != string::npos){
								// Ĭ����� HENGGE\administrator���������Ҳ�Ǳ�����Ĺ���Ա���������Ҳ��Ҫ��ö�ٲ���
								DWORD k;
								printf("[+] Domain\\Administrator -> %s\n", accountString.data());
								vector<string> vPasswordArray = {
									R"(admin@123)", R"(pass!@#QWE)" R"(123456)", R"(P@ssw0rd)", personalPassword.c_str() };
								for (k = 0; k < vPasswordArray.size(); k++){
									dwRet = wnetApi.openConnectByUserPass(ip, "Administrator", vPasswordArray[k]);
									if (dwRet == IPC_SUCCESS){
										dwSuccess = 1;
										printf("[+] %s %s %s Successed\n", ip.data(), accountString.data(), vPasswordArray[k].data());
										break;
									}
									else{
#ifdef DEBUG
										printf("[+] %s %s %s Failed\n", ip.data(), accountString.data(), vPasswordArray[k].data());

#else
#endif
									}
								}
								if (dwSuccess == 1)
									break;
							}
							else if (accountCompareString.find(domainNameCompareString) != string::npos){
								// administrators������ hengge\ske �������û���������domain users�������飬�������û��Ļ�Ҳ��Ҫö�ٲ���
								// ͬ����Ϊ�������

								// ���û�Ϊ��ǰ hengge\ske ����Ϊ��ǰ�û�
								// ���û�Ϊ��ǰ hengge\ske ����Ϊ��ǰ�û�������û�б�Ҫ
								dwRet = wnetApi.openConnectBySelf(ip);
								if (dwRet == IPC_SUCCESS){
									dwSuccess = 1;
									// cout << "[+] " << ip << " " << domainUsername << "/(self)" << " Successed" << endl;
									printf("[+] %s %s/(self) Successed\n", ip.data(), domainUsername.data());

								}
								else{
#ifdef DEBUG
									printf("[+] %s %s/(self) Failed\n", ip.data(), domainUsername.data());

#else
#endif
								}
								if (dwSuccess == 1)
									break;
							}
							else if (accountCompareString.find("domain admins") == string::npos
								&& accountCompareString.find("enterprise admins") == string::npos){
								// ���ڿո�δ֪domain Group�������
								printf("[+] %s Unknown Groups -> %s\n", ip.data(), accountString.data());
							}
						}
						else{
							// ��ǰaccountStringΪ������������
							if (accountCompareString.find("administrator") != string::npos){
								// �����������administrator��Ա
								DWORD k;
								// cout << "[+] " << accountString << " -> Administrator...." << endl;
								printf("[+] %s -> Administrator... \n", accountString.data());
								vector<string> vPasswordArray = { R"(admin@123)", R"(pass!@#QWE)", R"(123456)",
									R"(P@ssw0rd)", personalPassword.c_str() };
								for (k = 0; k < vPasswordArray.size(); k++){
									dwRet = wnetApi.openConnectByUserPass(ip, "Administrator", vPasswordArray[k]);
									// cout << "===" << dwRet << endl;
									if (dwRet == IPC_SUCCESS){
										dwSuccess = 1;
										printf("[+] %s %s/%s Successed\n", ip.data(), accountString.data(), vPasswordArray[k].data());
										break;
									}
									else{
#ifdef DEBUG
										printf("[+] %s %s/%s Failed\n", ip.data(), accountString.data(), vPasswordArray[k].data());
#else
#endif
									}
								}	
								if (dwSuccess == 1)
									break;
							}
							else if (accountCompareString.find(domainNameCompareString) == string::npos){
								DWORD p;
								// ���������Ĭ����administrators���µĲ���administrator���û�
								// cout << "[+] " << ip << " " << accountString << " -> not Administrator...." << endl;
								printf("[+] %s %s -> not Administrator\n", ip.data(), accountString.data());
								vector<string> vString = split(accountString, R"(\)");
								if (vString.size() == 2){
									vector<string> vPasswordArray{ vString[1], vString[1] + R"(@123)",
										R"(admin@123)", R"(pass!@#QWE)", R"(123456)", R"(P@ssw0rd)", personalPassword.c_str() };
									for (p = 0; p<vPasswordArray.size(); p++){
										dwRet = wnetApi.openConnectByUserPass(ip, accountString, vPasswordArray[p]);
										if (dwRet == IPC_SUCCESS){
											dwSuccess = 1;
											printf("[+] %s %s/%s Successed\n", ip.data(), accountString.data(), vPasswordArray[p].data());
											break;
										}
										else{
#ifdef DEBUG
											printf("[+] %s %s/%s Failed\n", ip.data(), accountString.data(), vPasswordArray[p].data());
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
