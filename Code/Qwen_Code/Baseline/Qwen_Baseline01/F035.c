// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *payload; } Job;
Job *jobs = NULL; int nj = 0;

int find_job(const char *n) {
    for(int i=0;i<nj;i++) if(!strcmp(jobs[i].name,n)) return i;
    return -1;
}

int main() {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000], arg2[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"ENQUEUE")) {
            sscanf(line,"%*s %s",arg1);
            char *p=line+8+strlen(arg1); while(*p==' ')p++;
            jobs=realloc(jobs,sizeof(Job)*(nj+1));
            jobs[nj].name=strdup(arg1); jobs[nj].payload=strdup(p); nj++;
        } else if(!strcmp(cmd,"CLONE")) {
            sscanf(line,"%*s %s %s",arg1,arg2);
            int i=find_job(arg1);
            if(i!=-1) {
                jobs=realloc(jobs,sizeof(Job)*(nj+1));
                jobs[nj].name=strdup(arg2); jobs[nj].payload=strdup(jobs[i].payload); nj++;
            }
        } else if(!strcmp(cmd,"CANCEL")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_job(arg1);
            if(i!=-1) {
                free(jobs[i].name); free(jobs[i].payload);
                for(int j=i;j<nj-1;j++) jobs[j]=jobs[j+1];
                nj--;
            }
        } else if(!strcmp(cmd,"RUN")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_job(arg1);
            if(i!=-1) {
                printf("%s\n",jobs[i].payload);
                free(jobs[i].name); free(jobs[i].payload);
                for(int j=i;j<nj-1;j++) jobs[j]=jobs[j+1];
                nj--;
            }
        } else if(!strcmp(cmd,"LIST")) {
            for(int i=0;i<nj;i++) printf("%s %s\n",jobs[i].name,jobs[i].payload);
        }
    }
    return 0;
}