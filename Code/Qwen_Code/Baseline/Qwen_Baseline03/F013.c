// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char title[100]; char note[1000]; } T;
T todo[1000], doing[1000], done[1000];
int tn=0, dn=0, don=0;
int find(T *arr, int n, char *t) { for(int i=0;i<n;i++) if(!strcmp(arr[i].title,t)) return i; return -1; }
void move(T *src, int *sn, T *dst, int *dn, char *t) {
    int i=find(src,*sn,t);
    if(i!=-1) { strcpy(dst[*dn].title,src[i].title); strcpy(dst[*dn].note,src[i].note); (*dn)++;
        for(int j=i;j<*sn-1;j++) src[j]=src[j+1]; (*sn)--; }
}

int main() {
    char cmd[20], title[100], note[1000];
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"TODO")) {
            scanf("%s", title); fgets(note,1000,stdin); note[strcspn(note,"\n")]=0;
            if(note[0]==' ') memmove(note,note+1,strlen(note));
            strcpy(todo[tn].title,title); strcpy(todo[tn].note,note); tn++;
        } else if(!strcmp(cmd,"START")) {
            scanf("%s", title); move(todo,&tn,doing,&dn,title);
        } else if(!strcmp(cmd,"DONE")) {
            scanf("%s", title); move(doing,&dn,done,&don,title);
        } else if(!strcmp(cmd,"REMOVE")) {
            scanf("%s", title);
            int i;
            if((i=find(todo,tn,title))!=-1) { for(int j=i;j<tn-1;j++) todo[j]=todo[j+1]; tn--; }
            else if((i=find(doing,dn,title))!=-1) { for(int j=i;j<dn-1;j++) doing[j]=doing[j+1]; dn--; }
            else if((i=find(done,don,title))!=-1) { for(int j=i;j<don-1;j++) done[j]=done[j+1]; don--; }
        } else if(!strcmp(cmd,"FIND")) {
            scanf("%s", title);
            int i;
            if((i=find(todo,tn,title))!=-1) printf("todo %s\n",todo[i].note);
            else if((i=find(doing,dn,title))!=-1) printf("doing %s\n",doing[i].note);
            else if((i=find(done,don,title))!=-1) printf("done %s\n",done[i].note);
        } else if(!strcmp(cmd,"REPORT")) {
            printf("todo:"); for(int i=0;i<tn;i++) printf(" %s",todo[i].title); printf("\n");
            printf("doing:"); for(int i=0;i<dn;i++) printf(" %s",doing[i].title); printf("\n");
            printf("done:"); for(int i=0;i<don;i++) printf(" %s",done[i].title); printf("\n");
        }
    }
    return 0;
}