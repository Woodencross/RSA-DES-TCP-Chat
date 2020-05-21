#include"des.h"
#include"rsa.h"
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<thread>

#pragma comment(lib,"ws2_32.lib")

//发送公钥
void sendpubkey(SOCKET sClient) {
	char c[16];
	_itoa_s(e, c, 10);
	send(sClient, c, strlen(c), 0);
	_itoa_s(n, c, 10);
	send(sClient, c, strlen(c), 0);
}

//接收加密后密钥并解密
void recvkey(SOCKET sClient) {
	int cypher[8];
	string s;
	cout << "RSA cypher:";
	for (int i = 0; i < 8; i++) {
		char c[64];
		int ret = recv(sClient, c, 64, 0);
		if (ret > 0) {
			c[ret] = '\0';
			cypher[i] = atoi(c);
		}
		cout << cypher[i] << " ";
	}
	cout << endl;
	RSAdecrypt(s, cypher);
	//cout << "random key:" << s << endl;
	key = str2bit(s);
}

//发送数据的子线程
void sendData(SOCKET sClient) {
	string data;
	while (true) {
		getline(cin, data);
		cout << "Server>>>" << data << endl;
		data = encrypt(data);
		//cout << data << endl;
		send(sClient, data.data(), data.length(), 0);
	}
}

//接收数据的父线程
int main() {
	RSAinit();

	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0) {
		cout << "Initialize WSA failed !" << endl;
		return 0;
	}

	//创建套接字
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET) {
		cout << "Create listen socket failed !" << endl;
		return 0;
	}

	//绑定IP和端口
	sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	inet_pton(AF_INET, INADDR_ANY, &sin.sin_addr.s_addr);
	sin.sin_port = htons(1234);
	if (::bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR) {
		cout << "Bind error !" << endl;
	}

	//监听
	if (listen(slisten, 5) == SOCKET_ERROR) {
		cout << "Listen error !" << endl;
		return 0;
	}


	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);

	L1:
	while (true) {
		cout << "Wait for connection..." << endl;
		sClient = accept(slisten, (SOCKADDR*)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET) {
			cout << "Accept error !" << endl;
			continue;
		}
		char addrLen[256];
		cout << "Connection accepted:" << inet_ntop(AF_INET, &remoteAddr.sin_addr, addrLen, sizeof(addrLen)) << endl;
		break;
	}

	//发送公钥
	sendpubkey(sClient);

	//接收加密后密钥并解密
	recvkey(sClient);

	cout << "Now you can type:" << endl;

	//发送线程
	thread t(sendData, sClient);
	t.detach();

	while (true) {
		char revData[2048];
		int ret = recv(sClient, revData, 2048, 0);
		if (ret > 0) {
			revData[ret] = '\0';
			cout << "Client>>>" << decrypt(revData) << endl;
		}
		else {
			cout << "System: Client quit." << endl;
			goto L1;
		}
	}

	closesocket(sClient);
	closesocket(slisten);
	WSACleanup();

	return 0;
}