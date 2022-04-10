#include<func.h>
#include"LogandReg.h"
#include"TransForm.h"
#include"Operate.h"
#include"Load.h"


int CreatMySocket(char *port){
    int my_soc=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in my_addr;
    memset(&my_addr,0,sizeof(struct sockaddr_in));
    my_addr.sin_family=AF_INET;
    my_addr.sin_port =htons(atoi(port));
    my_addr.sin_addr.s_addr=inet_addr("***.***.***.***");
    if(bind(my_soc,(struct sockaddr*)&my_addr,sizeof(struct sockaddr))==0){
        printf("bind successed\n");
    }
    if(listen(my_soc,1024)==0){
        printf("listen successed\n");
    }
    return my_soc;
}


int main(int argc,char *argv[]){
    
    int MySocket=CreatMySocket(argv[1]);                                  //创建本服务器的监听socket
    int NewEpoll=epoll_create(1);                                         //创建第一个Epoll池子，用于Accept新的连接
    struct epoll_event NewSocketEvent;
    NewSocketEvent.events=EPOLLIN;
    NewSocketEvent.data.fd=MySocket;
    epoll_ctl(NewEpoll,EPOLL_CTL_ADD,MySocket,&NewSocketEvent);      
    struct epoll_event NewSocketEventBuf[128];



    //为Accpet新的连接做准备
    int AccpetedSoc[1024];
    int NewAccpetedSocSize=0;     
    int AccpetedSocSize=0;        
    int AccpetedSocBuf;           
    struct sockaddr_in AccpetedSocAddr[1024]={0};                      //存储用户机的地址信息，为后续accept做准备
    int AccpetedSocAddrLen=sizeof(struct sockaddr);                    //accept参数

    int ActiveUsrEpoll=epoll_create(1024);                             //创建第二个Epoll池子，用于管理已经接受的Socket
    struct epoll_event ActiveUsrCtlEvent[1024];                           
    struct epoll_event ActiveUsrRecieveEvent[1024];
    int ActiveSocSize;
    
    char UsrImformation[1025];
    char name[512];
    char password[512];
    char newname[512];
    char systemorder[1024];
    char size[512];
    int SocBuf;
    int Imforlen;
    char path[128];



    int ret;
    pusr_t Hash[1024]={0};
    int DataBase=open("DataBase.txt",O_RDWR);
    ReadTxt(DataBase,Hash);
    int sendbuf;


    while(1){
        NewAccpetedSocSize= epoll_wait(NewEpoll,NewSocketEventBuf,128,500);
/*
        监听到许多新的Accpeted时，需要做到：
        1.为每个新监听到的连接分配一个UsrAddr地址
        2.将soc加入到连接用户池
        3.将soc注册到epoll监听池
*/
        for(int i=0;i<NewAccpetedSocSize;++i){
            AccpetedSocBuf=accept(MySocket,(struct sockaddr *)&AccpetedSocAddr[AccpetedSocSize],&AccpetedSocAddrLen);
            AccpetedSoc[AccpetedSocSize]=AccpetedSocBuf;
            ActiveUsrCtlEvent[AccpetedSocSize].events=EPOLLIN;
            ActiveUsrCtlEvent[AccpetedSocSize].data.fd=AccpetedSocBuf;
            epoll_ctl(ActiveUsrEpoll,EPOLL_CTL_ADD,AccpetedSocBuf,&ActiveUsrCtlEvent[AccpetedSocSize]);  
            ++AccpetedSocSize;
            send(AccpetedSocBuf,"hello my bro!\n",15,0);
        }

        ActiveSocSize= epoll_wait(ActiveUsrEpoll,ActiveUsrRecieveEvent,1024,500);
        for(int i = 0;i<ActiveSocSize;++i){
            printf("Now deal Usr No. %d\n",i+1);
            memset(UsrImformation,0,1025);
            SocBuf=ActiveUsrRecieveEvent[i].data.fd;
            Imforlen=recv(SocBuf,UsrImformation,1025,0);
            if(Imforlen==0){//说明这个连接关闭了，可以将其从池子中删去
                epoll_ctl(ActiveUsrEpoll,EPOLL_CTL_DEL,SocBuf,&ActiveUsrCtlEvent[AccpetedSocSize]);
            }        
            else {
                printf("usr send %s \n",UsrImformation);
            }
            if(UsrImformation[0]=='L'){                            //Log,登陆
                Transform_S1_S2(UsrImformation,name,password);  
                Log(SocBuf,name,password,Hash);
            }
            else if(UsrImformation[0]=='R'){                       //Register,注册
                Transform_S1_S2(UsrImformation,name,password);
                Reg(SocBuf,name,password,Hash,DataBase);
            }
            else if(UsrImformation[0]=='G'){                       //Getdir，得到目录信息
                Transform_S1(UsrImformation,path);
                send_dir(SocBuf,path);
            }
            else if(UsrImformation[0]=='M'){                       //用到mv的指令，即重命名和移动文件
                Transform_S1_S2(UsrImformation,path,newname);
                Make_System_Order("mv ",path,newname,systemorder);
                system(systemorder);
            }           
            else if(UsrImformation[0]=='C'){                       //Creatdir,创建目录
                Transform_S1_S2(UsrImformation,path,newname);
                Make_System_Order("mkdir ",path,"",systemorder);
                system(systemorder);
            }
            else if(UsrImformation[0]=='D'){                       //Download,下载
                Transform_S1(UsrImformation,path);
                DownFile(SocBuf,path);
            }
            else if(UsrImformation[0]=='U'){                       //Upload，上传
                Transform_S1_S2(UsrImformation,path,size);
                UpFile(SocBuf,path,size);
            }
            else if(UsrImformation[0]=='S'){                       //Shanchu，删除的拼音
                Transform_S1(UsrImformation,path);
                Make_System_Order("rm -rf ",path,"",systemorder);
                system(systemorder);
            }
        }
       
   

    }
    return 1;
 }


