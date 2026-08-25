// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *acc; int amt; char *memo; } T;
T *ts = NULL; int nt = 0;

void load(char *f) {
    FILE *fp=fopen(f,"r"); if(!fp) return;
    char ln[1024];
    while(fgets(ln,1024,fp)) {
        ln[strcspn(ln,"\n")]=0;
        char acc[256], memo[1024]=""; int amt;
        if(sscanf(ln,"%s %d",acc,&amt)==2) {
            char *p=strchr(ln,' '); if(p) { p=strchr(p+1,' '); if(p) { p++; strcpy(memo,p); } }
            ts=realloc(ts,(nt+1)*sizeof(T)); ts[nt].acc=strdup(acc); ts[nt].amt=amt; ts[nt].memo=strdup(memo); nt++;
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
        char cmd[20], acc[256], memo[1024]=""; int amt;
        sscanf(ln,"%s %s %d",cmd,acc,&amt);
        char *p=strstr(ln,acc); if(p) { p+=strlen(acc); while(*p==' ') p++; sscanf(p,"%d",&amt); p=strchr(p,' '); if(p) { p++; strcpy(memo,p); } }
        if(!strcmp(cmd,"ADD")) { ts=realloc(ts,(nt+1)*sizeof(T)); ts[nt].acc=strdup(acc); ts[nt].amt=amt; ts[nt].memo=strdup(memo); nt++; }
        else if(!strcmp(cmd,"REVERSE")) { for(int i=0;i<nt;i++) if(!strcmp(ts[i].acc,acc) && ts[i].amt==amt) { ts[i].amt=-ts[i].amt; break; } }
        else if(!strcmp(cmd,"BALANCE")) { int sum=0; for(int i=0;i<nt;i++) if(!strcmp(ts[i].acc,acc)) sum+=ts[i].amt; printf("%d\n",sum); }
        else if(!strcmp(cmd,"REPORT")) { for(int i=0;i<nt;i++) printf("%s %d %s\n",ts[i].acc,ts[i].amt,ts[i].memo); }
    }
    for(int i=0;i<nt;i++) { free(ts[i].acc); free(ts[i].memo); } free(ts);
    return 0;
}