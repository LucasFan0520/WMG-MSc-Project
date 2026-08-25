// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int rows; int cols; char ***cells; char *name; } Tab;
Tab *tabs = NULL; int nt = 0;

int find_tab(const char *n) {
    for(int i=0;i<nt;i++) if(!strcmp(tabs[i].name,n)) return i;
    return -1;
}

int main() {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"TABLE")) {
            int r,c; sscanf(line,"%*s %s %d %d",arg1,&r,&c);
            tabs=realloc(tabs,sizeof(Tab)*(nt+1));
            tabs[nt].name=strdup(arg1); tabs[nt].rows=r; tabs[nt].cols=c;
            tabs[nt].cells=malloc(sizeof(char**)*r);
            for(int i=0;i<r;i++) {
                tabs[nt].cells[i]=malloc(sizeof(char*)*c);
                for(int j=0;j<c;j++) tabs[nt].cells[i][j]=NULL;
            }
            nt++;
        } else if(!strcmp(cmd,"SET")) {
            int r,c; sscanf(line,"%*s %s %d %d",arg1,&r,&c);
            char *p=line; while(*p && *p!=' ') p++; while(*p==' ') p++;
            while(*p && *p!=' ') p++; while(*p==' ') p++;
            while(*p && *p!=' ') p++; while(*p==' ') p++;
            while(*p && *p!=' ') p++; while(*p==' ') p++;
            int i=find_tab(arg1);
            if(i!=-1 && r>=0 && r<tabs[i].rows && c>=0 && c<tabs[i].cols) {
                free(tabs[i].cells[r][c]);
                tabs[i].cells[r][c]=strdup(p);
            }
        } else if(!strcmp(cmd,"GET")) {
            int r,c; sscanf(line,"%*s %s %d %d",arg1,&r,&c);
            int i=find_tab(arg1);
            if(i!=-1 && r>=0 && r<tabs[i].rows && c>=0 && c<tabs[i].cols) {
                if(tabs[i].cells[r][c]) printf("%s\n",tabs[i].cells[r][c]);
                else printf("EMPTY\n");
            }
        } else if(!strcmp(cmd,"DELETE")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_tab(arg1);
            if(i!=-1) {
                free(tabs[i].name);
                for(int r=0;r<tabs[i].rows;r++) {
                    for(int c=0;c<tabs[i].cols;c++) free(tabs[i].cells[r][c]);
                    free(tabs[i].cells[r]);
                }
                free(tabs[i].cells);
                for(int j=i;j<nt-1;j++) tabs[j]=tabs[j+1];
                nt--;
            }
        } else if(!strcmp(cmd,"TABLES")) {
            for(int i=0;i<nt;i++) printf("%s\n",tabs[i].name);
        }
    }
    return 0;
}