// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *tag, *msg; } M;
M *ms = NULL; int nm = 0;

int main() {
    char ln[1024];
    while(fgets(ln,1024,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20], a[256], b[256], c[256];
        sscanf(ln,"%s",cmd);
        char *p1 = strchr(ln,' '); if(p1) p1++;
        if(!strcmp(cmd,"ADD")) { sscanf(p1,"%s",a); char *m=p1+strlen(a)+1; ms=realloc(ms,(nm+1)*sizeof(M)); ms[nm].tag=strdup(a); ms[nm].msg=strdup(m); nm++; }
        else if(!strcmp(cmd,"CONCAT")) { sscanf(p1,"%s %s %s",a,b,c); char *m1=NULL,*m2=NULL; for(int i=0;i<nm;i++) { if(!m1 && !strcmp(ms[i].tag,a)) m1=ms[i].msg; if(!m2 && !strcmp(ms[i].tag,b)) m2=ms[i].msg; } if(m1&&m2) { ms=realloc(ms,(nm+1)*sizeof(M)); ms[nm].tag=strdup(c); ms[nm].msg=malloc(strlen(m1)+strlen(m2)+2); sprintf(ms[nm].msg,"%s %s",m1,m2); nm++; } }
        else if(!strcmp(cmd,"REMOVE")) { sscanf(p1,"%s",a); for(int i=0;i<nm;) { if(!strcmp(ms[i].tag,a)) { free(ms[i].tag); free(ms[i].msg); for(int j=i;j<nm-1;j++) ms[j]=ms[j+1]; nm--; } else i++; } }
        else if(!strcmp(cmd,"FIND")) { sscanf(p1,"%s",a); for(int i=0;i<nm;i++) if(!strcmp(ms[i].tag,a)) { for(char *p=ms[i].msg;*p;p++) putchar(*p==' '?'_':*p); putchar('\n'); break; } }
        else if(!strcmp(cmd,"REPORT")) { for(int i=0;i<nm;i++) printf("%s %s\n",ms[i].tag,ms[i].msg); }
    }
    for(int i=0;i<nm;i++) { free(ms[i].tag); free(ms[i].msg); } free(ms);
    return 0;
}