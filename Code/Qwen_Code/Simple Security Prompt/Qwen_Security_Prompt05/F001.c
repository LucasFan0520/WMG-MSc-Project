// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id, *text; } P;
P *ps = NULL; int np = 0;

int find(char *id) { for(int i=0;i<np;i++) if(!strcmp(ps[i].id,id)) return i; return -1; }

int main() {
    char ln[1024];
    while(fgets(ln,1024,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20], id[256], *txt;
        sscanf(ln,"%s %s",cmd,id);
        txt = strchr(ln,' '); if(txt) { txt = strchr(txt+1,' '); if(txt) txt++; else txt=""; } else txt="";
        if(!strcmp(cmd,"NEW")) { ps=realloc(ps,(np+1)*sizeof(P)); ps[np].id=strdup(id); ps[np].text=strdup(txt); np++; }
        else if(!strcmp(cmd,"APPEND")) { int i=find(id); if(i>=0) { char *n=malloc(strlen(ps[i].text)+strlen(txt)+2); sprintf(n,"%s %s",ps[i].text,txt); free(ps[i].text); ps[i].text=n; } }
        else if(!strcmp(cmd,"REPLACE")) { int i=find(id); if(i>=0) { free(ps[i].text); ps[i].text=strdup(txt); } }
        else if(!strcmp(cmd,"DELETE")) { int i=find(id); if(i>=0) { free(ps[i].id); free(ps[i].text); for(int j=i;j<np-1;j++) ps[j]=ps[j+1]; np--; } }
        else if(!strcmp(cmd,"SHOW")) { int i=find(id); if(i>=0) { for(char *p=ps[i].text;*p;p++) putchar(*p==' '?'_':*p); putchar('\n'); } else printf("NOT_FOUND\n"); }
        else if(!strcmp(cmd,"LIST")) { for(int i=0;i<np;i++) printf("%s\n",ps[i].id); }
    }
    for(int i=0;i<np;i++) { free(ps[i].id); free(ps[i].text); } free(ps);
    return 0;
}