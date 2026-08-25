// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title, *body; } D;
D *ds = NULL; int nd = 0;
D *trash = NULL; int nt = 0;

int findd(char *t) { for(int i=0;i<nd;i++) if(!strcmp(ds[i].title,t)) return i; return -1; }

int main() {
    char ln[2048];
    while(fgets(ln,2048,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20], title[256];
        sscanf(ln,"%s %s",cmd,title);
        char *b=strstr(ln,title); if(b) { b+=strlen(title); if(*b) b++; } else b="";
        if(!strcmp(cmd,"CREATE")) { ds=realloc(ds,(nd+1)*sizeof(D)); ds[nd].title=strdup(title); ds[nd].body=strdup(b); nd++; }
        else if(!strcmp(cmd,"REVISE")) { int i=findd(title); if(i>=0) { free(ds[i].body); ds[i].body=strdup(b); } }
        else if(!strcmp(cmd,"DISCARD")) { int i=findd(title); if(i>=0) { trash=realloc(trash,(nt+1)*sizeof(D)); trash[nt].title=strdup(ds[i].title); trash[nt].body=strdup(ds[i].body); nt++; free(ds[i].title); free(ds[i].body); for(int j=i;j<nd-1;j++) ds[j]=ds[j+1]; nd--; } }
        else if(!strcmp(cmd,"RESTORE")) { if(nt>0) { nt--; ds=realloc(ds,(nd+1)*sizeof(D)); ds[nd]=trash[nt]; nd++; } }
        else if(!strcmp(cmd,"FIND")) { int i=findd(title); if(i>=0) printf("%s\n",ds[i].body); }
        else if(!strcmp(cmd,"PRINT")) { for(int i=0;i<nd;i++) printf("%s %s\n",ds[i].title,ds[i].body); }
    }
    for(int i=0;i<nd;i++) { free(ds[i].title); free(ds[i].body); } free(ds);
    for(int i=0;i<nt;i++) { free(trash[i].title); free(trash[i].body); } free(trash);
    return 0;
}