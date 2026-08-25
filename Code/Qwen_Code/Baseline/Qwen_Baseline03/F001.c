// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char id[100]; char text[1000]; } P;
P arr[1000]; int n=0;
int find(char *id) { for(int i=0;i<n;i++) if(!strcmp(arr[i].id,id)) return i; return -1; }

int main() {
    char cmd[20], id[100], text[1000];
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"NEW")) {
            scanf("%s", id); fgets(text,1000,stdin); text[strcspn(text,"\n")]=0;
            if(text[0]==' ') memmove(text,text+1,strlen(text));
            int i=find(id);
            if(i!=-1) strcpy(arr[i].text, text);
            else { strcpy(arr[n].id,id); strcpy(arr[n].text,text); n++; }
        } else if(!strcmp(cmd,"APPEND")) {
            scanf("%s", id); fgets(text,1000,stdin); text[strcspn(text,"\n")]=0;
            if(text[0]==' ') memmove(text,text+1,strlen(text));
            int i=find(id); if(i!=-1) { strcat(arr[i].text," "); strcat(arr[i].text,text); }
        } else if(!strcmp(cmd,"REPLACE")) {
            scanf("%s", id); fgets(text,1000,stdin); text[strcspn(text,"\n")]=0;
            if(text[0]==' ') memmove(text,text+1,strlen(text));
            int i=find(id); if(i!=-1) strcpy(arr[i].text,text);
        } else if(!strcmp(cmd,"DELETE")) {
            scanf("%s", id); int i=find(id);
            if(i!=-1) { for(int j=i;j<n-1;j++) arr[j]=arr[j+1]; n--; }
        } else if(!strcmp(cmd,"SHOW")) {
            scanf("%s", id); int i=find(id);
            if(i==-1) printf("NOT_FOUND\n");
            else { for(char *p=arr[i].text;*p;p++) printf("%c",*p==' '?'_':*p); printf("\n"); }
        } else if(!strcmp(cmd,"LIST")) {
            for(int i=0;i<n;i++) printf("%s\n",arr[i].id);
        }
    }
    return 0;
}