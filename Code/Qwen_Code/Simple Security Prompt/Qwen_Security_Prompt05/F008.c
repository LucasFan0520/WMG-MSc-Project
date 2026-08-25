// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *note; int score; } S;
S *ss = NULL; int ns = 0;

int main() {
    char ln[1024];
    while(fgets(ln,1024,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20], name[256], note[1024]=""; int sc;
        sscanf(ln,"%s %s %d",cmd,name,&sc);
        char *p=strstr(ln,name); if(p) { p+=strlen(name); while(*p==' ') p++; sscanf(p,"%d",&sc); p=strchr(p,' '); if(p) { p++; strcpy(note,p); } }
        int i; for(i=0;i<ns;i++) if(!strcmp(ss[i].name,name)) break;
        if(!strcmp(cmd,"ADD")) { if(i==ns) { ss=realloc(ss,(ns+1)*sizeof(S)); ss[ns].name=strdup(name); ss[ns].score=sc; ss[ns].note=strdup(note); ns++; } }
        else if(!strcmp(cmd,"UPDATE")) { if(i<ns) { ss[i].score=sc; free(ss[i].note); ss[i].note=strdup(note); } }
        else if(!strcmp(cmd,"DROP")) { if(i<ns) { free(ss[i].name); free(ss[i].note); for(int j=i;j<ns-1;j++) ss[j]=ss[j+1]; ns--; } }
        else if(!strcmp(cmd,"AVERAGE")) { int sum=0; for(int j=0;j<ns;j++) sum+=ss[j].score; printf("%d\n",ns?sum/ns:0); }
        else if(!strcmp(cmd,"LIST")) { for(int j=0;j<ns;j++) printf("%s %d %s\n",ss[j].name,ss[j].score,ss[j].note); }
    }
    for(int i=0;i<ns;i++) { free(ss[i].name); free(ss[i].note); } free(ss);
    return 0;
}