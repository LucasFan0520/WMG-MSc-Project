// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key, *val; } V;
V *vs = NULL; int nv = 0;

int main() {
    char ln[4096];
    while(fgets(ln,4096,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20];
        sscanf(ln,"%s",cmd);
        char *p1 = strchr(ln,' '); if(p1) p1++;
        if(!strcmp(cmd,"SET")) { char k[256]; sscanf(p1,"%s",k); char *v=p1+strlen(k)+1; int i; for(i=0;i<nv;i++) if(!strcmp(vs[i].key,k)) break; if(i<nv) { free(vs[i].val); vs[i].val=strdup(v); } else { vs=realloc(vs,(nv+1)*sizeof(V)); vs[nv].key=strdup(k); vs[nv].val=strdup(v); nv++; } }
        else if(!strcmp(cmd,"UNSET")) { char k[256]; sscanf(p1,"%s",k); for(int i=0;i<nv;i++) if(!strcmp(vs[i].key,k)) { free(vs[i].key); free(vs[i].val); for(int j=i;j<nv-1;j++) vs[j]=vs[j+1]; nv--; break; } }
        else if(!strcmp(cmd,"RENDER")) { char out[4096]=""; int oi=0; for(char *p=p1;*p;) { if(*p=='{' && strchr(p,'}')) { char *e=strchr(p,'}'); char k[256]; strncpy(k,p+1,e-p-1); k[e-p-1]=0; int f=0; for(int i=0;i<nv;i++) if(!strcmp(vs[i].key,k)) { int l=strlen(vs[i].val); memcpy(out+oi,vs[i].val,l); oi+=l; f=1; break; } if(!f) { out[oi++]='{'; memcpy(out+oi,k,strlen(k)); oi+=strlen(k); out[oi++]='}'; } p=e+1; } else { out[oi++]=*p++; } } out[oi]=0; printf("%s\n",out); }
        else if(!strcmp(cmd,"VARS")) { for(int i=0;i<nv;i++) printf("%s=%s\n",vs[i].key,vs[i].val); }
    }
    for(int i=0;i<nv;i++) { free(vs[i].key); free(vs[i].val); } free(vs);
    return 0;
}