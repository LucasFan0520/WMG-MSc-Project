// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char tag[100]; char msg[1000]; } M;
M arr[1000]; int n=0;
int find(char *t) { for(int i=0;i<n;i++) if(!strcmp(arr[i].tag,t)) return i; return -1; }

int main() {
    char cmd[20], t1[100], t2[100], t3[100], msg[1000];
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"ADD")) {
            scanf("%s", t1); fgets(msg,1000,stdin); msg[strcspn(msg,"\n")]=0;
            if(msg[0]==' ') memmove(msg,msg+1,strlen(msg));
            strcpy(arr[n].tag,t1); strcpy(arr[n].msg,msg); n++;
        } else if(!strcmp(cmd,"CONCAT")) {
            scanf("%s %s %s", t1, t2, t3);
            int i1=find(t1), i2=find(t2);
            if(i1!=-1 && i2!=-1) {
                strcpy(arr[n].tag,t3);
                sprintf(arr[n].msg,"%s %s",arr[i1].msg,arr[i2].msg);
                n++;
            }
        } else if(!strcmp(cmd,"REMOVE")) {
            scanf("%s", t1);
            for(int i=0;i<n;) { if(!strcmp(arr[i].tag,t1)) { for(int j=i;j<n-1;j++) arr[j]=arr[j+1]; n--; } else i++; }
        } else if(!strcmp(cmd,"FIND")) {
            scanf("%s", t1); int i=find(t1);
            if(i!=-1) { for(char *p=arr[i].msg;*p;p++) printf("%c",*p==' '?'_':*p); printf("\n"); }
        } else if(!strcmp(cmd,"REPORT")) {
            for(int i=0;i<n;i++) printf("%s %s\n",arr[i].tag,arr[i].msg);
        }
    }
    return 0;
}