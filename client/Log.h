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
	printf("��ӭ�������̵���ҳ�������������ִ�ж�Ӧ��ָ�\n1.��½\n2.ע��\n3.�˳�\n");
	scanf("%d", &option);
	fflush(stdin);
	if (option == 1){
		printf("���ǵ�½,��������Ҫ��½���˺���:\n");
		scanf("%s", name);
		printf("��������Ҫ��½���˺�����:\n");
		scanf("%s", password);
		fflush(stdin);
		memset(SendMessage, 0, sizeof(SendMessage));
		MessageForm(SendMessage,'L',name,password);
		send(soc, SendMessage, 1025, 0);
		recv(soc, RecvMessage, 64, 0);
		if (RecvMessage[0] == '1'){                          //��½�ɹ�
			printf("��½�ɹ���\n");
			*mode = LOGSUCCESSED;
			memset(path, 0, 512);
			strcat(path, name);
		}
		else if (RecvMessage[0] == '0'){                     //��½ʧ��
			printf("��½ʧ�ܣ�\n");
		}
	}
	else if (option == 2){
		printf("����ע��,��������Ҫע����˺���:\n");
		scanf("%s", name);
		printf("��������Ҫע����˺�����:\n");
		scanf("%s", password);
		fflush(stdin);
		memset(SendMessage, 0, sizeof(SendMessage));
		MessageForm(SendMessage, 'R', name, password);
		send(soc, SendMessage, 1025, 0);
		recv(soc, RecvMessage, 1, 0);
		if (RecvMessage[0] == '1'){                          //ע��ɹ�
			*mode = LOGSUCCESSED;
			memset(path, 0, 512);
			strcat(path, name);
		}
		else if (RecvMessage[0] == '0'){                     //ע��ʧ��
			printf("ע��ʧ�ܣ�\n");
		}
	}
	else if (option == 3){
		*mode = EXITDISK;
	}
	else{
		printf("������󣬰������λ��ص���ҳ���¼���ָ��\n");
		getchar();
	}
}