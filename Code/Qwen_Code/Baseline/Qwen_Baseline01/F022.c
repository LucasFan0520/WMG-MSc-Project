// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title; char *note; int done; } Task;
typedef struct { char *name; Task *tasks; int n; } Proj;
Proj *projs = NULL; int np = 0;

int find_proj(const char *n) {
    for(int i=0;i<np;i++) if(!strcmp(projs[i].name,n)) return i;
    return -1;
}

int main() {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000], arg2[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"PROJECT")) {
            sscanf(line,"%*s %s",arg1);
            projs=realloc(projs,sizeof(Proj)*(np+1));
            projs[np].name=strdup(arg1); projs[np].tasks=NULL; projs[np].n=0; np++;
        } else if(!strcmp(cmd,"TASK")) {
            sscanf(line,"%*s %s",arg1);
            char *p=line+5+strlen(arg1); while(*p==' ')p++;
            char title[1000]; sscanf(p,"%s",title);
            char *note=p+strlen(title); while(*note==' ')note++;
            int i=find_proj(arg1);
            if(i!=-1) {
                projs[i].tasks=realloc(projs[i].tasks,sizeof(Task)*(projs[i].n+1));
                projs[i].tasks[projs[i].n].title=strdup(title);
                projs[i].tasks[projs[i].n].note=strdup(note);
                projs[i].tasks[projs[i].n].done=0;
                projs[i].n++;
            }
        } else if(!strcmp(cmd,"MOVETASK")) {
            sscanf(line,"%*s %s %s",arg1,arg2);
            char *p=line+9+strlen(arg1)+1+strlen(arg2); while(*p==' ')p++;
            int i1=find_proj(arg1), i2=find_proj(arg2);
            if(i1!=-1 && i2!=-1) {
                for(int j=0;j<projs[i1].n;j++) {
                    if(!strcmp(projs[i1].tasks[j].title,p)) {
                        projs[i2].tasks=realloc(projs[i2].tasks,sizeof(Task)*(projs[i2].n+1));
                        projs[i2].tasks[projs[i2].n]=projs[i1].tasks[j];
                        projs[i2].n++;
                        for(int k=j;k<projs[i1].n-1;k++) projs[i1].tasks[k]=projs[i1].tasks[k+1];
                        projs[i1].n--; break;
                    }
                }
            }
        } else if(!strcmp(cmd,"DONETASK")) {
            sscanf(line,"%*s %s",arg1);
            char *p=line+9+strlen(arg1); while(*p==' ')p++;
            int i=find_proj(arg1);
            if(i!=-1) {
                for(int j=0;j<projs[i].n;j++) {
                    if(!strcmp(projs[i].tasks[j].title,p)) {
                        projs[i].tasks[j].done=1; break;
                    }
                }
            }
        } else if(!strcmp(cmd,"DELETEPROJECT")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_proj(arg1);
            if(i!=-1) {
                free(projs[i].name);
                for(int j=0;j<projs[i].n;j++) { free(projs[i].tasks[j].title); free(projs[i].tasks[j].note); }
                free(projs[i].tasks);
                for(int j=i;j<np-1;j++) projs[j]=projs[j+1];
                np--;
            }
        } else if(!strcmp(cmd,"REPORT")) {
            for(int i=0;i<np;i++) {
                printf("%s\n",projs[i].name);
                for(int j=0;j<projs[i].n;j++) {
                    printf(" %s %s %s\n",projs[i].tasks[j].title,projs[i].tasks[j].done?"DONE":"TODO",projs[i].tasks[j].note);
                }
            }
        }
    }
    return 0;
}