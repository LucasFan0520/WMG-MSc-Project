// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *note; char **labels; int nl; } I;
I *is = NULL; int ni = 0;

int findi(char *n) { for(int i=0;i<ni;i++) if(!strcmp(is[i].name,n)) return i; return -1; }

int main() {
    char ln[1024];
    while(fgets(ln,1024,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20], name[256], arg[256];
        sscanf(ln,"%s %s",cmd,name);
        char *p=strstr(ln,name); if(p) { p+=strlen(name); if(*p) p++; } else p="";
        int i=findi(name);
        if(!strcmp(cmd,"ITEM")) { is=realloc(is,(ni+1)*sizeof(I)); is[ni].name=strdup(name); is[ni].note=strdup(p); is[ni].labels=NULL; is[ni].nl=0; ni++; }
        else if(!strcmp(cmd,"LABEL")) { sscanf(p,"%s",arg); if(i>=0) { is[i].labels=realloc(is[i].labels,(is[i].nl+1)*sizeof(char*)); is[i].labels[is[i].nl++]=strdup(arg); } }
        else if(!strcmp(cmd,"UNLABEL")) { sscanf(p,"%s",arg); if(i>=0) { for(int j=0;j<is[i].nl;j++) if(!strcmp(is[i].labels[j],arg)) { free(is[i].labels[j]); for(int l=j;l<is[i].nl-1;l++) is[i].labels[l]=is[i].labels[l+1]; is[i].nl--; break; } } }
        else if(!strcmp(cmd,"DELETE")) { if(i>=0) { for(int j=0;j<is[i].nl;j++) free(is[i].labels[j]); free(is[i].labels); free(is[i].name); free(is[i].note); for(int j=i;j<ni-1;j++) is[j]=is[j+1]; ni--; } }
        else if(!strcmp(cmd,"FIND")) { if(i>=0) { printf("%s %s",is[i].name,is[i].note); for(int j=0;j<is[i].nl;j++) printf(" %s",is[i].labels[j]); printf("\n"); } }
        else if(!strcmp(cmd,"REPORT")) { for(int j=0;j<ni;j++) { printf("%s %s",is[j].name,is[j].note); for(int l=0;l<is[j].nl;l++) printf(" %s",is[j].labels[l]); printf("\n"); } }
    }
    for(int i=0;i<ni;i++) { for(int j=0;j<is[i].nl;j++) free(is[i].labels[j]); free(is[i].labels); free(is[i].name); free(is[i].note); } free(is);
    return 0;
}