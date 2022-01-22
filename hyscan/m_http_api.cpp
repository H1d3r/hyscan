#include "m_http_api.h"


m_http_api::m_http_api()
{
}


m_http_api::~m_http_api()
{
}

string m_http_api::getHttpTitle(string& text){
	// ��������ǩ���ϵ���
	smatch matchResult;
	string::const_iterator iterStart = text.begin();
	string::const_iterator iterEnd = text.end();
	// title��ǩ
	if (regex_search(iterStart, iterEnd, matchResult, regex("<title>(.*)</title>"), regex_constants::format_first_only)){
		//cout << matchResult[1] << endl;
		return matchResult[1];
	}
	// h1
	if (regex_search(iterStart, iterEnd, matchResult, regex("<h1>(.*)</h1>"), regex_constants::format_first_only)){
		//cout << matchResult[1] << endl;

		return matchResult[1];
	}
	// h2
	if (regex_search(iterStart, iterEnd, matchResult, regex("<h2>(.*)</h2>"), regex_constants::format_first_only)){
		//cout << matchResult[1] << endl;

		return matchResult[1];
	}
	// h3
	if (regex_search(iterStart, iterEnd, matchResult, regex("<h3>(.*)</h3>"), regex_constants::format_first_only)){
		//cout << matchResult[1] << endl;

		return matchResult[1];
	}
	// h4
	if (regex_search(iterStart, iterEnd, matchResult, regex("<meta name=\"description\" content=\"(.*)\">"), regex_constants::format_first_only)){
		//cout << matchResult[1] << endl;

		return matchResult[1];
	}
	return "";
}
/*
HttpRequest m_http_api::requestInit(){
	HttpRequest a;
	return a;
}*/


void HttpRequest::request(string& url, WORD port){
	HINTERNET hSession = NULL;
	HINTERNET hConnect = NULL;
	HINTERNET hRequest = NULL;
	m_http_api httpApi;
	setlocale(0, "");

	//1. ��ʼ��һ��WinHTTP-session���������1Ϊ�˾��������
	hSession = WinHttpOpen(L"Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36", NULL, NULL, NULL, NULL);
	if (hSession == NULL) {
#ifdef DEBUG
		cout << "Error:Open session failed: " << GetLastError() << endl;
#else
#endif
		return;
	}
	// ��ʱʱ������
	WinHttpSetTimeouts(hRequest, 5000, 5000, 0, 0);

	//2. ͨ������������ӵ�����������Ҫָ��������IP�Ͷ˿ںš������ӳɹ������ص�hConnect�����ΪNULL
	hConnect = WinHttpConnect(hSession, string2Wstring(url).c_str(), (INTERNET_PORT)port, 0);
	if (hConnect == NULL) {
#ifdef DEBUG
		cout << "Error:Connect failed: " << GetLastError() << endl;
#else
#endif
		return;
	}

	//3. ͨ��hConnect�������һ��hRequest��������ڷ����������ȡ�ӷ��������ص����ݡ�
	if (port == 443){
		// use SSL if applicable (HTTPS)
		hRequest = WinHttpOpenRequest(hConnect, L"Get", NULL, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
		// hRequest = WinHttpOpenRequest(hConnect, L"Post", L"getServiceInfo", NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
	}
	else{
		hRequest = WinHttpOpenRequest(hConnect, L"Get", NULL, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
	}

	
	// ��ѡ֤��
	//BOOL fRet = WinHttpSetOption(hRequest, WINHTTP_OPTION_CLIENT_CERT_CONTEXT, WINHTTP_NO_CLIENT_CERT_CONTEXT, 0);

	//���в���2��ʾ����ʽ���˴�ΪPost������3:����Post�ľ����ַ��������ľ����ַΪhttp://192.168.50.112/getServiceInfo
	if (hRequest == NULL) {
#ifdef DEBUG
		cout << "Error:OpenRequest failed: " << GetLastError() << endl;
#else
#endif
		return;
	}

	BOOL bResults;

	// header add
	// bResults = WinHttpAddRequestHeaders(hRequest, L"", (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD);

	// �����������post����
	//string data = "This is my data to be sent";
	//const void *ss = (const char *)data.c_str();

	// POST 
	//bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, const_cast<void*>(ss), data.length(), data.length(), 0);

	// GET
	bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);

	if (!bResults){
#ifdef DEBUG
		cout << "Error:SendRequest failed: " << GetLastError() << endl;
#else
#endif
		return;
	}
	else{
		//��3�� ��������ɹ���׼�����ܷ�������response��
		// ע�⣺��ʹ�� WinHttpQueryDataAvailable��WinHttpReadDataǰ����ʹ��WinHttpReceiveResponse����access���������ص�����
		bResults = WinHttpReceiveResponse(hRequest, NULL);
	}

	//4-2. ��ȡ�������������ݵ�header��Ϣ����һ����������ȡ�������ݵ��������͡�

	DWORD dwSize = 0;
	DWORD dwMaxSize = 0;
	/*
	PWCHAR lpHeaderBuffer = NULL;
	BOOL bUtf8 = FALSE;
	// ��ȡ��Ӧ����headerͷ
	if (bResults)
	{
		// ��ȡheader�ĳ���
		WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_RAW_HEADERS_CRLF,
			WINHTTP_HEADER_NAME_BY_INDEX, NULL,
			&dwSize, WINHTTP_NO_HEADER_INDEX);

		// ����header�ĳ���Ϊbuffer�����ڴ�ռ�
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		{
			lpHeaderBuffer = new WCHAR[dwSize / sizeof(WCHAR)];

			if (lpHeaderBuffer != NULL){
				// ʹ��WinHttpQueryHeaders��ȡheader��Ϣ
				bResults = WinHttpQueryHeaders(hRequest,
					WINHTTP_QUERY_RAW_HEADERS_CRLF,
					WINHTTP_HEADER_NAME_BY_INDEX,
					lpHeaderBuffer, &dwSize,
					WINHTTP_NO_HEADER_INDEX);
				if (wcsstr(lpHeaderBuffer, L"charset=utf-8") != NULL){
					bUtf8 = TRUE;
				}
			}
		}
	}*/
	


	//��������header��Ϣ�ᷢ�ַ������������ݵ�charsetΪuft-8������ζ�ź�����Ҫ�Ի�ȡ����raw data���п��ַ�ת����һ��ʼ����û����ʶ����Ҫ����ת�����Եõ������ݶ������롣
	//���������ԭ���ǣ�HTTP�ڴ���������Ƕ�ֵ�ģ�����û��text������unicode�ĸ��HTTPʹ��7bit��ASCII����ΪHTTP headers����������������Ķ�ֵ���ݣ���Ҫ����header��ָ���ı��뷽ʽ����������ͨ����Content-Type header��.
	//��˵�����յ�ԭʼ��HTTP����ʱ���Ƚ��䱣�浽char[] buffer�У�Ȼ������WinHttpQueryHearders()��ȡHTTPͷ���õ����ݵ�Content-Type,�������֪�����ݵ�����ɶ���͵��ˣ���ASCII����Unicode����������
	//һ����֪���˾���ı��뷽ʽ����Ϳ���ͨ��MultiByteToWideChar()����ת���ɺ��ʱ�����ַ�������wchar_t[]�С�

	// ��ȡ��������������
	LPSTR pszOutBuffer = NULL;
	DWORD dwDownloaded = 0;         //ʵ����ȡ���ַ���
	/*
	if (bResults)
	{
		do
		{
			//(1) ��ȡ�������ݵĴ�С�����ֽ�Ϊ��λ��
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize)){
#ifdef DEBUG
				cout << "Error��WinHttpQueryDataAvailable failed��" << GetLastError() << endl;
#else
#endif
				break;
			}
			if (!dwSize)break;  //���ݴ�СΪ0                

			//(2) ���ݷ������ݵĳ���Ϊbuffer�����ڴ�ռ�
			pszOutBuffer = new char[dwSize + 1];
			if (!pszOutBuffer){
#ifdef DEBUG
				cout << "Out of memory." << endl;
#else
#endif
				break;
			}
			ZeroMemory(pszOutBuffer, dwSize + 1);       //��buffer��0

			//(3) ͨ��WinHttpReadData��ȡ�������ķ�������
			if (!WinHttpReadData(hRequest, pszOutBuffer, dwSize, &dwDownloaded)){
#ifdef DEBUG
				cout << "Error��WinHttpQueryDataAvailable failed��" << GetLastError() << endl;
#else
#endif
			}
			if (!dwDownloaded)
				break;

			string title = httpApi.getHttpTitle(string(pszOutBuffer));
			cout << "[+] " << url << " - " << utf82Gbk(title.c_str()) << endl;
			delete[] pszOutBuffer;

		} while (dwSize > 0);

	}*/

	if (bResults)
	{
		//(1) ��ȡ�������ݵĴ�С�����ֽ�Ϊ��λ��
		dwSize = 0;
		dwMaxSize = 1024;
		if (!WinHttpQueryDataAvailable(hRequest, &dwSize)){
#ifdef DEBUG
			cout << "Error��WinHttpQueryDataAvailable failed��" << GetLastError() << endl;
#else
#endif
			return;
		}
		if (!dwSize)return;  //���ݴ�СΪ0   

		dwSize = dwSize > dwMaxSize ? dwMaxSize : dwSize;

		//(2) ���ݷ������ݵĳ���Ϊbuffer�����ڴ�ռ�
		pszOutBuffer = new char[dwSize + 1];
		if (!pszOutBuffer){
#ifdef DEBUG
			cout << "Out of memory." << endl;
#else
#endif
			return;
		}
		ZeroMemory(pszOutBuffer, dwSize + 1);       //��buffer��0

		//(3) ͨ��WinHttpReadData��ȡ�������ķ�������
		if (!WinHttpReadData(hRequest, pszOutBuffer, dwSize, &dwDownloaded)){
#ifdef DEBUG
			cout << "Error��WinHttpQueryDataAvailable failed��" << GetLastError() << endl;
#else
#endif
		}
		if (!dwDownloaded)
			return;
		string title = httpApi.getHttpTitle(string(pszOutBuffer, dwSize));
		printf("[+] %s:%d - %s \n", url.data(), port, utf82Gbk(title).data());

		delete[] pszOutBuffer;

	}


	//5. ���ιر�request��connect��session���
	if (hRequest) WinHttpCloseHandle(hRequest);
	if (hConnect) WinHttpCloseHandle(hConnect);
	if (hSession) WinHttpCloseHandle(hSession);
}