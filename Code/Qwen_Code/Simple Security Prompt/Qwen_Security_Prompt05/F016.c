// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *email, *phone; } C;
C *cs = NULL; int nc = 0;

int findc(char *n) { for(int i=0;i<nc;i++) if(!strcmp(cs[i].name,n)) return i; return -1; }

void load(char *f) {
    FILE *fp=fopen(f,"r"); if(!fp) return;
    char ln[1024];
    while(fgets(ln,1024,fp)) {
        ln[strcspn(ln,"\n")]=0;
        char *c1=strchr(ln,','), *c2=strchr(c1?c1+1:ln,',');
        if(c1 && c2) {
            *c1=0; *c2=0;
            cs=realloc(cs,(nc+1)*sizeof(C));
            cs[nc].name=strdup(ln); cs[nc].email=strdup(c1+1); cs[nc].phone=strdup(c2+1); nc++;
        }
    }
    fclose(fp);
}

int main(int argc, char **argv) {
    if(argc>1) load(argv[1]);
    char ln[1024];
    while(fgets(ln,1024,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20];
        sscanf(ln,"%s",cmd);
        char *p=strchr(ln,' '); if(p) p++;
        if(!strcmp(cmd,"ADD")) { char *c1=strchr(p,','), *c2=strchr(c1?c1+1:p,','); if(c1&&c2) { *c1=0; *c2=0; cs=realloc(cs,(nc+1)*sizeof(C)); cs[nc].name=strdup(p); cs[nc].email=strdup(c1+1); cs[nc].phone=strdup(c2+1); nc++; } }
        else if(!strcmp(cmd,"DELETE")) { int i=findc(p); if(i>=0) { free(cs[i].name); free(cs[i].email); free(cs[i].phone); for(int j=i;j<nc-1;j++) cs[j]=cs[j+1]; nc--; } }
        else if(!strcmp(cmd,"FIND")) { int i=findc(p); if(i>=0) printf("%s,%s,%s\n",cs[i].name,cs[i].email,cs[i].phone); }
        else if(!strcmp(cmd,"REPORT")) { for(int i=0;i<nc;i++) printf("%s,%s,%s\n",cs[i].name,cs[i].email,cs[i].phone); }
    }
    for(int i=0;i<nc;i++) { free(cs[i].name); free(cs[i].email); free(cs[i].phone); } free(cs);
    return 0;
}