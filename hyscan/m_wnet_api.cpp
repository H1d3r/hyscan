#include "m_wnet_api.h"

WNET_API::WNET_API()
{
	
}

WNET_API::~WNET_API()
{
	
}

DWORD WNET_API::openConnectBySelf(string serverName){
	// cout << "[+] Calling WNetAddConnection2 with " << serverName << endl;

	NETRESOURCE netResource;
	RtlZeroMemory(&netResource, sizeof(NETRESOURCE));
	string fmtRemoteName;
	fmtRemoteName.append("\\\\");
	fmtRemoteName.append(serverName);
	fmtRemoteName.append("\\c$");
	netResource.dwType = RESOURCETYPE_ANY;
	netResource.lpLocalName = NULL;
	netResource.lpRemoteName = (LPSTR)fmtRemoteName.c_str();
	netResource.lpProvider = NULL;
	
	DWORD dwRet = WNetAddConnection2(&netResource, NULL, NULL, CONNECT_TEMPORARY);
	if (dwRet == NO_ERROR){
		this->closeConnection(fmtRemoteName);
	}
	return dwRet;
}

DWORD WNET_API::openConnectByUserPass(string serverName, string username, string password){
	// cout << "[+] Calling WNetAddConnection2 with " << serverName << endl;

	NETRESOURCE netResource;
	RtlZeroMemory(&netResource, sizeof(NETRESOURCE));
	string fmtRemoteName;
	fmtRemoteName.append("\\\\");
	fmtRemoteName.append(serverName);
	fmtRemoteName.append("\\c$");
	netResource.dwType = RESOURCETYPE_ANY;
	netResource.lpLocalName = NULL;
	netResource.lpRemoteName = (LPSTR)fmtRemoteName.c_str();
	netResource.lpProvider = NULL;

	DWORD dwRet = WNetAddConnection2(&netResource, password.c_str(), username.c_str(), CONNECT_TEMPORARY);
	switch (dwRet)
	{
	case IPC_SUCCESS:
		printf("[+] %s Connection success\n", netResource.lpRemoteName);
		WNetCancelConnection2(netResource.lpLocalName, CONNECT_UPDATE_PROFILE, TRUE);
		this->closeConnection(fmtRemoteName);
	case IPC_PRIVILEGE_ERROR:
		printf("\t[-] %s The privilege wrong\n", netResource.lpRemoteName);
	case IPC_NETWORK_ERROR:
		printf("\t[-] %s The network name could not be found\n", netResource.lpRemoteName);
	case IPC_USER_PASS_ERROR:
		printf("\t[-] %s The username or password is incorrect\n", netResource.lpRemoteName);
	case IPC_PASS_EXPIRE:
		printf("\t[-] %s The password is expired\n", netResource.lpRemoteName);
	default:
		printf("\t[-] %s WNetAddConnection2 failed with error\n", netResource.lpRemoteName);
	}
	
	return dwRet;
}

DWORD WNET_API::closeConnection(string lpRemoteName){
	DWORD dwRetVal;
	dwRetVal = WNetCancelConnection2(lpRemoteName.c_str(), 0, TRUE);

	if (dwRetVal == NO_ERROR)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void WNET_API::getLoggedUsers(wstring serverName){
	NET_API_STATUS nStatus;
	LPSESSION_INFO_10 pBuf = NULL;
	LPSESSION_INFO_10 pTmpBuf;
	DWORD dwLevel = 10;
	DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
	DWORD dwEntriesRead = 0;
	DWORD dwTotalEntries = 0;
	DWORD dwResumeHandle = 0;
	DWORD i;
	DWORD dwTotalCount = 0;
	//LPTSTR pszServerName = NULL;
	do // begin do
	{
		nStatus = NetSessionEnum((LPWSTR)serverName.c_str(), NULL, NULL, dwLevel, (LPBYTE*)&pBuf, dwPrefMaxLen,
			&dwEntriesRead,
			&dwTotalEntries,
			&dwResumeHandle);
		
		if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
		{
			if ((pTmpBuf = pBuf) != NULL)
			{
				//
				// Loop through the entries.
				//
				for (i = 0; (i < dwEntriesRead); i++)
				{

					if (pTmpBuf == NULL)
					{
						fprintf(stderr, "An access violation has occurred\n");
						break;
					}
					//
					// Print the retrieved data. 
					//

					//sprintf(url, "%ws -> %ws\n", pTmpBuf->sesi10_cname,pTmpBuf->sesi10_username);

					SYSTEMTIME sys;
					GetLocalTime(&sys);
					char current_time[64] = { NULL };
					sprintf(current_time, "%4d-%02d-%02d %02d:%02d:%02d ", sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);

					printf("[%s]  [%ws]  [%ws]  [%ws]\n", current_time, serverName, pTmpBuf->sesi10_cname, pTmpBuf->sesi10_username);
					/*
					wprintf(L"\n\tClient: %s\n", pTmpBuf->sesi10_cname);
					wprintf(L"\tUser:   %s\n", pTmpBuf->sesi10_username);
					printf("\tActive: %d\n", pTmpBuf->sesi10_time);
					printf("\tIdle:   %d\n", pTmpBuf->sesi10_idle_time);
					*/
					pTmpBuf++;
					dwTotalCount++;
				}
			}
		}
		//
		// Otherwise, indicate a system error.
		//
		else
			fprintf(stderr, "A system error has occurred: %d\n", nStatus);
		//
		// Free the allocated memory.
		//
		if (pBuf != NULL)
		{
			delete pBuf;
			pBuf = NULL;
		}
	}
	// 
	// Continue to call NetSessionEnum while 
	//  there are more entries. 
	// 
	while (nStatus == ERROR_MORE_DATA); // end do

	// Check again for an allocated buffer.
	//
	if (pBuf != NULL)
		delete pBuf;
}

/////////////////////// @ske��ʦ��
// @param serverName
// @param groupName
vector<wstring> WNET_API::getLocalGroupMembers(wstring serverName, wstring groupName){
	LOCALGROUP_MEMBERS_INFO_2* pGroupInfo;			// LOCALGROUP_MEMBERS_INFO_2�ṹ��÷����뱾�����Ա������SID���ʻ���Ϣ������������buff��Ż�ȡ������Ϣ
	DWORD dwPrefmaxlen = MAX_PREFERRED_LENGTH;	// ָ���������ݵ���ѡ��󳤶ȣ����ֽ�Ϊ��λ�����ָ��MAX_PREFERRED_LENGTH���ú�������������������ڴ�����
	DWORD dwEntriesRead;						// ָ��һ��ֵ��ָ�룬��ֵ����ʵ��ö�ٵ�Ԫ������
	DWORD dwTotalentries;						//ָ��һ��ֵ��ֵ����ֵ���տ����Ѵӵ�ǰ����λ��ö�ٵ���Ŀ����
	vector<wstring> membersList;
	DWORD dwRet;
	dwRet = NetLocalGroupGetMembers(serverName.c_str(),
		groupName.c_str(), 2, (LPBYTE*)&pGroupInfo, dwPrefmaxlen, &dwEntriesRead, &dwTotalentries, NULL);
	if (dwRet == NO_ERROR)
	{
		for (DWORD i = 0; i < dwEntriesRead; i++){
			// wcout << pGroupInfo[i].lgrmi2_domainandname << " ";
			membersList.push_back(pGroupInfo[i].lgrmi2_domainandname);
		}
		// cout << endl;
	}else{
		printf("[-] WNET_API::getLocalGroupMembers Error, The Error is %d\n", dwRet);
	}
	return membersList;
}


/////////////////////// @ske��ʦ��
// @param serverName
// @param groupName
vector<wstring> WNET_API::getDomainGroupMembers(wstring serverName, wstring groupName){
	DWORD dwLevel = 1;
	GROUP_USERS_INFO_1* bufptr;
	DWORD dwPrefmaxlen = MAX_PREFERRED_LENGTH;
	DWORD dwEntriesread;
	DWORD dwTotalentries;
	DWORD dwRet;
	vector<wstring> membersList;				// ����vector�����������

	dwRet = NetGroupGetUsers(serverName.c_str(), groupName.c_str(), dwLevel, (LPBYTE*)&bufptr, dwPrefmaxlen, &dwEntriesread, &dwTotalentries, NULL);
	
	if (dwRet == NO_ERROR)
	{
		for (DWORD i = 0; i < dwEntriesread; i++)
		{
			wstring nameString(bufptr[i].grui1_name);
			if (*nameString.rbegin() == '$')
				nameString.replace(nameString.end() - 1, nameString.end(), 1, NULL);			// ��������ĩβ��$�滻Ϊ��
			// wcout << nameString << endl;
			membersList.push_back(nameString.c_str());
		}
		
		return membersList;
	}
	else
	{
		printf("[-] WNET_API::getDomainGroupMembers Error, The Error is %d\n", dwRet);
		exit(-1);
	}
}



