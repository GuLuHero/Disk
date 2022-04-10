#include <func.h>
#define NAME_FLAG        151
#define PASSWORD_FLAG    152


#define LOGSUCCESS       153                   //登陆只有两个结果
#define LOGFILED         154

#define NOUSR            155                   //查询的结果：没有用户，密码错误，密码正确
#define PASSWORDFALSE    156
#define PASSWORDTRUE     157

#define REGSUCCESS       158                   //注册的结果，注册成功,注册失败
#define REGFAILED        159

typedef struct Usr{
    char name[64];
    char password[64];
    struct Usr *next;
}usr_t,*pusr_t;
unsigned int ELFHash(char *str){
    unsigned int hash = 0;
    unsigned int x = 0;
    while (*str){
        hash = (hash << 4) + (*str++);
        if ((x = hash & 0xF0000000L) != 0){
            hash ^= (x >> 24);
            hash &= ~x;
        }
    }
    return (hash & 0x7FFFFFFF);
}

int WriteFd(int fd,char *name,char *password){
    write(fd,name,strlen(name));
    char s1=' ';
    write(fd,&s1,1);
    write(fd,password,strlen(password));
    char s2='\n';
    write(fd,&s2,1);
}
int InsertHash(pusr_t *HashTable,char *name,char *password){
    pusr_t p=malloc(sizeof(usr_t));
    strcpy(p->name,name);
    strcpy(p->password,password);
    p->next=NULL;
    int pos=ELFHash(p->name)%1024;
    if(HashTable[pos]==NULL){
        HashTable[pos]=p;
        return 1;
    }
    else{
        p->next=HashTable[pos];
        HashTable[pos]=p;
    }
    return 1;
}
int InquiryHash (pusr_t *HashTable,char *name,char *password){
    int pos=ELFHash(name)%1024;
    if(HashTable[pos]==NULL){
        return NOUSR;
    }
    else{
        pusr_t q=HashTable[pos];
        while(q){
            if(strcmp(q->name,name)==0&&strcmp(q->password,password)==0){
                return PASSWORDTRUE;
            }
            
            else if(strcmp(q->name,name)==0&&strcmp(q->password,password)!=0){
                return PASSWORDFALSE;
            }
            else{
                q=q->next;
            }
        }
        printf("after %s pos in no Hash\n",name);
        return NOUSR;
    }
}
int ReadTxt(int fd,pusr_t *Hash){
    char c;
    char name[512];
    char password[512];
    memset(password,0,512);
    memset(name,0,512);
    int pos=0;
    int ret=read(fd,&c,1);
    int flag=NAME_FLAG;                                
    while(ret!=0&&c!=EOF){
        if(flag==NAME_FLAG){
            if(c!=' '){
                name[pos]=c;
                ret=read(fd,&c,1);
                ++pos;
            }
            else if(c==' '){
                name[pos]='\0';
                flag=PASSWORD_FLAG;
                pos=0;
                ret=read(fd,&c,1);
                
            }
            else{
                continue;
            }
        }
        else if(flag==PASSWORD_FLAG){
            if(c!='\n'){
                password[pos]=c;
                ret=read(fd,&c,1);
                ++pos;
            }
            else if(c=='\n'){
                password[pos]='\0';
                flag=NAME_FLAG;
                pos=0;
                ret=read(fd,&c,1);
                InsertHash(Hash,name,password);
                memset(password,0,32);
                memset(name,0,32);
            }
            else{
                continue;
            }
        }
    }
}
int RegisterHash(int fd,pusr_t *hash,char *name,char *password){
    int ret=InquiryHash(hash,name,password);
    if(ret==NOUSR){
        InsertHash(hash,name,password);
        WriteFd(fd,name,password);
        mkdir(name,0777);
        return REGSUCCESS;
    }
    else{
        return REGFAILED;
    }
}


int Log(int soc,char *name,char *password,pusr_t *Hash){

    int ret=InquiryHash(Hash,name,password);
    if(ret==PASSWORDTRUE){
        send(soc,"1",1,0);
        return LOGSUCCESS;
    }
    else {
        send(soc,"0",1,0);
        return LOGFILED;
    }
}
int Reg(int soc,char *name,char *password,pusr_t *Hash,int DataBase){   


    int ret=RegisterHash(DataBase,Hash,name,password);
    if(ret==REGSUCCESS){
        send(soc,"1",11,0);
        return REGSUCCESS;
    }
    else{
        send(soc,"0",11,0);
        return REGFAILED;
    }

    
}










