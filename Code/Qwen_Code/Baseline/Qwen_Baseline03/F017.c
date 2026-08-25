// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char key[100]; char val[1000]; } KV;
KV arr[1000]; int n=0;
int find(char *k) { for(int i=0;i<n;i++) if(!strcmp(arr[i].key,k)) return i; return -1; }

void load(char *fname) {
    FILE *f=fopen(fname,"r"); if(!f) return;
    char line[1100];
    while(fgets(line,1100,f)) {
        line[strcspn(line,"\n")]=0;
        char *eq=strchr(line,'=');
        if(eq) {
            *eq=0; strcpy(arr[n].key,line); strcpy(arr[n].val,eq+1); n++;
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if(argc>1) load(argv[1]);
    char cmd[20], key[100], val[1000];
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"SET")) {
            scanf("%s", key); fgets(val,1000,stdin); val[strcspn(val,"\n")]=0;
            if(val[0]==' ') memmove(val,val+1,strlen(val));
            int i=find(key);
            if(i!=-1) strcpy(arr[i].val,val);
            else { strcpy(arr[n].key,key); strcpy(arr[n].val,val); n++; }
        } else if(!strcmp(cmd,"UNSET")) {
            scanf("%s", key); int i=find(key);
            if(i!=-1) { for(int j=i;j<n-1;j++) arr[j]=arr[j+1]; n--; }
        } else if(!strcmp(cmd,"GET")) {
            scanf("%s", key); int i=find(key);
            if(i!=-1) printf("%s\n",arr[i].val);
        } else if(!strcmp(cmd,"WRITE")) {
            for(int i=0;i<n;i++) printf("%s=%s\n",arr[i].key,arr[i].val);
        }
    }
    return 0;
}