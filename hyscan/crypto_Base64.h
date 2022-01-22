#ifndef BASE_64_H
#define BASE_64_H
#include "public.h"
#define DEFAULTBASE64CHAR "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
/**
* Base64 ����/����
* @author liruixing
*/
class crypto_Base64{
private:
	std::string _base64_table;
	static const char base64_pad = '='; 
public:
	crypto_Base64()
	{
		_base64_table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; /*����Base64����ʹ�õı�׼��*/
	}
	/**
	* ���������unsigned���ͣ�����������ĵ�ʱ�����
	*/
	std::string Encode(const unsigned char * str, int bytes);
	std::string Decode(const char *str, int bytes);
	void Debug(bool open = true);
};
#endif