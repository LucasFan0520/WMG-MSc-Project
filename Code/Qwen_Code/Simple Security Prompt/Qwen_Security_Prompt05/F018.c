// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *word; int count; } W;
W *ws = NULL; int nw = 0;

int findw(char *w) { for(int i=0;i<nw;i++) if(!strcmp(ws[i].word,w)) return i; return -1; }

int cmp(const void *a, const void *b) { return ((W*)b)->count - ((W*)a)->count; }

void load(char *f) {
    FILE *fp=fopen(f,"r"); if(!fp) return;
    char ln[1024];
    while(fgets(ln,1024,fp)) {
        ln[strcspn(ln,"\n")]=0;
        int i=findw(ln);
        if(i>=0) ws[i].count++;
        else { ws=realloc(ws,(nw+1)*sizeof(W)); ws[nw].word=strdup(ln); ws[nw].count=1; nw++; }
    }
    fclose(fp);
}

int main(int argc, char **argv) {
    if(argc>1) load(argv[1]);
    char ln[1024];
    while(fgets(ln,1024,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20];
        sscanf(ln,"%s",cmd);
        char *p=strchr(ln,' '); if(p) p++;
        int i=findw(p);
        if(!strcmp(cmd,"WORD")) { if(i>=0) ws[i].count++; else { ws=realloc(ws,(nw+1)*sizeof(W)); ws[nw].word=strdup(p); ws[nw].count=1; nw++; } }
        else if(!strcmp(cmd,"REMOVE")) { if(i>=0) { free(ws[i].word); for(int j=i;j<nw-1;j++) ws[j]=ws[j+1]; nw--; } }
        else if(!strcmp(cmd,"COUNT")) { printf("%d\n",i>=0?ws[i].count:0); }
        else if(!strcmp(cmd,"TOP")) { qsort(ws,nw,sizeof(W),cmp); for(int j=0;j<nw;j++) printf("%s %d\n",ws[j].word,ws[j].count); }
    }
    for(int i=0;i<nw;i++) free(ws[i].word); free(ws);
    return 0;
}