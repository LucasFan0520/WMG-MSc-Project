// F040.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id; int hlen; int plen; char *header; char *payload; } Pkt;
Pkt *pkts = NULL; int np = 0;

int find_pkt(const char *id) {
    for(int i=0;i<np;i++) if(!strcmp(pkts[i].id,id)) return i;
    return -1;
}

int main() {
    char line[10000];
    while(fgets(line, sizeof(line), stdin)) {
        line[strcspn(line,"\n")]=0;
        if(!strcmp(line,"END")) break;
        char cmd[20], arg1[1000];
        if(sscanf(line,"%s",cmd)!=1) continue;
        if(!strcmp(cmd,"PACKET")) {
            int hl,pl; sscanf(line,"%*s %s %d %d",arg1,&hl,&pl);
            char *p=line;
            while(*p && *p!=' ') p++; while(*p==' ') p++;
            while(*p && *p!=' ') p++; while(*p==' ') p++;
            while(*p && *p!=' ') p++; while(*p==' ') p++;
            while(*p && *p!=' ') p++; while(*p==' ') p++;
            char *header=p;
            while(*p && *p!=' ') p++;
            if(*p) { *p=0; p++; while(*p==' ') p++; }
            char *payload=p;
            if((int)strlen(header)==hl && (int)strlen(payload)==pl) {
                pkts=realloc(pkts,sizeof(Pkt)*(np+1));
                pkts[np].id=strdup(arg1); pkts[np].hlen=hl; pkts[np].plen=pl;
                pkts[np].header=strdup(header); pkts[np].payload=strdup(payload);
                np++;
            }
        } else if(!strcmp(cmd,"DROP")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_pkt(arg1);
            if(i!=-1) {
                free(pkts[i].id); free(pkts[i].header); free(pkts[i].payload);
                for(int j=i;j<np-1;j++) pkts[j]=pkts[j+1];
                np--;
            }
        } else if(!strcmp(cmd,"HEADER")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_pkt(arg1);
            if(i!=-1) printf("%s\n",pkts[i].header);
        } else if(!strcmp(cmd,"PAYLOAD")) {
            sscanf(line,"%*s %s",arg1);
            int i=find_pkt(arg1);
            if(i!=-1) printf("%s\n",pkts[i].payload);
        } else if(!strcmp(cmd,"STATS")) {
            int total=0;
            for(int i=0;i<np;i++) total+=pkts[i].plen;
            printf("%d\n",total);
        }
    }
    return 0;
}