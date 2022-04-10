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
	/*加载Winsock DLL*/
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		printf("Winsock    初始化失败!\n");
		return 1;
	}
	
	/*创建Socket*/
	SOCKET sClient;
	char Buffer[1024];
	struct sockaddr_in server;
	struct hostent *host = NULL;
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sClient == INVALID_SOCKET) {
		printf("socket() 失败: %d\n", WSAGetLastError());
		return 1;
	}
	/*指定服务器地址*/
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = inet_addr("***.***.***.***");

	/*与服务器建立连接*/
	if (connect(sClient, (struct sockaddr*)&server,
		sizeof(server)) == SOCKET_ERROR) {
		printf("connect() 失败: %d\n", WSAGetLastError());
		return 1;
	}
	recv(sClient, Buffer, 1024, 0);
	printf("%s", Buffer);                                                                         
	int mode = LOGORROG;
	char SendMessage[1025];                                                                       //发送给服务器的字符串
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