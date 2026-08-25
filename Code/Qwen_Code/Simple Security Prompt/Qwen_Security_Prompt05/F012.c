// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *note; } Q;
Q *qs = NULL; int nq = 0;

int findq(char *n) { for(int i=0;i<nq;i++) if(!strcmp(qs[i].name,n)) return i; return -1; }

int main() {
    char ln[1024];
    while(fgets(ln,1024,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20], name[256];
        sscanf(ln,"%s %s",cmd,name);
        char *n=strstr(ln,name); if(n) { n+=strlen(name); if(*n) n++; } else n="";
        if(!strcmp(cmd,"JOIN") || !strcmp(cmd,"REJOIN")) { qs=realloc(qs,(nq+1)*sizeof(Q)); qs[nq].name=strdup(name); qs[nq].note=strdup(n); nq++; }
        else if(!strcmp(cmd,"SERVE")) { if(nq>0) { printf("%s\n",qs[0].name); free(qs[0].name); free(qs[0].note); for(int i=0;i<nq-1;i++) qs[i]=qs[i+1]; nq--; } }
        else if(!strcmp(cmd,"CANCEL")) { int i=findq(name); if(i>=0) { free(qs[i].name); free(qs[i].note); for(int j=i;j<nq-1;j++) qs[j]=qs[j+1]; nq--; } }
        else if(!strcmp(cmd,"FIND")) { int i=findq(name); if(i>=0) printf("%s\n",qs[i].note); }
        else if(!strcmp(cmd,"QUEUE")) { for(int i=0;i<nq;i++) printf("%s\n",qs[i].name); }
    }
    for(int i=0;i<nq;i++) { free(qs[i].name); free(qs[i].note); } free(qs);
    return 0;
}