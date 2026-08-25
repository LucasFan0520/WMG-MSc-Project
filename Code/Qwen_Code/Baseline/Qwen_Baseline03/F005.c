// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char lines[1000][1000]; int n=0;

int main() {
    char cmd[20], text[1000]; int idx;
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"INSERT")) {
            scanf("%d",&idx); fgets(text,1000,stdin); text[strcspn(text,"\n")]=0;
            if(text[0]==' ') memmove(text,text+1,strlen(text));
            if(idx>=0 && idx<=n) {
                for(int i=n;i>idx;i--) strcpy(lines[i],lines[i-1]);
                strcpy(lines[idx],text); n++;
            }
        } else if(!strcmp(cmd,"APPEND")) {
            fgets(text,1000,stdin); text[strcspn(text,"\n")]=0;
            if(text[0]==' ') memmove(text,text+1,strlen(text));
            strcpy(lines[n++],text);
        } else if(!strcmp(cmd,"DELETE")) {
            scanf("%d",&idx);
            if(idx>=0 && idx<n) {
                for(int i=idx;i<n-1;i++) strcpy(lines[i],lines[i+1]);
                n--;
            }
        } else if(!strcmp(cmd,"PATCH")) {
            scanf("%d",&idx); fgets(text,1000,stdin); text[strcspn(text,"\n")]=0;
            if(text[0]==' ') memmove(text,text+1,strlen(text));
            if(idx>=0 && idx<n) strcpy(lines[idx],text);
        } else if(!strcmp(cmd,"PRINT")) {
            for(int i=0;i<n;i++) {
                for(char *p=lines[i];*p;p++) printf("%c",*p==' '?'_':*p);
                printf("\n");
            }
        }
    }
    return 0;
}