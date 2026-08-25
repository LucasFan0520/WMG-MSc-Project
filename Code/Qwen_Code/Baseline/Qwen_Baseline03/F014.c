// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char name[100]; char note[1000]; char labels[100][100]; int ln; } I;
I items[1000]; int n=0;
int find(char *name) { for(int i=0;i<n;i++) if(!strcmp(items[i].name,name)) return i; return -1; }

int main() {
    char cmd[20], name[100], note[1000], label[100];
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"ITEM")) {
            scanf("%s", name); fgets(note,1000,stdin); note[strcspn(note,"\n")]=0;
            if(note[0]==' ') memmove(note,note+1,strlen(note));
            strcpy(items[n].name,name); strcpy(items[n].note,note); items[n].ln=0; n++;
        } else if(!strcmp(cmd,"LABEL")) {
            scanf("%s %s", name, label); int i=find(name);
            if(i!=-1) strcpy(items[i].labels[items[i].ln++],label);
        } else if(!strcmp(cmd,"UNLABEL")) {
            scanf("%s %s", name, label); int i=find(name);
            if(i!=-1) {
                for(int j=0;j<items[i].ln;j++) {
                    if(!strcmp(items[i].labels[j],label)) {
                        for(int k=j;k<items[i].ln-1;k++) strcpy(items[i].labels[k],items[i].labels[k+1]);
                        items[i].ln--; break;
                    }
                }
            }
        } else if(!strcmp(cmd,"DELETE")) {
            scanf("%s", name); int i=find(name);
            if(i!=-1) { for(int j=i;j<n-1;j++) items[j]=items[j+1]; n--; }
        } else if(!strcmp(cmd,"FIND")) {
            scanf("%s", name); int i=find(name);
            if(i!=-1) {
                printf("%s %s",items[i].name,items[i].note);
                for(int j=0;j<items[i].ln;j++) printf(" %s",items[i].labels[j]);
                printf("\n");
            }
        } else if(!strcmp(cmd,"REPORT")) {
            for(int i=0;i<n;i++) {
                printf("%s %s",items[i].name,items[i].note);
                for(int j=0;j<items[i].ln;j++) printf(" %s",items[i].labels[j]);
                printf("\n");
            }
        }
    }
    return 0;
}