#include <stdio.h>
#include <WinSock2.h>
#include <string.h>
#include "Disk.h"
#include "Log.h"
#define PORT 8816



#pragma comment(lib , "ws2_32.lib")




int main(){
	HANDLE handle;
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), WHITE);
	WSADATA wsd;
	/*����Winsock DLL*/
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		printf("Winsock    ��ʼ��ʧ��!\n");
		return 1;
	}
	
	/*����Socket*/
	SOCKET sClient;
	char Buffer[1024];
	struct sockaddr_in server;
	struct hostent *host = NULL;
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sClient == INVALID_SOCKET) {
		printf("socket() ʧ��: %d\n", WSAGetLastError());
		return 1;
	}
	/*ָ����������ַ*/
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = inet_addr("***.***.***.***");

	/*���������������*/
	if (connect(sClient, (struct sockaddr*)&server,
		sizeof(server)) == SOCKET_ERROR) {
		printf("connect() ʧ��: %d\n", WSAGetLastError());
		return 1;
	}
	recv(sClient, Buffer, 1024, 0);
	printf("%s", Buffer);                                                                         
	int mode = LOGORROG;
	char SendMessage[1025];                                                                       //���͸����������ַ���
	char RecvMessage[512];
	char path[512];
	memset(path, 0, 64);
	int option;
	int MessageLen;
	while (mode!=EXITDISK){
		if (mode == LOGORROG){
			LogOrReg(sClient, path, &mode);
		}

		else if (mode == LOGSUCCESSED){
			DiskOperate(sClient, path, &mode);
		}

		else  {
			return 1;
		}

	}
	return 1;

}