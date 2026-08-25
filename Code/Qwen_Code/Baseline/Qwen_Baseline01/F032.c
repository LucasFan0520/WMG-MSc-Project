// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char *val; } KV;
KV *kvs = NULL; int nk = 0;
KV *trans = NULL; int nt = 0;
int in_trans = 0;

int find_kv(KV *arr, int n, const char *k) {
    for(int i=0;i<n;i++) if(!strcmp(arr[i].key,k)) return i;
    return -1;
}

int main() {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"SET")) {
            sscanf(line,"%*s %s",arg1);
            char *p=line+4+strlen(arg1); while(*p==' ')p++;
            int i=find_kv(kvs,nk,arg1);
            if(i!=-1) { free(kvs[i].val); kvs[i].val=strdup(p); }
            else { kvs=realloc(kvs,sizeof(KV)*(nk+1)); kvs[nk].key=strdup(arg1); kvs[nk].val=strdup(p); nk++; }
        } else if(!strcmp(cmd,"BEGIN")) {
            in_trans=1; nt=0;
        } else if(!strcmp(cmd,"TSET")) {
            sscanf(line,"%*s %s",arg1);
            char *p=line+5+strlen(arg1); while(*p==' ')p++;
            int i=find_kv(trans,nt,arg1);
            if(i!=-1) { free(trans[i].val); trans[i].val=strdup(p); }
            else { trans=realloc(trans,sizeof(KV)*(nt+1)); trans[nt].key=strdup(arg1); trans[nt].val=strdup(p); nt++; }
        } else if(!strcmp(cmd,"COMMIT")) {
            for(int i=0;i<nt;i++) {
                int j=find_kv(kvs,nk,trans[i].key);
                if(j!=-1) { free(kvs[j].val); kvs[j].val=strdup(trans[i].val); }
                else { kvs=realloc(kvs,sizeof(KV)*(nk+1)); kvs[nk].key=strdup(trans[i].key); kvs[nk].val=strdup(trans[i].val); nk++; }
            }
            for(int i=0;i<nt;i++) { free(trans[i].key); free(trans[i].val); }
            nt=0; in_trans=0;
        } else if(!strcmp(cmd,"ABORT")) {
            for(int i=0;i<nt;i++) { free(trans[i].key); free(trans[i].val); }
            nt=0; in_trans=0;
        } else if(!strcmp(cmd,"GET")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_kv(kvs,nk,arg1);
            if(i!=-1) printf("%s\n",kvs[i].val);
        } else if(!strcmp(cmd,"DUMP")) {
            for(int i=0;i<nk;i++) printf("%s %s\n",kvs[i].key,kvs[i].val);
        }
    }
    return 0;
}