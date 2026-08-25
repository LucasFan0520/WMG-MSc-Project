// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char desc[1000]; } E;
typedef struct { char name[100]; E events[1000]; int cnt; } G;
G groups[1000]; int gn=0;
int findG(char *name) { for(int i=0;i<gn;i++) if(!strcmp(groups[i].name,name)) return i; return -1; }

int main() {
    char cmd[20], g1[100], g2[100], g3[100], desc[1000];
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"EVENT")) {
            scanf("%s", g1); fgets(desc,1000,stdin); desc[strcspn(desc,"\n")]=0;
            if(desc[0]==' ') memmove(desc,desc+1,strlen(desc));
            int i=findG(g1);
            if(i==-1) { strcpy(groups[gn].name,g1); i=gn++; }
            strcpy(groups[i].events[groups[i].cnt++].desc, desc);
        } else if(!strcmp(cmd,"MERGE")) {
            scanf("%s %s %s", g1, g2, g3);
            int i1=findG(g1), i2=findG(g2);
            int i3=findG(g3);
            if(i3==-1) { strcpy(groups[gn].name,g3); i3=gn++; }
            if(i1!=-1) for(int j=0;j<groups[i1].cnt;j++) strcpy(groups[i3].events[groups[i3].cnt++].desc, groups[i1].events[j].desc);
            if(i2!=-1) for(int j=0;j<groups[i2].cnt;j++) strcpy(groups[i3].events[groups[i3].cnt++].desc, groups[i2].events[j].desc);
        } else if(!strcmp(cmd,"DELETEGROUP")) {
            scanf("%s", g1); int i=findG(g1);
            if(i!=-1) { for(int j=i;j<gn-1;j++) groups[j]=groups[j+1]; gn--; }
        } else if(!strcmp(cmd,"COUNT")) {
            scanf("%s", g1); int i=findG(g1);
            printf("%d\n", i!=-1 ? groups[i].cnt : 0);
        } else if(!strcmp(cmd,"REPORT")) {
            for(int i=0;i<gn;i++) printf("%s %d\n",groups[i].name,groups[i].cnt);
        }
    }
    return 0;
}