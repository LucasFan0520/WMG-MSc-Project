// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char val[100]; } V;
typedef struct { char key[100]; V vals[1000]; int cnt; } K;
K keys[1000]; int kn=0;
int findK(char *key) { for(int i=0;i<kn;i++) if(!strcmp(keys[i].key,key)) return i; return -1; }

int main() {
    char cmd[20], key[100], val[100];
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"ADD")) {
            scanf("%s %s", key, val);
            int i=findK(key);
            if(i==-1) { strcpy(keys[kn].key,key); i=kn++; }
            strcpy(keys[i].vals[keys[i].cnt++].val, val);
        } else if(!strcmp(cmd,"REMOVE")) {
            scanf("%s %s", key, val);
            int i=findK(key);
            if(i!=-1) {
                for(int j=0;j<keys[i].cnt;j++) {
                    if(!strcmp(keys[i].vals[j].val,val)) {
                        for(int k=j;k<keys[i].cnt-1;k++) keys[i].vals[k]=keys[i].vals[k+1];
                        keys[i].cnt--; break;
                    }
                }
                if(keys[i].cnt==0) { for(int k=i;k<kn-1;k++) keys[k]=keys[k+1]; kn--; }
            }
        } else if(!strcmp(cmd,"ERASE")) {
            scanf("%s", key); int i=findK(key);
            if(i!=-1) { for(int j=i;j<kn-1;j++) keys[j]=keys[j+1]; kn--; }
        } else if(!strcmp(cmd,"VALUES")) {
            scanf("%s", key); int i=findK(key);
            if(i!=-1) for(int j=0;j<keys[i].cnt;j++) printf("%s\n",keys[i].vals[j].val);
        } else if(!strcmp(cmd,"KEYS")) {
            for(int i=0;i<kn;i++) printf("%s\n",keys[i].key);
        }
    }
    return 0;
}