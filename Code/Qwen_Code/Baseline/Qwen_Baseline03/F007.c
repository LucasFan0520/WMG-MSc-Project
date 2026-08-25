// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { int time; char label[100]; char note[1000]; } A;
A arr[1000]; int n=0;
int cmp(const void *a, const void *b) {
    A *x=(A*)a, *y=(A*)b;
    if(x->time!=y->time) return x->time-y->time;
    return strcmp(x->label,y->label);
}

int main() {
    char cmd[20], label[100], note[1000]; int t;
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"ADD")) {
            scanf("%d %s", &t, label); fgets(note,1000,stdin); note[strcspn(note,"\n")]=0;
            if(note[0]==' ') memmove(note,note+1,strlen(note));
            arr[n].time=t; strcpy(arr[n].label,label); strcpy(arr[n].note,note); n++;
        } else if(!strcmp(cmd,"DELETE")) {
            scanf("%d %s", &t, label);
            for(int i=0;i<n;) {
                if(arr[i].time==t && !strcmp(arr[i].label,label)) {
                    for(int j=i;j<n-1;j++) arr[j]=arr[j+1]; n--;
                } else i++;
            }
        } else if(!strcmp(cmd,"FIND")) {
            scanf("%s", label);
            int min_t=2000000000, idx=-1;
            for(int i=0;i<n;i++) if(!strcmp(arr[i].label,label) && arr[i].time<min_t) { min_t=arr[i].time; idx=i; }
            if(idx!=-1) printf("%d %s %s\n",arr[idx].time,arr[idx].label,arr[idx].note);
        } else if(!strcmp(cmd,"SORT")) {
            qsort(arr,n,sizeof(A),cmp);
            for(int i=0;i<n;i++) printf("%d %s %s\n",arr[i].time,arr[i].label,arr[i].note);
        } else if(!strcmp(cmd,"CLEAR")) {
            n=0;
        }
    }
    return 0;
}