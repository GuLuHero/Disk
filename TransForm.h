#include<func.h>
void Transform_S1_S2(char *message,char *s1,char *s2){
    int message_pos=0;
    int i;
    for(message_pos=1,i=0;message[message_pos]!='\0';++i,++message_pos){
        s1[i]=message[message_pos];
    }
    s1[i]='\0';
    for(message_pos=513,i=0;message[message_pos]!='\0';++i,++message_pos){
        s2[i]=message[message_pos];
    }
    s2[i]='\0';
    printf("s1=%s  s2=%s",s1,s2);
}
void Transform_S1(char *message,char *S1){
    int i;
    for(i=0;message[i+1]!='\0';++i){
        S1[i]=message[i+1];
    }
    S1[i]=message[i+1];
    printf("s1=%s ",S1);
}
