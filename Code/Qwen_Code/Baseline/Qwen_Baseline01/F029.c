// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char *val; } Rec;
Rec *recs = NULL; int nr = 0;

int main() {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"ADD")) {
            sscanf(line,"%*s %s",arg1);
            char *p=line+4+strlen(arg1); while(*p==' ')p++;
            recs=realloc(recs,sizeof(Rec)*(nr+1));
            recs[nr].key=strdup(arg1); recs[nr].val=strdup(p); nr++;
        } else if(!strcmp(cmd,"DEDUP")) {
            for(int i=0;i<nr;i++) {
                for(int j=i+1;j<nr;) {
                    if(!strcmp(recs[i].key,recs[j].key)) {
                        free(recs[j].key); free(recs[j].val);
                        for(int k=j;k<nr-1;k++) recs[k]=recs[k+1];
                        nr--;
                    } else j++;
                }
            }
        } else if(!strcmp(cmd,"DELETE")) {
            sscanf(line,"%*s %s",arg1);
            for(int i=0;i<nr;) {
                if(!strcmp(recs[i].key,arg1)) {
                    free(recs[i].key); free(recs[i].val);
                    for(int j=i;j<nr-1;j++) recs[j]=recs[j+1];
                    nr--;
                } else i++;
            }
        } else if(!strcmp(cmd,"FIND")) {
            sscanf(line,"%*s %s",arg1);
            for(int i=0;i<nr;i++) if(!strcmp(recs[i].key,arg1)) printf("%s\n",recs[i].val);
        } else if(!strcmp(cmd,"REPORT")) {
            for(int i=0;i<nr;i++) printf("%s %s\n",recs[i].key,recs[i].val);
        }
    }
    return 0;
}