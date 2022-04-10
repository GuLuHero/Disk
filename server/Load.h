#include<func.h>
typedef struct down_node{
    int soc;
    char path[512];
}down_node_t,*pdown_node_t;

typedef struct up_node{
    int soc;
    char path[512];
    char size[128];
}up_node_t,*pup_node_t;


void *Order_down_pth(void *node1){
    pdown_node_t node=(pdown_node_t) node1;
    printf("path = %s \n",node->path);
    struct stat fdbuf;
    int fd=stat(node->path,&fdbuf);
    FILE *fp=fopen(node->path,"r+");
    char sendbuf[128];
    char buf[8192];
    long int sendsize=0;
    long int filesize=fdbuf.st_size;
    memset(sendbuf,0,128);
    memset(sendbuf,0,8192);
    sprintf(sendbuf,"%ld",fdbuf.st_size);
    send(node->soc,sendbuf,strlen(sendbuf),0);
    int len=1;
    while(len!=0){
        memset(buf,0,8192);
        len=fread(buf,sizeof(char),8192,fp);
        sendsize+=send(node->soc,buf,len,0);
    }
    
    fclose(fp);
    close(fd);
}
void *DownFile(int soc,char *path){
    printf("path = %s \n",path);
    down_node_t th_node;
    th_node.soc=soc;
    memset(th_node.path,0,512);
    strcat(th_node.path,path);
    pthread_t Down_thdId;
    if(pthread_create(&Down_thdId,NULL,Order_down_pth,(void *)(&th_node))==0){
        printf("ph creat ok\n");
    }
}
void *Order_up_pth(void *node1){
    int up_soc=socket(AF_INET,SOCK_STREAM,0);
    int port=10000;
    struct sockaddr_in my_addr;
    memset(&my_addr,0,sizeof(struct sockaddr_in));
    my_addr.sin_family=AF_INET;
    my_addr.sin_port =htons(port);
    my_addr.sin_addr.s_addr=inet_addr("172.18.37.85");
    while(bind(up_soc,(struct sockaddr*)&my_addr,sizeof(struct sockaddr))!=0){
        ++port;
        my_addr.sin_port =htons(port);
    }
    printf("pth bind successed\n");
    if(listen(up_soc,1024)==0){
        printf("pth listen successed\n");
    }
    pup_node_t p = node1;
    char sendmessage[10];
    sprintf(sendmessage,"%d",port);
    sendmessage[5]='\0';
    send(p->soc,sendmessage,6,0);

    struct sockaddr_in SocAddr;                                                   //存储用户机的地址信息，为后续accept做准备
    int SocAddrLen=sizeof(struct sockaddr);                                       //accept参数
    int SendSoc=accept(up_soc,(struct sockaddr *)&SocAddr,&SocAddrLen);
    send(SendSoc,"hello bro",10,0);
    char recvbuf[8192];

    FILE *fp = fopen(p->path, "wb");
	int len;
	long int FileSize = atol(p->size);
	printf("filesize = %ld ", FileSize);
	long int RecvSize = 0;
    char Buffer[8192];
    memset(Buffer,0,8192);
	while (RecvSize<FileSize){
		len = recv(SendSoc, Buffer, 8192, 0);
        if(len==-1){
            break;
        }
		fwrite(Buffer, sizeof(char), len, fp);
		memset(Buffer, 0, 8192);
		RecvSize += len;
	}
	fclose(fp);
	printf("hello my bon\n");
}
void UpFile(int Soc,char *path,char *size){
    up_node_t p;
    strcpy(p.size,size);
    strcpy(p.path,path);
    p.soc=Soc;
    pthread_t Down_thdId;
    if(pthread_create(&Down_thdId,NULL,Order_up_pth,(void *)(&p))==0){
        printf(" ok\n");
    }
}