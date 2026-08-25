// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *enc; } RLE;
RLE *rles = NULL; int nr = 0;

int find_rle(const char *n) {
    for(int i=0;i<nr;i++) if(!strcmp(rles[i].name,n)) return i;
    return -1;
}

int main() {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"STORE")) {
            sscanf(line,"%*s %s",arg1);
            char *p=line+6+strlen(arg1); while(*p==' ')p++;
            int i=find_rle(arg1);
            if(i!=-1) { free(rles[i].enc); rles[i].enc=strdup(p); }
            else { rles=realloc(rles,sizeof(RLE)*(nr+1)); rles[nr].name=strdup(arg1); rles[nr].enc=strdup(p); nr++; }
        } else if(!strcmp(cmd,"DECODE")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_rle(arg1);
            if(i!=-1) {
                char *p=rles[i].enc;
                while(*p) {
                    int count=0;
                    while(*p>='0'&&*p<='9') { count=count*10+(*p-'0'); p++; if(count>1000000) { count=-1; break; } }
                    if(count==-1 || !*p) break;
                    for(int j=0;j<count;j++) putchar(*p==' '?'_':*p);
                    p++;
                }
                printf("\n");
            }
        } else if(!strcmp(cmd,"DELETE")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_rle(arg1);
            if(i!=-1) {
                free(rles[i].name); free(rles[i].enc);
                for(int j=i;j<nr-1;j++) rles[j]=rles[j+1];
                nr--;
            }
        } else if(!strcmp(cmd,"LIST")) {
            for(int i=0;i<nr;i++) printf("%s\n",rles[i].name);
        }
    }
    return 0;
}