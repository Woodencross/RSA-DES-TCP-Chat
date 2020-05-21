#include"des.h"
#include"rsa.h"
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<thread>
#include<Windows.h>

#pragma comment(lib,"ws2_32.lib")

//�������8�ֽ���Կ
string randomkey() {
	string s;
	srand((int)time(NULL));
	for (int i = 0; i < 8; i++) {
		char c = random(2) == 0 ? (random(26) + 65) : (random(26) + 97);
		s += c;
	}
	key = str2bit(s);
	cout << "random key:" << s << endl;
	return s;
}

//���չ�Կ
void recvpubkey(SOCKET sClient) {
	char recvkey[256];
	int ret = recv(sClient, recvkey, 256, 0);
	if (ret > 0) {
		recvkey[ret] = '\0';
		e = atoi(recvkey);
		cout << "receive e=" << e << endl;
	}
	memset(&recvkey, 0, 256);
	ret = recv(sClient, recvkey, 256, 0);
	if (ret > 0) {
		recvkey[ret] = '\0';
		n = atoi(recvkey);
		cout << "receive n=" << n << endl;
	}
}

//������Կ������
void sendkey(SOCKET sClient) {
	string rankey = randomkey();
	int cypher[8];
	RSAencrypt(rankey, cypher);
	for (int i = 0; i < 8; i++) {
		char c[64];
		_itoa_s(cypher[i], c, 10);
		send(sClient, c, strlen(c), 0);
		Sleep(10);//��Ҫ
	}
	cout << endl;
}

//�������߳�
void sendData(SOCKET sClient) {
	string data;
	while (true) {
		getline(cin, data);
		cout << "Client>>>" << data << endl;
		data = encrypt(data);
		send(sClient, data.data(), data.length(), 0);
	}
}

//���ո��߳�
int main() {
	srand((int)time(NULL));
	
	//��ʼ��
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0) {
		cout << "Initialize WSA failed !" << endl;
		return 0;
	}

	//
	
	SOCKET sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sClient == INVALID_SOCKET) {
		cout << "Create socket failed !" << endl;
		return 0;
	}

	sockaddr_in serAddr;
	memset(&serAddr, 0, sizeof(serAddr));
	serAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &serAddr.sin_addr.s_addr);
	serAddr.sin_port = htons(1234);
	if (connect(sClient, (sockaddr*)&serAddr, sizeof(serAddr)) == SOCKET_ERROR) {
		cout << "Connect error !" << endl;
		closesocket(sClient);
		return 0;
	}

	//���չ�Կ
	recvpubkey(sClient);

	//������Կ������
	sendkey(sClient);

	cout << "Now you can type:" << endl;

	thread t(sendData, sClient);
	t.detach();

	while (true) {
		char revData[2048];
		int ret = recv(sClient, revData, 2048, 0);
		if (ret > 0) {
			revData[ret] = '\0';
			cout << "Server>>>" << decrypt(revData) << endl;
		}
		else {
			cout << "System: Server closed." << endl;
			break;
		}
	}

	closesocket(sClient);
	WSACleanup();
	system("pause");
	return 0;
}