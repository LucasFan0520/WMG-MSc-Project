// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char acc[100]; int amt; char memo[1000]; } T;
T arr[1000]; int n=0;

void load(char *fname) {
    FILE *f=fopen(fname,"r"); if(!f) return;
    char line[1100];
    while(fgets(line,1100,f)) {
        line[strcspn(line,"\n")]=0;
        char acc[100], memo[1000]; int amt;
        if(sscanf(line,"%s %d",acc,&amt)==2) {
            char *m=strchr(line,' '); if(m) m=strchr(m+1,' ');
            if(m) { strcpy(arr[n].acc,acc); arr[n].amt=amt; strcpy(arr[n].memo,m+1); n++; }
        }
    }
    fclose(f);
}

int bal(char *acc) {
    int s=0; for(int i=0;i<n;i++) if(!strcmp(arr[i].acc,acc)) s+=arr[i].amt; return s;
}

int main(int argc, char **argv) {
    if(argc>1) load(argv[1]);
    char cmd[20], acc[100], memo[1000]; int amt;
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"ADD")) {
            scanf("%s %d", acc, &amt); fgets(memo,1000,stdin); memo[strcspn(memo,"\n")]=0;
            if(memo[0]==' ') memmove(memo,memo+1,strlen(memo));
            strcpy(arr[n].acc,acc); arr[n].amt=amt; strcpy(arr[n].memo,memo); n++;
        } else if(!strcmp(cmd,"REVERSE")) {
            scanf("%s %d", acc, &amt);
            for(int i=0;i<n;i++) if(!strcmp(arr[i].acc,acc) && arr[i].amt==amt) { arr[i].amt=-arr[i].amt; break; }
        } else if(!strcmp(cmd,"BALANCE")) {
            scanf("%s", acc); printf("%d\n",bal(acc));
        } else if(!strcmp(cmd,"REPORT")) {
            for(int i=0;i<n;i++) printf("%s %d %s\n",arr[i].acc,arr[i].amt,arr[i].memo);
        }
    }
    return 0;
}