// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id; char *name; char *dept; char *title; } Emp;
Emp *emps = NULL; int ne = 0;

int find_emp(const char *id) {
    for(int i=0;i<ne;i++) if(!strcmp(emps[i].id,id)) return i;
    return -1;
}

int main() {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000], arg2[1000], arg3[1000], arg4[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"ADD") || !strcmp(cmd,"UPDATE")) {
            sscanf(line,"%*s %s %s %s %s",arg1,arg2,arg3,arg4);
            int i=find_emp(arg1);
            if(i!=-1) {
                free(emps[i].name); free(emps[i].dept); free(emps[i].title);
                emps[i].name=strdup(arg2); emps[i].dept=strdup(arg3); emps[i].title=strdup(arg4);
            } else {
                emps=realloc(emps,sizeof(Emp)*(ne+1));
                emps[ne].id=strdup(arg1); emps[ne].name=strdup(arg2); emps[ne].dept=strdup(arg3); emps[ne].title=strdup(arg4);
                ne++;
            }
        } else if(!strcmp(cmd,"DELETE")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_emp(arg1);
            if(i!=-1) {
                free(emps[i].id); free(emps[i].name); free(emps[i].dept); free(emps[i].title);
                for(int j=i;j<ne-1;j++) emps[j]=emps[j+1];
                ne--;
            }
        } else if(!strcmp(cmd,"FIND")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_emp(arg1);
            if(i!=-1) printf("%s %s %s %s\n",emps[i].id,emps[i].name,emps[i].dept,emps[i].title);
        } else if(!strcmp(cmd,"LIST")) {
            for(int i=0;i<ne;i++) printf("%s %s %s %s\n",emps[i].id,emps[i].name,emps[i].dept,emps[i].title);
        }
    }
    return 0;
}