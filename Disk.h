#include <stdio.h>
#include <WinSock2.h>
#include <string.h>
#include <windows.h>
#pragma comment(lib , "ws2_32.lib")
#define PORT2 8815
#define  RED      FOREGROUND_INTENSITY|FOREGROUND_RED
#define  WHITE    FOREGROUND_INTENSITY|FOREGROUND_RED   |  FOREGROUND_GREEN  | FOREGROUND_BLUE
#define  BLUE     FOREGROUND_INTENSITY|FOREGROUND_BLUE
#define  GREEN    FOREGROUND_INTENSITY|FOREGROUND_GREEN
#define  YELLOW   FOREGROUND_INTENSITY|FOREGROUND_RED  |  FOREGROUND_GREEN
//SetConsoleTextAttribute(handle, WHITE);//恢复默认的白色
typedef struct dir{
	char name[128];
	int type;
	struct dir *next;
}dir_t, *pdir_t;
int MakeMyDir(pdir_t head, char *message, int flag){
	pdir_t p = malloc(sizeof(dir_t));
	strcpy(p->name, message);
	p->type = flag;
	p->next = head->next;
	head->next = p;
}
int SeekFile(pdir_t head,char *name){
	pdir_t p = head->next;
	while (p){
		if (strcmp(p->name, name) == 0){
			return p->type;
		}
		else{
			p = p->next;
		}
	}
	return 0;
}

void help_order(){
	printf("查看当前目录文件：ls\n");
	printf("打开目录：open+目录名，如open dir1 \n");
	printf("返回上级目录：reback \n");
	printf("创建新的文件夹：mkdir+要创建的文件夹名字\n");
	printf("重命名文件或文件夹：rename+要改变的文件或文件夹名字+新名字\n");
	printf("移动文件或文件夹到下级目录：mv+目标文件+下级目录名\n");
	printf("移动文件或文件夹到上级目录：mvback+目标文件名\n");
	printf("删除文件或文件夹：del+待删除文件或文件夹名\n");
	printf("下载文件：down+文件名\n");
	printf("上传文件：将要上传的文件移动到网盘所在文件夹后输入 up+文件名\n");
}
void ls_order(pdir_t head, HANDLE handle){
	pdir_t p = head->next;
	while (p){
		if (p->type == 4){
			SetConsoleTextAttribute(handle, RED);        //设置为红色
			printf("%s ", p->name);
			SetConsoleTextAttribute(handle, WHITE);
			p = p->next;
		}
		else if (p->type == 8){
			SetConsoleTextAttribute(handle, BLUE);
			printf("%s ", p->name);
			SetConsoleTextAttribute(handle, WHITE);
			p = p->next;
		}
	}
	printf("\n");
}
void make_send_message(SOCKET soc,char *s, char *s1, char *s2){
	char sendmessage[1025];
	sendmessage[0] = s[0];
	int pos;
	int i;
	for (pos = 1, i = 0; s1[i] != '\0'; ++i, ++pos){
		sendmessage[pos] = s1[i];
	}
	sendmessage[pos] = '\0';
	for (pos = 513, i = 0; s2[i] != '\0'; ++i, ++pos){
		sendmessage[pos] = s2[i];
	}
	sendmessage[pos] = '\0';
	send(soc, sendmessage, 1025, 0);
}
void get_dir(SOCKET soc, char *path, pdir_t head){
	head->next = NULL;
	char RecvMessage[128];
	char SendMessage[128];
	SendMessage[0] = 'G';
	SendMessage[1] = '\0';
	strcat(SendMessage, path);
	send(soc, SendMessage, 128, 0);
	while (1){
		memset(RecvMessage, 0, 128);
		recv(soc, RecvMessage, 128, 0);
		if (strcmp(RecvMessage, "13141314.cjavaphthongolang123") == 0){
			break;
		}
		if (RecvMessage[127] == '4'){
			MakeMyDir(head, RecvMessage, 4);
		}
		else if (RecvMessage[127] == '8'){
			MakeMyDir(head, RecvMessage, 8);
		}
		Sleep(100);
	}

}
void open_order(SOCKET soc, char *path, pdir_t head, char *dirname, HANDLE handle){
	 pdir_t q = head->next;
	 while (q){
		 if (strcmp(q->name, dirname) == 0 && q->type == 4){
			 strcat(path, "/");
			 strcat(path, dirname);
			 get_dir(soc, path, head);
		  	 break;
		 }
		 q = q->next;
	 }
	if (q == NULL){
		printf("没有该文件夹！\n");
	}
}
int reback_order(char *path){
	int len = strlen(path);
	int pos = len - 1;
	while (pos != 0){
		if (path[pos] == '/'){
			path[pos] = '\0';
			return 1;
		}
		else{
			--pos;
		}
	}
	printf("已经是用户根目录！\n");
	return 0;
}
void mv_order(SOCKET soc, char *path, char *filename, char *dirname){
	char SourcePath[512];
	char GoalPath[512];
	memset(SourcePath, '\0', 512);
	memset(GoalPath, '\0', 512);
	printf("path = %s ,filename = %s dirname = %s", path, filename, dirname);
	strcpy(SourcePath, path);
	strcat(SourcePath, "/");
	strcat(SourcePath, filename);
	strcpy(GoalPath, path);
	strcat(GoalPath, "/");
	strcat(GoalPath, dirname);
	make_send_message(soc, "M", SourcePath, GoalPath);
}
void mkdir_order(SOCKET soc, char *path,char *newdirname){
	char GoalPath[512];
	memset(GoalPath, '\0', 512);
	strcpy(GoalPath, path);
	strcat(GoalPath, "/");
	strcat(GoalPath, newdirname);
	make_send_message(soc, "C", GoalPath, "");
}
void del_order(SOCKET soc, char *path, char *filename){
	char Mseeage[512];
	memset(Mseeage, '\0', 512);

	strcpy(Mseeage, path);
	strcat(Mseeage, "/");
	strcat(Mseeage, filename);
	make_send_message(soc, "S", Mseeage, "");
}
int mvback_order(SOCKET soc, char *path, char *filename){
	int len = strlen(path);
	int pos = len - 1;
	while (pos != 0){
		if (path[pos] == '/'){
			mv_order(soc, path,filename, "..");
			return 1;
		}
		else{
			--pos;
		}
	}
	printf("已经是最高级目录，移动失败\n");
}
void DownFile(SOCKET soc, char *FileName){
	char Buffer[8192];
	memset(Buffer, 0, 8192);
	FILE *fp = fopen(FileName, "wb");
	int len;
	recv(soc, Buffer, 128, 0);
	long int FileSize = atol(Buffer);
	printf("filesize = %ld ", FileSize);
	long int RecvSize = 0;
	while (RecvSize<FileSize){
		len = recv(soc, Buffer, 8192, 0);
		fwrite(Buffer, sizeof(char), len, fp);
		memset(Buffer, 0, 8192);
		RecvSize += len;
		printf("已下载 %ld/%ld\n", RecvSize, FileSize);
	}
	fclose(fp);
	printf("hello my bon\n");

}
void down_order(SOCKET soc, char *path,char *FileName){
	char sendbuf[512];
	memset(sendbuf, 0, 512);
	strcpy(sendbuf, path);
	strcat(sendbuf, "/");
	strcat(sendbuf, FileName);
	make_send_message(soc, "D", sendbuf, "");
	DownFile(soc, FileName);
}
void UpFile(SOCKET soc, FILE *fp,long size){
	char UpPort[6];
	recv(soc, UpPort, 6, 0);
	printf("server send port = %s \n", UpPort);
	
	SOCKET upsoc;
	struct sockaddr_in server;
	struct hostent *host = NULL;
	upsoc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (upsoc == INVALID_SOCKET) {
		printf("socket() 失败: %d\n", WSAGetLastError());
		return 1;
	}
	
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(UpPort));
	server.sin_addr.s_addr = inet_addr("***.***.***.***");
	
	
	if (connect(upsoc, (struct sockaddr*)&server,
		sizeof(server)) == SOCKET_ERROR) {
		printf("connect() 失败: %d\n", WSAGetLastError());
		return 1;
	}
	char sendbuf[128];
	recv(upsoc, sendbuf, 20, 0);
	printf("sever send %s \n", sendbuf);
	
	long int len = -1;


	char buf[8192];
	long int sendsize = 0;


	while (len != 0){
		memset(buf, 0, 8192);
		len = fread(buf, sizeof(char), 8192, fp);
		sendsize += send(upsoc, buf, len, 0);
		printf("send %ld/%ld\n", sendsize, size);
	}






}
void up_order(SOCKET soc, char *path, FILE *fp){

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);
	char char_size[512];
	sprintf(char_size, "%ld", size);
	printf("size = %ld size_char = %s \n", size, char_size);
	make_send_message(soc, "U", path, char_size);
	fseek(fp, 0, SEEK_SET);
	UpFile(soc, fp,size);
	printf("path = %s \n", path);
	reback_order(path);
	printf("path = %s \n", path);
	fclose(fp);
}
void DiskOperate(SOCKET soc, char *path, char *name){
	char order[512];
	char dirname[512];
	char filename[512];
	pdir_t head = malloc(sizeof(dir_t));
	head->next = NULL;
	HANDLE handle;
	get_dir(soc, path, head);
	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, GREEN);
	printf("当前工作目录为: /%s   \n", path);
	SetConsoleTextAttribute(handle, WHITE);
	fflush(stdin);
	ls_order(head, handle);
	scanf("%s", order);
	if (strcmp(order, "help") == 0){
		fflush(stdin);
		SetConsoleTextAttribute(handle, YELLOW);
		help_order();
		SetConsoleTextAttribute(handle, WHITE);
	}

	else if (strcmp(order, "ls") == 0){
		fflush(stdin);
	}
	else if (strcmp(order, "open") == 0){
		scanf("%s", dirname);
		fflush(stdin);
		open_order(soc, path, head, dirname, handle);
		
	}
	else if (strcmp(order, "reback") == 0){
		fflush(stdin);
		reback_order(path);
	}
	
	else if (strcmp(order, "mv") == 0){
		memset(dirname, 0, sizeof(dirname));
		scanf("%s %s", filename,dirname);
		if (SeekFile(head, filename) == 8 && SeekFile(head, dirname) == 4){
			mv_order(soc, path, filename, dirname);
		}
		else{
			printf("此目录下没有文件或路径\n");
		}
	}
	else if (strcmp(order, "mvback") == 0){
		memset(filename, 0, sizeof(filename));
		scanf("%s", filename);
		fflush(stdin);
		if (SeekFile(head, filename) == 8 ){
			mvback_order(soc, path, filename);
		}
		else{
			printf("此目录下没有该文件\n");
		}
	}
	else if (strcmp(order, "rename") == 0){
		memset(dirname, 0, sizeof(dirname));
		memset(filename, 0, sizeof(filename));
		scanf("%s %s", filename, dirname);
		fflush(stdin);
		if (SeekFile(head, filename) == 8 && SeekFile(head, dirname) == 0){
			mv_order(soc, path, filename, dirname);
		}
		else{
			printf("改名失败\n");
		}
	}
	else if (strcmp(order, "mkdir") == 0){
		scanf("%s", dirname);
		fflush(stdin);
		if (SeekFile(head, dirname) == 0){
			mkdir_order(soc, path,  dirname);
		}
		else{
			printf("创建文件夹失败，已存在同名文件或文件夹\n");
		}
	}
	else if (strcmp(order, "del") == 0){
		scanf("%s", filename);
		fflush(stdin);
		if (SeekFile(head, filename) == 8 || SeekFile(head, filename) == 4){
			del_order(soc, path,filename);
		}
		else{
			printf("此目录下无该文件或文件夹，删除失败\n");
		}
	}
	else if (strcmp(order, "down") == 0){

		scanf("%s", filename);
		fflush(stdin);
		if (SeekFile(head, filename) == 8){
			down_order(soc, path, filename);
		}
		else{
			printf("此目录下无该文件，下载失败\n");
		}
	}
	else if (strcmp(order, "up") == 0){
		scanf("%s", filename);
		fflush(stdin);
		FILE *fp = fopen(filename, "rb+");
		if (fp != NULL){
			
			strcat(path, "/");
			strcat(path, filename);
			up_order(soc, path, fp);
			
		}
		else{
			printf("没有该文件！\n");
		}
	}
	
	else if (strcmp(order, "ls") != 0){
		printf("命令格式错误，输入help以获取教程\n");
		fflush(stdin);
	}
	
	
	
}