// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *email, *phone, *note; } C;
C *cs = NULL; int nc = 0;

int findc(char *n) { for(int i=0;i<nc;i++) if(!strcmp(cs[i].name,n)) return i; return -1; }

int main() {
    char ln[1024];
    while(fgets(ln,1024,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20], name[256], arg1[256], arg2[256];
        sscanf(ln,"%s %s",cmd,name);
        char *p=strstr(ln,name); if(p) { p+=strlen(name); if(*p) p++; } else p="";
        int i=findc(name);
        if(!strcmp(cmd,"ADD")) { sscanf(p,"%s %s",arg1,arg2); cs=realloc(cs,(nc+1)*sizeof(C)); cs[nc].name=strdup(name); cs[nc].email=strdup(arg1); cs[nc].phone=strdup(arg2); cs[nc].note=strdup(""); nc++; }
        else if(!strcmp(cmd,"NOTE")) { if(i>=0) { free(cs[i].note); cs[i].note=strdup(p); } }
        else if(!strcmp(cmd,"MERGE")) { sscanf(p,"%s",arg1); int ik=findc(name), ir=findc(arg1); if(ik>=0 && ir>=0) { char *nn=malloc(strlen(cs[ik].note)+strlen(cs[ir].note)+2); sprintf(nn,"%s %s",cs[ik].note,cs[ir].note); free(cs[ik].note); cs[ik].note=nn; free(cs[ir].name); free(cs[ir].email); free(cs[ir].phone); free(cs[ir].note); for(int j=ir;j<nc-1;j++) cs[j]=cs[j+1]; nc--; } }
        else if(!strcmp(cmd,"DELETE")) { if(i>=0) { free(cs[i].name); free(cs[i].email); free(cs[i].phone); free(cs[i].note); for(int j=i;j<nc-1;j++) cs[j]=cs[j+1]; nc--; } }
        else if(!strcmp(cmd,"FIND")) { if(i>=0) printf("%s %s %s %s\n",cs[i].name,cs[i].email,cs[i].phone,cs[i].note); }
        else if(!strcmp(cmd,"LIST")) { for(int j=0;j<nc;j++) printf("%s %s %s %s\n",cs[j].name,cs[j].email,cs[j].phone,cs[j].note); }
    }
    for(int i=0;i<nc;i++) { free(cs[i].name); free(cs[i].email); free(cs[i].phone); free(cs[i].note); } free(cs);
    return 0;
}