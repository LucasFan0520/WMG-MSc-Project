// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *exp; } A;
A *as = NULL; int na = 0;

int main() {
    char ln[1024];
    while(fgets(ln,1024,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20], a[256], b[256];
        sscanf(ln,"%s",cmd);
        char *p1 = strchr(ln,' '); if(p1) p1++;
        if(!strcmp(cmd,"DEFINE")) { sscanf(p1,"%s",a); char *e=p1+strlen(a)+1; int i; for(i=0;i<na;i++) if(!strcmp(as[i].name,a)) break; if(i<na) { free(as[i].exp); as[i].exp=strdup(e); } else { as=realloc(as,(na+1)*sizeof(A)); as[na].name=strdup(a); as[na].exp=strdup(e); na++; } }
        else if(!strcmp(cmd,"EXPAND")) { sscanf(p1,"%s",a); for(int i=0;i<na;i++) if(!strcmp(as[i].name,a)) { for(char *p=as[i].exp;*p;p++) putchar(*p==' '?'_':*p); putchar('\n'); break; } }
        else if(!strcmp(cmd,"RENAME")) { sscanf(p1,"%s %s",a,b); for(int i=0;i<na;i++) if(!strcmp(as[i].name,a)) { free(as[i].name); as[i].name=strdup(b); break; } }
        else if(!strcmp(cmd,"ERASE")) { sscanf(p1,"%s",a); for(int i=0;i<na;i++) if(!strcmp(as[i].name,a)) { free(as[i].name); free(as[i].exp); for(int j=i;j<na-1;j++) as[j]=as[j+1]; na--; break; } }
        else if(!strcmp(cmd,"DUMP")) { for(int i=0;i<na;i++) printf("%s %s\n",as[i].name,as[i].exp); }
    }
    for(int i=0;i<na;i++) { free(as[i].name); free(as[i].exp); } free(as);
    return 0;
}