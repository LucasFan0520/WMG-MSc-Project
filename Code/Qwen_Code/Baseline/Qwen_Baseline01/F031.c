// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *note; } Rec;
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
        char cmd[20];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"BATCH")) {
            int n; sscanf(line,"%*s %d",&n);
            Rec *batch=malloc(sizeof(Rec)*n);
            int valid=1;
            for(int i=0;i<n;i++) {
                if(!fgets(line,sizeof(line),stdin)) { valid=0; break; }
                line[strcspn(line,"\n")]=0;
                char name[1000]; sscanf(line,"%s",name);
                char *note=line+strlen(name); while(*note==' ')note++;
                batch[i].name=strdup(name); batch[i].note=strdup(note);
            }
            if(valid) {
                for(int i=0;i<n;i++) {
                    recs=realloc(recs,sizeof(Rec)*(nr+1));
                    recs[nr++]=batch[i];
                }
            } else {
                for(int i=0;i<n;i++) { free(batch[i].name); free(batch[i].note); }
            }
            free(batch);
        } else if(!strcmp(cmd,"DELETE")) {
            char arg1[1000]; sscanf(line,"%*s %s",arg1);
            int i=find_rec(arg1);
            if(i!=-1) {
                free(recs[i].name); free(recs[i].note);
                for(int j=i;j<nr-1;j++) recs[j]=recs[j+1];
                nr--;
            }
        } else if(!strcmp(cmd,"FIND")) {
            char arg1[1000]; sscanf(line,"%*s %s",arg1);
            int i=find_rec(arg1);
            if(i!=-1) printf("%s %s\n",recs[i].name,recs[i].note);
        } else if(!strcmp(cmd,"REPORT")) {
            for(int i=0;i<nr;i++) printf("%s %s\n",recs[i].name,recs[i].note);
        }
    }
    return 0;
}