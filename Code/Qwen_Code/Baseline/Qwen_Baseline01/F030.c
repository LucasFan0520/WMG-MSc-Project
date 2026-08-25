// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *old; char *new; } Rule;
Rule *rules = NULL; int nr = 0;

int main() {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000], arg2[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"ADD")) {
            sscanf(line,"%*s %s %s",arg1,arg2);
            rules=realloc(rules,sizeof(Rule)*(nr+1));
            rules[nr].old=strdup(arg1); rules[nr].new=strdup(arg2); nr++;
        } else if(!strcmp(cmd,"APPLY")) {
            char *p=line+6; while(*p==' ')p++;
            int found=0;
            for(int i=0;i<nr;i++) {
                char *pos=strstr(p,rules[i].old);
                if(pos) {
                    char res[10000];
                    strncpy(res,p,pos-p); res[pos-p]=0;
                    strcat(res,rules[i].new);
                    strcat(res,pos+strlen(rules[i].old));
                    printf("%s\n",res);
                    found=1; break;
                }
            }
            if(!found) printf("%s\n",p);
        } else if(!strcmp(cmd,"DELETE")) {
            sscanf(line,"%*s %s",arg1);
            for(int i=0;i<nr;i++) {
                if(!strcmp(rules[i].old,arg1)) {
                    free(rules[i].old); free(rules[i].new);
                    for(int j=i;j<nr-1;j++) rules[j]=rules[j+1];
                    nr--; break;
                }
            }
        } else if(!strcmp(cmd,"SWAP")) {
            int i,j; sscanf(line,"%*s %d %d",&i,&j);
            if(i>=0 && i<nr && j>=0 && j<nr) {
                Rule t=rules[i]; rules[i]=rules[j]; rules[j]=t;
            }
        } else if(!strcmp(cmd,"RULES")) {
            for(int i=0;i<nr;i++) printf("%s %s\n",rules[i].old,rules[i].new);
        }
    }
    return 0;
}