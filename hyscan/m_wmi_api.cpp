#include "m_wmi_api.h"
#include "Tools.h"

WMIAPI::WMIAPI()
{
	pWbemSvc = NULL;
	pWbemLoc = NULL;
	pWbemClsObj = NULL;
	pEnumClsObj = NULL;
}


WMIAPI::~WMIAPI()
{
	
}

// https://docs.microsoft.com/en-us/windows/win32/api/wbemcli/nn-wbemcli-iwbemservices

HRESULT WMIAPI::init(){
	HRESULT hr;
	hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (SUCCEEDED(hr))
	{
		hr = CoInitializeSecurity(NULL,
			-1,
			NULL,
			NULL,
			RPC_C_AUTHN_LEVEL_PKT,
			RPC_C_IMP_LEVEL_IMPERSONATE,
			NULL,
			EOAC_NONE,
			NULL);
		if (SUCCEEDED(hr)){
			//��������һ��WMI�����ռ����ӣ�����һ��CLSID_WbemLocator����  
			hr = CoCreateInstance(CLSID_WbemLocator,
				0,
				CLSCTX_INPROC_SERVER,
				IID_IWbemLocator,
				(LPVOID*)&pWbemLoc);
			return hr;
		}
	}
	return hr;
}

void WMIAPI::release(){
	if (pWbemSvc != NULL)
	{
		pWbemSvc->Release();
		pWbemSvc = NULL;
	}
	if (pWbemLoc != NULL)
	{
		pWbemLoc->Release();
		pWbemLoc = NULL;

	}
	if (pEnumClsObj != NULL)
	{
		pEnumClsObj->Release();
		pEnumClsObj = NULL;
	}
	CoUninitialize();
}

void WMIAPI::connect(string& machineName){
	HRESULT hr;
	wstring connectAddr;
	hr = this->init();
	if (SUCCEEDED(hr) || RPC_E_CHANGED_MODE == hr)
	{
		//���ý��̵İ�ȫ���𣬣�����COM���ʱ�ڳ�ʼ��COM֮��Ҫ����CoInitializeSecurity���ý��̰�ȫ���𣬷���ᱻϵͳʶ��Ϊ������  
		connectAddr.append(L"\\\\");
		connectAddr.append(string2Wstring(machineName));
		connectAddr.append(L"\\root\\CIMV2");
		hr = pWbemLoc->ConnectServer(
			(BSTR)connectAddr.c_str(),  // Namespace
			NULL,          // Userid
			NULL,           // PW
			NULL,           // Locale
			0,              // flags
			NULL,           // Authority
			NULL,           // Context
			&pWbemSvc
			);
		if (SUCCEEDED(hr)){
			cout << "\t[+] " << machineName << " Connection success" << endl;
		}
	}
}

void WMIAPI::exec(string& command){
	
}
