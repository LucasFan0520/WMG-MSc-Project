// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *desc; } Ing;
typedef struct { char *name; Ing *ings; int n; } Rec;
Rec *recs = NULL; int nr = 0;

int find_rec(const char *n) {
    for(int i=0;i<nr;i++) if(!strcmp(recs[i].name,n)) return i;
    return -1;
}

int main() {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"RECIPE")) {
            sscanf(line,"%*s %s",arg1);
            recs=realloc(recs,sizeof(Rec)*(nr+1));
            recs[nr].name=strdup(arg1); recs[nr].ings=NULL; recs[nr].n=0; nr++;
        } else if(!strcmp(cmd,"ING")) {
            char *p=line+4; while(*p==' ')p++;
            char name[1000]; sscanf(p,"%s",name);
            char *desc=p+strlen(name); while(*desc==' ')desc++;
            int i=find_rec(name);
            if(i!=-1) {
                recs[i].ings=realloc(recs[i].ings,sizeof(Ing)*(recs[i].n+1));
                recs[i].ings[recs[i].n++].desc=strdup(desc);
            }
        } else if(!strcmp(cmd,"REMOVEING")) {
            char *p=line+10; while(*p==' ')p++;
            char name[1000]; sscanf(p,"%s",name);
            char *desc=p+strlen(name); while(*desc==' ')desc++;
            int i=find_rec(name);
            if(i!=-1) {
                for(int j=0;j<recs[i].n;j++) {
                    if(!strcmp(recs[i].ings[j].desc,desc)) {
                        free(recs[i].ings[j].desc);
                        for(int k=j;k<recs[i].n-1;k++) recs[i].ings[k]=recs[i].ings[k+1];
                        recs[i].n--; break;
                    }
                }
            }
        } else if(!strcmp(cmd,"DELETERECIPE")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_rec(arg1);
            if(i!=-1) {
                free(recs[i].name);
                for(int j=0;j<recs[i].n;j++) free(recs[i].ings[j].desc);
                free(recs[i].ings);
                for(int j=i;j<nr-1;j++) recs[j]=recs[j+1];
                nr--;
            }
        } else if(!strcmp(cmd,"SHOW")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_rec(arg1);
            if(i!=-1) {
                for(int j=0;j<recs[i].n;j++) printf("%s\n",recs[i].ings[j].desc);
            }
        } else if(!strcmp(cmd,"LIST")) {
            for(int i=0;i<nr;i++) printf("%s\n",recs[i].name);
        }
    }
    return 0;
}