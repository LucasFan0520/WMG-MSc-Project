// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int pri; char *title; char *note; } Rem;
Rem *rems = NULL; int nr = 0;

int main() {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"ADD")) {
            int p; sscanf(line,"%*s %d %s",&p,arg1);
            char *note=line;
            while(*note && *note!=' ') note++;
            while(*note==' ') note++;
            while(*note && *note!=' ') note++;
            while(*note==' ') note++;
            rems=realloc(rems,sizeof(Rem)*(nr+1));
            rems[nr].pri=p; rems[nr].title=strdup(arg1); rems[nr].note=strdup(note); nr++;
        } else if(!strcmp(cmd,"BUMP")) {
            int d; sscanf(line,"%*s %s %d",arg1,&d);
            for(int i=0;i<nr;i++) if(!strcmp(rems[i].title,arg1)) rems[i].pri+=d;
        } else if(!strcmp(cmd,"REMOVE")) {
            sscanf(line,"%*s %s",arg1);
            for(int i=0;i<nr;) {
                if(!strcmp(rems[i].title,arg1)) {
                    free(rems[i].title); free(rems[i].note);
                    for(int j=i;j<nr-1;j++) rems[j]=rems[j+1];
                    nr--;
                } else i++;
            }
        } else if(!strcmp(cmd,"TOP")) {
            int n; sscanf(line,"%*s %d",&n);
            Rem *sorted=malloc(sizeof(Rem)*nr);
            memcpy(sorted,rems,sizeof(Rem)*nr);
            for(int i=0;i<nr-1;i++) for(int j=i+1;j<nr;j++) {
                if(sorted[j].pri>sorted[i].pri) { Rem t=sorted[i]; sorted[i]=sorted[j]; sorted[j]=t; }
            }
            for(int i=0;i<n && i<nr;i++) printf("%d %s %s\n",sorted[i].pri,sorted[i].title,sorted[i].note);
            free(sorted);
        } else if(!strcmp(cmd,"LIST")) {
            for(int i=0;i<nr;i++) printf("%d %s %s\n",rems[i].pri,rems[i].title,rems[i].note);
        }
    }
    return 0;
}