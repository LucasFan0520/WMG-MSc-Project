// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char item[100]; int count; } I;
I arr[1000]; int n=0;
int find(char *item) { for(int i=0;i<n;i++) if(!strcmp(arr[i].item,item)) return i; return -1; }

int main() {
    char cmd[20], item[100]; int c;
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"ADD")) {
            scanf("%s %d", item, &c);
            int i=find(item);
            if(i!=-1) arr[i].count+=c;
            else { strcpy(arr[n].item,item); arr[n].count=c; n++; }
        } else if(!strcmp(cmd,"TAKE")) {
            scanf("%s %d", item, &c);
            int i=find(item);
            if(i!=-1) { arr[i].count-=c; if(arr[i].count<0) arr[i].count=0; }
        } else if(!strcmp(cmd,"REMOVE")) {
            scanf("%s", item); int i=find(item);
            if(i!=-1) { for(int j=i;j<n-1;j++) arr[j]=arr[j+1]; n--; }
        } else if(!strcmp(cmd,"COUNT")) {
            scanf("%s", item); int i=find(item);
            printf("%d\n", i!=-1 ? arr[i].count : 0);
        } else if(!strcmp(cmd,"REPORT")) {
            for(int i=0;i<n;i++) printf("%s %d\n",arr[i].item,arr[i].count);
        }
    }
    return 0;
}