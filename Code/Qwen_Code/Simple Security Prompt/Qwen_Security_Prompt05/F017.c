// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key, *val; } KV;
KV *kvs = NULL; int nk = 0;

int findk(char *k) { for(int i=0;i<nk;i++) if(!strcmp(kvs[i].key,k)) return i; return -1; }

void load(char *f) {
    FILE *fp=fopen(f,"r"); if(!fp) return;
    char ln[1024];
    while(fgets(ln,1024,fp)) {
        ln[strcspn(ln,"\n")]=0;
        char *eq=strchr(ln,'=');
        if(eq) {
            *eq=0;
            kvs=realloc(kvs,(nk+1)*sizeof(KV));
            kvs[nk].key=strdup(ln); kvs[nk].val=strdup(eq+1); nk++;
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
        char cmd[20], key[256];
        sscanf(ln,"%s %s",cmd,key);
        char *p=strstr(ln,key); if(p) { p+=strlen(key); if(*p) p++; } else p="";
        int i=findk(key);
        if(!strcmp(cmd,"SET")) { if(i>=0) { free(kvs[i].val); kvs[i].val=strdup(p); } else { kvs=realloc(kvs,(nk+1)*sizeof(KV)); kvs[nk].key=strdup(key); kvs[nk].val=strdup(p); nk++; } }
        else if(!strcmp(cmd,"UNSET")) { if(i>=0) { free(kvs[i].key); free(kvs[i].val); for(int j=i;j<nk-1;j++) kvs[j]=kvs[j+1]; nk--; } }
        else if(!strcmp(cmd,"GET")) { if(i>=0) printf("%s\n",kvs[i].val); }
        else if(!strcmp(cmd,"WRITE")) { for(int j=0;j<nk;j++) printf("%s=%s\n",kvs[j].key,kvs[j].val); }
    }
    for(int i=0;i<nk;i++) { free(kvs[i].key); free(kvs[i].val); } free(kvs);
    return 0;
}