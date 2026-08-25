// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char title[100]; char body[1000]; } D;
D drafts[1000]; int n=0;
D discarded[1000]; int dn=0;
int find(char *t) { for(int i=0;i<n;i++) if(!strcmp(drafts[i].title,t)) return i; return -1; }

int main() {
    char cmd[20], title[100], body[1000];
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"CREATE")) {
            scanf("%s", title); fgets(body,1000,stdin); body[strcspn(body,"\n")]=0;
            if(body[0]==' ') memmove(body,body+1,strlen(body));
            strcpy(drafts[n].title,title); strcpy(drafts[n].body,body); n++;
        } else if(!strcmp(cmd,"REVISE")) {
            scanf("%s", title); fgets(body,1000,stdin); body[strcspn(body,"\n")]=0;
            if(body[0]==' ') memmove(body,body+1,strlen(body));
            int i=find(title); if(i!=-1) strcpy(drafts[i].body,body);
        } else if(!strcmp(cmd,"DISCARD")) {
            scanf("%s", title); int i=find(title);
            if(i!=-1) {
                strcpy(discarded[dn].title,drafts[i].title);
                strcpy(discarded[dn].body,drafts[i].body); dn++;
                for(int j=i;j<n-1;j++) drafts[j]=drafts[j+1]; n--;
            }
        } else if(!strcmp(cmd,"RESTORE")) {
            if(dn>0) {
                dn--;
                strcpy(drafts[n].title,discarded[dn].title);
                strcpy(drafts[n].body,discarded[dn].body); n++;
            }
        } else if(!strcmp(cmd,"FIND")) {
            scanf("%s", title); int i=find(title);
            if(i!=-1) printf("%s %s\n",drafts[i].title,drafts[i].body);
        } else if(!strcmp(cmd,"PRINT")) {
            for(int i=0;i<n;i++) printf("%s %s\n",drafts[i].title,drafts[i].body);
        }
    }
    return 0;
}