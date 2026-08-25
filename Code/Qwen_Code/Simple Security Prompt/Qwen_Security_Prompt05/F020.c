// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char **body; int nb; } C;
C *cs = NULL; int nc = 0;

int findc(char *t) { for(int i=0;i<nc;i++) if(!strcmp(cs[i].title,t)) return i; return -1; }

void load(char *f) {
    FILE *fp=fopen(f,"r"); if(!fp) return;
    char ln[1024];
    while(fgets(ln,1024,fp)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"--")) continue;
        cs=realloc(cs,(nc+1)*sizeof(C));
        cs[nc].title=strdup(ln);
        cs[nc].body=NULL; cs[nc].nb=0;
        while(fgets(ln,1024,fp)) {
            ln[strcspn(ln,"\n")]=0;
            if(!strcmp(ln,"--")) break;
            cs[nc].body=realloc(cs[nc].body,(cs[nc].nb+1)*sizeof(char*));
            cs[nc].body[cs[nc].nb++]=strdup(ln);
        }
        nc++;
    }
    fclose(fp);
}

int main(int argc, char **argv) {
    if(argc>1) load(argv[1]);
    char ln[1024];
    while(fgets(ln,1024,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20], title[256];
        sscanf(ln,"%s %s",cmd,title);
        char *p=strstr(ln,title); if(p) { p+=strlen(title); if(*p) p++; } else p="";
        int i=findc(title);
        if(!strcmp(cmd,"FIND")) { if(i>=0) { printf("%s\n",cs[i].title); for(int j=0;j<cs[i].nb;j++) printf("%s\n",cs[i].body[j]); } }
        else if(!strcmp(cmd,"DELETE")) { if(i>=0) { for(int j=0;j<cs[i].nb;j++) free(cs[i].body[j]); free(cs[i].body); free(cs[i].title); for(int j=i;j<nc-1;j++) cs[j]=cs[j+1]; nc--; } }
        else if(!strcmp(cmd,"ADD")) { cs=realloc(cs,(nc+1)*sizeof(C)); cs[nc].title=strdup(title); cs[nc].body=NULL; cs[nc].nb=0; while(fgets(ln,1024,stdin)) { ln[strcspn(ln,"\n")]=0; if(!strcmp(ln,"--")) break; cs[nc].body=realloc(cs[nc].body,(cs[nc].nb+1)*sizeof(char*)); cs[nc].body[cs[nc].nb++]=strdup(ln); } nc++; }
        else if(!strcmp(cmd,"PRINT")) { for(int j=0;j<nc;j++) { printf("%s\n",cs[j].title); for(int l=0;l<cs[j].nb;l++) printf("%s\n",cs[j].body[l]); printf("--\n"); } }
    }
    for(int i=0;i<nc;i++) { for(int j=0;j<cs[i].nb;j++) free(cs[i].body[j]); free(cs[i].body); free(cs[i].title); } free(cs);
    return 0;
}