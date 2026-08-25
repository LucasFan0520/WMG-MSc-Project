// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **lines = NULL; int nl = 0;

int main() {
    char ln[1024];
    while(fgets(ln,1024,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20];
        sscanf(ln,"%s",cmd);
        char *p1 = strchr(ln,' '); if(p1) p1++;
        if(!strcmp(cmd,"INSERT")) { int idx; sscanf(p1,"%d",&idx); char *t=p1; while(*t && *t!=' ') t++; if(*t) t++; if(idx>=0 && idx<=nl) { lines=realloc(lines,(nl+1)*sizeof(char*)); for(int i=nl;i>idx;i--) lines[i]=lines[i-1]; lines[idx]=strdup(t); nl++; } }
        else if(!strcmp(cmd,"APPEND")) { lines=realloc(lines,(nl+1)*sizeof(char*)); lines[nl++]=strdup(p1); }
        else if(!strcmp(cmd,"DELETE")) { int idx; sscanf(p1,"%d",&idx); if(idx>=0 && idx<nl) { free(lines[idx]); for(int i=idx;i<nl-1;i++) lines[i]=lines[i+1]; nl--; } }
        else if(!strcmp(cmd,"PATCH")) { int idx; sscanf(p1,"%d",&idx); char *t=p1; while(*t && *t!=' ') t++; if(*t) t++; if(idx>=0 && idx<nl) { free(lines[idx]); lines[idx]=strdup(t); } }
        else if(!strcmp(cmd,"PRINT")) { for(int i=0;i<nl;i++) { for(char *p=lines[i];*p;p++) putchar(*p==' '?'_':*p); putchar('\n'); } }
    }
    for(int i=0;i<nl;i++) free(lines[i]); free(lines);
    return 0;
}