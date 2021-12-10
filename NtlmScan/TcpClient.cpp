#include "TcpClient.h"


TcpClient::TcpClient()
{
	socketTimeOut = 3000;
}


TcpClient::~TcpClient()
{
}

/*
��ʼ��winSock
*/
int TcpClient::initWinSock()
{
	WORD verision = MAKEWORD(2, 2);
	WSADATA lpData;
	int iFlag = WSAStartup(verision, &lpData); // ָ��winsock�汾����ʼ��
	if (iFlag != 0)
	{
#ifdef DEBUG
		std::cout << "[-] winSock init fail" << std::endl;
#else
		/*null*/
#endif
		return -1;
	}
#ifdef DEBUG
	std::cout << "[+] winSock init success" << std::endl;
#else
	/*null*/
#endif
	return 0;
}

/*
��������socket  
*/
int TcpClient::createSocket(SOCKET &clientSocket)
{
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&socketTimeOut, sizeof(int));
	if (clientSocket == INVALID_SOCKET)
	{
#ifdef DEBUG
		cout << "[-] socket create fail" << endl;
#else
		/*null*/
#endif
		return -1;
	}

#ifdef DEBUG
	cout << "[+] socket create success" << endl;
#else
	/*null*/
#endif
	return 0;
}

/*
���ӷ�����
*/
int TcpClient::connectSocket(SOCKET &conSock, std::string ip, int port)
{
	// ������ַ�ṹ��
	sockaddr_in hostAddr;
	hostAddr.sin_family = AF_INET;
	hostAddr.sin_port = htons(port);//ת���������ֽ���  
	//hostAddr.sin_addr.S_un.S_addr = inet_addr(SERVERIP);//ת���������ֽ���  
	//cout << "net IP:" << hostAddr.sin_addr.S_un.S_addr << endl;  
	/*
	inet_addr()�汾̫�ͣ�������ʹ��inet_pton(Э���壬�ַ���IP��ַ��voidĿ��in_addr*)
	ͷ�ļ���WS2tcpip.h
	*/
	in_addr addr;
	inet_pton(AF_INET, ip.c_str(), (void*)&addr);
	hostAddr.sin_addr = addr;
	// std::cout << "ip(�����ֽ���):" << addr.S_un.S_addr << std::endl;
	cout << "[+] scanning target ip =========================> " << ip.c_str() << endl;

	// ������������������
	int err = connect(conSock, (sockaddr*)&hostAddr, sizeof(sockaddr));
	if (err == INVALID_SOCKET)
	{
#ifdef DEBUG
		cout << "[-] connect server fail" << endl;
#else
		/*null*/
#endif
		return -1;
	}
	return 0;
}

int TcpClient::sendData(SOCKET &clientSock, string &packetData)
{
	int err = send(clientSock, packetData.c_str(), packetData.size(), 0);
	if (err == SOCKET_ERROR)
	{
#ifdef DEBUG
		cout << "[-] send fail" << endl;
#else
		/*null*/
#endif
		return -1;
	}

#ifdef DEBUG
	cout << "[+] send data ====> " << packetData.c_str() << endl;
#else
	/*null*/
#endif
	return 0;
}

int TcpClient::receiveData(SOCKET &clientSock, string &packetData)
{
	static int cnt = 1; // �������ݱ��-��̬
	// ͨ���ѽ������ӵ��׽��֣��������� �趨����1024�ֽ�
	char buf[1024] = {0};
	// flags������ʽ��0�������ݣ�MSG_PEEDϵͳ�����������ݸ��Ƶ����ṩ�Ľ��ջ������ڣ�ϵͳ����������δɾ����MSG_OOB����������ݣ�ͨ���ò���0���ɣ�
	int buflen = recv(clientSock, buf, 1024, 0);
	if (buflen == SOCKET_ERROR)
	{
#ifdef DEBUG
		cout << "[-] recv data fail" << endl;
#else
		/*null*/
#endif
		return -1;
	}
	// һ����������ʾ��������
	packetData = string(buf, 1024);
#ifdef DEBUG
	cout << "[+] recv data ====> " << buf << endl;
#else
	/*null*/
#endif

	return 0;
}