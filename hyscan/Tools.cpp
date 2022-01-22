#include "Tools.h"
#include <stdarg.h>
string wchar2Char(wchar_t* srcString)
{
	char* m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, srcString, wcslen(srcString), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, srcString, wcslen(srcString), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	string w_str(m_char);
	return w_str;
}

wstring string2Wstring(const string& srcString)
{
	int num = MultiByteToWideChar(CP_UTF8, 0, srcString.c_str(), -1, NULL, 0);
	wchar_t *wide = new wchar_t[num];
	MultiByteToWideChar(CP_UTF8, 0, srcString.c_str(), -1, wide, num);
	std::wstring w_str(wide);
	delete[] wide;
	return w_str;
}

string gbk2Utf8(const string& srcString)
{
	int len = MultiByteToWideChar(CP_ACP, 0, srcString.c_str(), -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, srcString.c_str(), -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	string strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;
}

string utf82Gbk(const string& srcString)
{	
	int len = MultiByteToWideChar(CP_UTF8, 0, srcString.c_str(), -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, srcString.c_str(), -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}

/*
int cidr2ipmask(char* cidr, unsigned int* ip, unsigned int* mask)
{
	//unsigned char cidr_[0x32];
	unsigned char b[5];
	//memcpy((char*)cidr_, cidr, 0x32);
	sscanf(cidr, "%hhx.%hhx.%hhx.%hhx/%hhx", &b[0], &b[1], &b[2], &b[3], &b[4]);
	if (b[4] > 32) {
		return -1;
	}
	
	*ip = (b[0] << 24UL) | (b[1] << 16UL) | (b[2] << 8UL) | (b[3]);
	*mask = (0xFFFFFFFFUL << (32 - b[4])) & 0xFFFFFFFFUL;
}*/

string int2ip(unsigned int num)
{
	string strRet = "";
	for (int i = 0; i < 4; i++)
	{
		uint32_t tmp = (num >> ((3 - i) * 8)) & 0xFF;

		char chBuf[8] = "";
		_itoa_s(tmp, chBuf, 10);
		strRet += chBuf;

		if (i < 3)
		{
			strRet += _IP_MARK;
		}
	}

	return strRet;
}

vector<string> split(const string& str, const string& delim) {
	vector<string> vString;
	if ("" == str) return vString;
	//�Ƚ�Ҫ�и���ַ�����string����ת��Ϊchar*����  
	char* strs = new char[str.length() + 1]; //��Ҫ����  
	strcpy(strs, str.c_str());

	char* d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());
	
	char* p = strtok(strs, d);
	while (p) {
		string s = p; //�ָ�õ����ַ���ת��Ϊstring����  
		vString.push_back(s); //����������  
		p = strtok(NULL, d);
	}

	delete d;
	delete strs;
	return vString;
}

BYTE str2byte(const string &str)
{
	BYTE bRet = 0x00;	//���
	size_t iPos = 1;	//λ
	size_t power = 1;	//�ݴ�

	//û�ҵ�"x"����
	/*
	if (std::string::npos == str.find("x"))
	{
	return false;
	}*/

	//������������ȡÿ���ַ�
	while (str.find("x") != (str.length() - iPos))
	{
		char cVal = str[str.length() - iPos];
		int iVal = int(cVal);

		//0~9
		if ((iVal >= 48) && (iVal <= 57))
		{
			bRet += ((iVal - 48) * power);
		}
		//A~F
		else if ((iVal >= 65) && (iVal <= 70))
		{
			bRet += ((iVal - 55) * power);
		}
		//a~f
		else if ((iVal >= 97) && (iVal <= 102))
		{
			bRet += ((iVal - 87) * power);
		}

		++iPos;
		power *= 16;
	}

	return bRet;
}

string getHexString(const string& jsonString){
	string finaString;
	string tempString;

	if (jsonString.empty()){
		finaString = "";
		return finaString;
	}

	finaString.resize(jsonString.size()); //Ĭ�ϳ�ʼ����Ϊ��󣬺��������е���

	DWORD dwIndex = 0;
	DWORD i = 0, j;
	DWORD dwStringSize = 0;
	while (i < jsonString.size()){
		// ÿ��ȡ�����ַ�
		tempString = jsonString.substr(i, 2);

		// �ж��Ƿ�Ϊ���� \x �������ַ�
		if (tempString.find("\\x") != string::npos){
			// ���������ô�� ������2���ַ����ܹ�4���ַ�����ת��Ϊʮ������
			tempString = jsonString.substr(i, 4);
			finaString[dwIndex] = str2byte(tempString);
			dwIndex++;

			i += 4; // ƫ��+4
			dwStringSize++; // ������С+1���ֽڴ�С
		}
		else{
			// ���û����ô��ǰ�������ַ�������ͨ�ַ�
			for (j = 0; j < 2; j++, dwIndex++)
			{
				finaString[dwIndex] = tempString[j];
			}

			i += 2; // ƫ��+2
			dwStringSize += 2; // ������С+2���ֽڴ�С
		}
	}

	finaString.resize(dwStringSize);
	return finaString;
}