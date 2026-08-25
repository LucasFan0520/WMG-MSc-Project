// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char name[100]; int score; char note[1000]; } S;
S arr[1000]; int n=0;
int find(char *name) { for(int i=0;i<n;i++) if(!strcmp(arr[i].name,name)) return i; return -1; }

int main() {
    char cmd[20], name[100], note[1000]; int score;
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"ADD") || !strcmp(cmd,"UPDATE")) {
            scanf("%s %d", name, &score); fgets(note,1000,stdin); note[strcspn(note,"\n")]=0;
            if(note[0]==' ') memmove(note,note+1,strlen(note));
            int i=find(name);
            if(i!=-1) { arr[i].score=score; strcpy(arr[i].note,note); }
            else { strcpy(arr[n].name,name); arr[n].score=score; strcpy(arr[n].note,note); n++; }
        } else if(!strcmp(cmd,"DROP")) {
            scanf("%s", name); int i=find(name);
            if(i!=-1) { for(int j=i;j<n-1;j++) arr[j]=arr[j+1]; n--; }
        } else if(!strcmp(cmd,"AVERAGE")) {
            if(n==0) printf("0\n");
            else { int sum=0; for(int i=0;i<n;i++) sum+=arr[i].score; printf("%d\n",sum/n); }
        } else if(!strcmp(cmd,"LIST")) {
            for(int i=0;i<n;i++) printf("%s %d %s\n",arr[i].name,arr[i].score,arr[i].note);
        }
    }
    return 0;
}