// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char **descs; int nd; } G;
typedef struct { char *name; G g; } GR;
GR *grs = NULL; int ng = 0;

int findg(char *n) { for(int i=0;i<ng;i++) if(!strcmp(grs[i].name,n)) return i; return -1; }

int main() {
    char ln[1024];
    while(fgets(ln,1024,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20], a[256], b[256], c[256];
        sscanf(ln,"%s",cmd);
        char *p1=strchr(ln,' '); if(p1) p1++;
        if(!strcmp(cmd,"EVENT")) { sscanf(p1,"%s",a); char *d=p1+strlen(a)+1; int i=findg(a); if(i<0) { grs=realloc(grs,(ng+1)*sizeof(GR)); grs[ng].name=strdup(a); grs[ng].g.descs=NULL; grs[ng].g.nd=0; i=ng++; } grs[i].g.descs=realloc(grs[i].g.descs,(grs[i].g.nd+1)*sizeof(char*)); grs[i].g.descs[grs[i].g.nd++]=strdup(d); }
        else if(!strcmp(cmd,"MERGE")) { sscanf(p1,"%s %s %s",a,b,c); int ia=findg(a), ib=findg(b); if(ia>=0 && ib>=0) { int ic=findg(c); if(ic<0) { grs=realloc(grs,(ng+1)*sizeof(GR)); grs[ng].name=strdup(c); grs[ng].g.descs=NULL; grs[ng].g.nd=0; ic=ng++; } for(int j=0;j<grs[ia].g.nd;j++) { grs[ic].g.descs=realloc(grs[ic].g.descs,(grs[ic].g.nd+1)*sizeof(char*)); grs[ic].g.descs[grs[ic].g.nd++]=strdup(grs[ia].g.descs[j]); } for(int j=0;j<grs[ib].g.nd;j++) { grs[ic].g.descs=realloc(grs[ic].g.descs,(grs[ic].g.nd+1)*sizeof(char*)); grs[ic].g.descs[grs[ic].g.nd++]=strdup(grs[ib].g.descs[j]); } } }
        else if(!strcmp(cmd,"DELETEGROUP")) { sscanf(p1,"%s",a); int i=findg(a); if(i>=0) { for(int j=0;j<grs[i].g.nd;j++) free(grs[i].g.descs[j]); free(grs[i].g.descs); free(grs[i].name); for(int j=i;j<ng-1;j++) grs[j]=grs[j+1]; ng--; } }
        else if(!strcmp(cmd,"COUNT")) { sscanf(p1,"%s",a); int i=findg(a); printf("%d\n",i>=0?grs[i].g.nd:0); }
        else if(!strcmp(cmd,"REPORT")) { for(int i=0;i<ng;i++) printf("%s\n",grs[i].name); }
    }
    for(int i=0;i<ng;i++) { for(int j=0;j<grs[i].g.nd;j++) free(grs[i].g.descs[j]); free(grs[i].g.descs); free(grs[i].name); } free(grs);
    return 0;
}