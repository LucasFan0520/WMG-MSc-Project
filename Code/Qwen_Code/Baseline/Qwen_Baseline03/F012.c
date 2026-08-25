// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char name[100]; char note[1000]; } Q;
Q q[1000]; int n=0;
int find(char *name) { for(int i=0;i<n;i++) if(!strcmp(q[i].name,name)) return i; return -1; }

int main() {
    char cmd[20], name[100], note[1000];
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"JOIN") || !strcmp(cmd,"REJOIN")) {
            scanf("%s", name); fgets(note,1000,stdin); note[strcspn(note,"\n")]=0;
            if(note[0]==' ') memmove(note,note+1,strlen(note));
            strcpy(q[n].name,name); strcpy(q[n].note,note); n++;
        } else if(!strcmp(cmd,"SERVE")) {
            if(n>0) { printf("%s\n",q[0].name); for(int i=0;i<n-1;i++) q[i]=q[i+1]; n--; }
        } else if(!strcmp(cmd,"CANCEL")) {
            scanf("%s", name); int i=find(name);
            if(i!=-1) { for(int j=i;j<n-1;j++) q[j]=q[j+1]; n--; }
        } else if(!strcmp(cmd,"FIND")) {
            scanf("%s", name); int i=find(name);
            if(i!=-1) printf("%s\n",q[i].note);
        } else if(!strcmp(cmd,"QUEUE")) {
            for(int i=0;i<n;i++) printf("%s\n",q[i].name);
        }
    }
    return 0;
}