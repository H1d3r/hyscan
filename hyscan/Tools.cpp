#include "Tools.h"
#include <stdarg.h>
string wchar2Char(wchar_t* wc)
{
	char* m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wc, wcslen(wc), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wc, wcslen(wc), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	string w_str(m_char);
	return w_str;
}

wstring string2Wstring(const std::string& str)
{
	int num = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t *wide = new wchar_t[num];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, num);
	std::wstring w_str(wide);
	delete[] wide;
	return w_str;
}

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
}

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

// Ĭ�ϵ�base�����
static inline bool is_base64(unsigned char c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
}

string base64decode(string const& encodedString, string base64chars) {
	int in_len = encodedString.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	unsigned char char_array_4[4], char_array_3[3];
	string ret;

	while (in_len-- && (encodedString[in_] != '=') && is_base64(encodedString[in_])) {
		char_array_4[i++] = encodedString[in_]; in_++;
		if (i == 4) {
			for (i = 0; i <4; i++)
				char_array_4[i] = base64chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
				ret += char_array_3[i];
			i = 0;
		}
	}

	if (i) {
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = encodedString.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret += char_array_3[j];
	}

	return ret;
}

string formatString(string& formatStr, ...){
	
	return NULL;
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

std::u16string utf82Utf16(const string& u8str, bool addbom, bool* ok)
{
	u16string u16str;
	u16str.reserve(u8str.size());
	if (addbom) {
		u16str.push_back(0xFEFF);	//bom (�ֽڱ�ʾΪ FF FE)
	}
	std::string::size_type len = u8str.length();

	const unsigned char* p = (unsigned char*)(u8str.data());
	// �ж��Ƿ����BOM(�жϳ���С��3�ֽڵ����)
	if (len > 3 && p[0] == 0xEF && p[1] == 0xBB && p[2] == 0xBF){
		p += 3;
		len -= 3;
	}

	bool is_ok = true;
	// ��ʼת��
	for (std::string::size_type i = 0; i < len; ++i) {
		uint32_t ch = p[i];	// ȡ��UTF8�������ֽ�
		if ((ch & 0x80) == 0) {
			// ���λΪ0��ֻ��1�ֽڱ�ʾUNICODE�����
			u16str.push_back((char16_t)ch);
			continue;
		}
		switch (ch & 0xF0)
		{
		case 0xF0: // 4 �ֽ��ַ�, 0x10000 �� 0x10FFFF
		{
					   uint32_t c2 = p[++i];
					   uint32_t c3 = p[++i];
					   uint32_t c4 = p[++i];
					   // ����UNICODE�����ֵ(��һ���ֽ�ȡ��3bit������ȡ6bit)
					   uint32_t codePoint = ((ch & 0x07U) << 18) | ((c2 & 0x3FU) << 12) | ((c3 & 0x3FU) << 6) | (c4 & 0x3FU);
					   if (codePoint >= 0x10000)
					   {
						   // ��UTF-16�� U+10000 �� U+10FFFF ������16bit��Ԫ��ʾ, �������.
						   // 1����������ȥ0x10000(�õ�����Ϊ20bit��ֵ)
						   // 2��high ������ �ǽ���20bit�еĸ�10bit����0xD800(110110 00 00000000)
						   // 3��low  ������ �ǽ���20bit�еĵ�10bit����0xDC00(110111 00 00000000)
						   codePoint -= 0x10000;
						   u16str.push_back((char16_t)((codePoint >> 10) | 0xD800U));
						   u16str.push_back((char16_t)((codePoint & 0x03FFU) | 0xDC00U));
					   }
					   else
					   {
						   // ��UTF-16�� U+0000 �� U+D7FF �Լ� U+E000 �� U+FFFF ��Unicode�����ֵ��ͬ.
						   // U+D800 �� U+DFFF ����Ч�ַ�, Ϊ�˼���������������������(������򲻱���)
						   u16str.push_back((char16_t)codePoint);
					   }
		}
			break;
		case 0xE0: // 3 �ֽ��ַ�, 0x800 �� 0xFFFF
		{
					   uint32_t c2 = p[++i];
					   uint32_t c3 = p[++i];
					   // ����UNICODE�����ֵ(��һ���ֽ�ȡ��4bit������ȡ6bit)
					   uint32_t codePoint = ((ch & 0x0FU) << 12) | ((c2 & 0x3FU) << 6) | (c3 & 0x3FU);
					   u16str.push_back((char16_t)codePoint);
		}
			break;
		case 0xD0: // 2 �ֽ��ַ�, 0x80 �� 0x7FF
		case 0xC0:
		{
					 uint32_t c2 = p[++i];
					 // ����UNICODE�����ֵ(��һ���ֽ�ȡ��5bit������ȡ6bit)
					 uint32_t codePoint = ((ch & 0x1FU) << 12) | ((c2 & 0x3FU) << 6);
					 u16str.push_back((char16_t)codePoint);
		}
			break;
		default:	// ���ֽڲ���(ǰ���Ѿ��������Բ�Ӧ�ý���)
			is_ok = false;
			break;
		}
	}
	if (ok != NULL) { *ok = is_ok; }

	return u16str;
}
/*
string octet2String(const char * src_in, int size)
{
	if (src_in[0] == 0x04)
	{
		if ((int)src_in[1] <128)
		{
			return string(src_in + 2, src_in[1]);
		}
		else
		{
			int count_len = (int)src_in[2] - 0x80;
			int content_len = 0;
			for (int i = 0; i<count_len; i++)
			{
				count_len =(count_len << 8) + (int)src_in[2 + i];
			}

			return (content_len > size ? "" : string(src_in + 3, content_len));
		}
	}
	return "";
}

string string2octet(const char* src_in, int size)
{
	
	return "";
	}*/