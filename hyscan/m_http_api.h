#ifndef HTTPCLIENT_H_H
#define HTTPCLIENT_H_H
#include "public.h"
#include "Tools.h"
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
class m_http_api
{
public:
	m_http_api();
	~m_http_api();
	//HttpRequest requestInit();
	string getHttpTitle(string& text);
	void request(string& url, string& port);
};


// �ͷ��ţ��Ժ���ʱ��Ļ���д�ıȽ�������
class HttpResponse{
	map <string, string> header;
	string url;
	string text;
	string status_code;
};


class HttpOption{
	
};


class HttpRequest{
public:
	HttpRequest(){

	}

	~HttpRequest(){
		
	}
	void request(string& url, WORD wPort);
};
#endif
