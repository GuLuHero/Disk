void send_dir(int soc,char *path){
    DIR *dir=opendir(path);
    struct dirent *pdir;
    int len;
    char SendMessage[128];
    while(pdir=readdir(dir)){
        if(strcmp(pdir->d_name,".")==0||strcmp(pdir->d_name,"..")==0){
            continue;
        }
        memset(SendMessage,0,128);
        strcpy(SendMessage,pdir->d_name);
        if(pdir->d_type==4){
            SendMessage[127]='4';
        }
        else if(pdir->d_type==8){
            SendMessage[127]='8';
        }
        len = send(soc,SendMessage,128,0);      
        printf("i have sent %s len = %d\n",SendMessage,len);
    }
        send(soc,"13141314.cjavaphthongolang123",32,0);
}
void Make_System_Order(char *order,char *SourcePath,char *GoalPath,char *system_order){
    memset(system_order,0,sizeof(system_order));
    system_order[0]='\0';
    strcat(system_order,order);
    strcat(system_order,SourcePath);
    strcat(system_order," ");
    strcat(system_order,GoalPath);
}