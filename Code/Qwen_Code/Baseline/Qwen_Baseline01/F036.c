// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *data; } Rec;
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
        if(!strcmp(cmd,"ADD")) {
            sscanf(line,"%*s %s",arg1);
            char *p=line+4+strlen(arg1); while(*p==' ')p++;
            int len; char *colon=strchr(p,':');
            if(!colon) continue;
            if(sscanf(p,"%d",&len)!=1) continue;
            if(strlen(colon+1) != len) continue;
            recs=realloc(recs,sizeof(Rec)*(nr+1));
            recs[nr].name=strdup(arg1); recs[nr].data=strdup(colon+1); nr++;
        } else if(!strcmp(cmd,"DELETE")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_rec(arg1);
            if(i!=-1) {
                free(recs[i].name); free(recs[i].data);
                for(int j=i;j<nr-1;j++) recs[j]=recs[j+1];
                nr--;
            }
        } else if(!strcmp(cmd,"SHOW")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_rec(arg1);
            if(i!=-1) {
                for(int j=0;recs[i].data[j];j++) putchar(recs[i].data[j]==' '?'_':recs[i].data[j]);
                putchar('\n');
            }
        } else if(!strcmp(cmd,"REPORT")) {
            for(int i=0;i<nr;i++) printf("%s\n",recs[i].name);
        }
    }
    return 0;
}