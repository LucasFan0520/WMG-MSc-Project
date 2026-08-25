// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char name[100]; char exp[1000]; } A;
A arr[1000]; int n=0;
int find(char *name) { for(int i=0;i<n;i++) if(!strcmp(arr[i].name,name)) return i; return -1; }

int main() {
    char cmd[20], n1[100], n2[100], exp[1000];
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"DEFINE")) {
            scanf("%s", n1); fgets(exp,1000,stdin); exp[strcspn(exp,"\n")]=0;
            if(exp[0]==' ') memmove(exp,exp+1,strlen(exp));
            int i=find(n1);
            if(i!=-1) strcpy(arr[i].exp,exp);
            else { strcpy(arr[n].name,n1); strcpy(arr[n].exp,exp); n++; }
        } else if(!strcmp(cmd,"EXPAND")) {
            scanf("%s", n1); int i=find(n1);
            if(i!=-1) { for(char *p=arr[i].exp;*p;p++) printf("%c",*p==' '?'_':*p); printf("\n"); }
        } else if(!strcmp(cmd,"RENAME")) {
            scanf("%s %s", n1, n2); int i=find(n1);
            if(i!=-1) strcpy(arr[i].name,n2);
        } else if(!strcmp(cmd,"ERASE")) {
            scanf("%s", n1); int i=find(n1);
            if(i!=-1) { for(int j=i;j<n-1;j++) arr[j]=arr[j+1]; n--; }
        } else if(!strcmp(cmd,"DUMP")) {
            for(int i=0;i<n;i++) printf("%s %s\n",arr[i].name,arr[i].exp);
        }
    }
    return 0;
}