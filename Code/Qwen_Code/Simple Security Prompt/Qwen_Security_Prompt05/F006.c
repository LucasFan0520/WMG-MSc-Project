// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *item; int count; } I;
I *is = NULL; int ni = 0;

int main() {
    char ln[1024];
    while(fgets(ln,1024,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20], item[256]; int c;
        sscanf(ln,"%s %s %d",cmd,item,&c);
        int i; for(i=0;i<ni;i++) if(!strcmp(is[i].item,item)) break;
        if(!strcmp(cmd,"ADD")) { if(i<ni) is[i].count+=c; else { is=realloc(is,(ni+1)*sizeof(I)); is[ni].item=strdup(item); is[ni].count=c; ni++; } }
        else if(!strcmp(cmd,"TAKE")) { if(i<ni) { is[i].count-=c; if(is[i].count<0) is[i].count=0; } }
        else if(!strcmp(cmd,"REMOVE")) { if(i<ni) { free(is[i].item); for(int j=i;j<ni-1;j++) is[j]=is[j+1]; ni--; } }
        else if(!strcmp(cmd,"COUNT")) { printf("%d\n",i<ni?is[i].count:0); }
        else if(!strcmp(cmd,"REPORT")) { for(int j=0;j<ni;j++) printf("%s %d\n",is[j].item,is[j].count); }
    }
    for(int i=0;i<ni;i++) free(is[i].item); free(is);
    return 0;
}