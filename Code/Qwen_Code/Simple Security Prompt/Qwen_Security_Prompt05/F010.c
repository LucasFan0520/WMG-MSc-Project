// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char **vals; int nv; } K;
K *ks = NULL; int nk = 0;

int findk(char *k) { for(int i=0;i<nk;i++) if(!strcmp(ks[i].key,k)) return i; return -1; }

int main() {
    char ln[1024];
    while(fgets(ln,1024,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20], k[256], v[256];
        sscanf(ln,"%s %s %s",cmd,k,v);
        int i=findk(k);
        if(!strcmp(cmd,"ADD")) { if(i<0) { ks=realloc(ks,(nk+1)*sizeof(K)); ks[nk].key=strdup(k); ks[nk].vals=NULL; ks[nk].nv=0; i=nk++; } ks[i].vals=realloc(ks[i].vals,(ks[i].nv+1)*sizeof(char*)); ks[i].vals[ks[i].nv++]=strdup(v); }
        else if(!strcmp(cmd,"REMOVE")) { if(i>=0) { for(int j=0;j<ks[i].nv;j++) if(!strcmp(ks[i].vals[j],v)) { free(ks[i].vals[j]); for(int l=j;l<ks[i].nv-1;l++) ks[i].vals[l]=ks[i].vals[l+1]; ks[i].nv--; break; } } }
        else if(!strcmp(cmd,"ERASE")) { if(i>=0) { for(int j=0;j<ks[i].nv;j++) free(ks[i].vals[j]); free(ks[i].vals); free(ks[i].key); for(int j=i;j<nk-1;j++) ks[j]=ks[j+1]; nk--; } }
        else if(!strcmp(cmd,"VALUES")) { if(i>=0) for(int j=0;j<ks[i].nv;j++) printf("%s\n",ks[i].vals[j]); }
        else if(!strcmp(cmd,"KEYS")) { for(int j=0;j<nk;j++) printf("%s\n",ks[j].key); }
    }
    for(int i=0;i<nk;i++) { for(int j=0;j<ks[i].nv;j++) free(ks[i].vals[j]); free(ks[i].vals); free(ks[i].key); } free(ks);
    return 0;
}