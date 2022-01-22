#ifndef MYSQLSERVICEEXP_H_H
#define MYSQLSERVICEEXP_H_H
#include "m_base_exp.h"

class m_mysql_exp: public m_base_exp
{
public:
	m_mysql_exp(PortService portService);
	~m_mysql_exp();
	void checkServiceIsVuln();
	void checkWeakpass();
	void mysql_init(string& receiveData1);
	void mysql_connect();
	string hex2String(const char* data, size_t length);
};


struct MYSQL_GREET{
	byte protocol_version; // 0x0a ��һ���ֽڱ�ʾЭ��汾��
	char server_version[10]; // �������汾�ţ��ַ�������Null����
	DWORD connection_id; // connection id �������߳�id
	long long auth_plugin_data_part_1; // ��һ����8���ֽڵ���ս����������滹�еڶ�����
	byte filler_1; //  0x00 ���λ0x00
	byte capability_flag_1[2]; // ������Ȩ�ܱ�־����λ2���ֽڣ�
	unsigned long long character_set; // character_set (1) -- default server character-set, only the lower 8-bits Protocol::CharacterSet (optional)
	byte status_flags[2];
	byte capability_flags_2[2];
	byte auth_plugin_data_len[2]; // ��ս������ĳ���
	char reserved[10];
	byte auth_plugin_data_part_2[12]; // ��ս������ĵڶ����֣�ͨ����12�ֽ� ������־00
	string auth_plugin_name; // ��֤��������ƣ�null��β
};

struct MYSQL_AUTH{	
	byte capability_flags[4]; //  capability flags of the client as defined in Protocol::CapabilityFlags
	DWORD max_packet_size; // max size of a command packet that the client wants to send to the server
	byte character_set; // connection's default character set as defined in Protocol::CharacterSet.
	char blank[22];
	char username[10];
	byte o_14 = 0x14;
	char database[10];
	char password[20];
	char auth_plugin_name[22];
};
#endif
