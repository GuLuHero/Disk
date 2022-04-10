#include <stdio.h>
#include <WinSock2.h>
#include <string.h>
#pragma comment(lib , "ws2_32.lib")
#define LOGORROG      100  
#define LOGSUCCESSED  101
#define DOWNLOAD      102
#define UPLOAD        103
#define EXITDISK      104

void MessageForm(char *message, char c, char *name, char *password){ 
	message[0] = c;
	int messagepos = 0;
	int i;
	for (i = 0, messagepos = 1; name[i] != '\0'; ++i, ++messagepos){
		message[messagepos] = name[i];
	}
	message[messagepos] = '\0';
	for (i = 0, messagepos = 513; name[i] != '\0'; ++i, ++messagepos){
		message[messagepos] = password[i];
	}
	message[messagepos] = '\0';
}

int LogOrReg(SOCKET soc, char *path, int *mode){
	fflush(stdin);
	char name[64];
	char password[64];
	char SendMessage[1025];
	char RecvMessage[64];
	int MessageLen;
	int option;
	printf("欢迎来到凌盘的首页，请键入数字以执行对应的指令：\n1.登陆\n2.注册\n3.退出\n");
	scanf("%d", &option);
	fflush(stdin);
	if (option == 1){
		printf("这是登陆,请输入您要登陆的账号名:\n");
		scanf("%s", name);
		printf("请输入您要登陆的账号密码:\n");
		scanf("%s", password);
		fflush(stdin);
		memset(SendMessage, 0, sizeof(SendMessage));
		MessageForm(SendMessage,'L',name,password);
		send(soc, SendMessage, 1025, 0);
		recv(soc, RecvMessage, 64, 0);
		if (RecvMessage[0] == '1'){                          //登陆成功
			printf("登陆成功！\n");
			*mode = LOGSUCCESSED;
			memset(path, 0, 512);
			strcat(path, name);
		}
		else if (RecvMessage[0] == '0'){                     //登陆失败
			printf("登陆失败！\n");
		}
	}
	else if (option == 2){
		printf("这是注册,请输入您要注册的账号名:\n");
		scanf("%s", name);
		printf("请输入您要注册的账号密码:\n");
		scanf("%s", password);
		fflush(stdin);
		memset(SendMessage, 0, sizeof(SendMessage));
		MessageForm(SendMessage, 'R', name, password);
		send(soc, SendMessage, 1025, 0);
		recv(soc, RecvMessage, 1, 0);
		if (RecvMessage[0] == '1'){                          //注册成功
			*mode = LOGSUCCESSED;
			memset(path, 0, 512);
			strcat(path, name);
		}
		else if (RecvMessage[0] == '0'){                     //注册失败
			printf("注册失败！\n");
		}
	}
	else if (option == 3){
		*mode = EXITDISK;
	}
	else{
		printf("键入错误，按任意键位后回到首页重新键入指令\n");
		getchar();
	}
}