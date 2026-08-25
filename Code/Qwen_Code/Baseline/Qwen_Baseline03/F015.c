// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char name[100]; char email[100]; char phone[100]; char note[1000]; } C;
C arr[1000]; int n=0;
int find(char *name) { for(int i=0;i<n;i++) if(!strcmp(arr[i].name,name)) return i; return -1; }

int main() {
    char cmd[20], n1[100], n2[100], email[100], phone[100], note[1000];
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"ADD")) {
            scanf("%s %s %s", n1, email, phone);
            strcpy(arr[n].name,n1); strcpy(arr[n].email,email); strcpy(arr[n].phone,phone); arr[n].note[0]=0; n++;
        } else if(!strcmp(cmd,"NOTE")) {
            scanf("%s", n1); fgets(note,1000,stdin); note[strcspn(note,"\n")]=0;
            if(note[0]==' ') memmove(note,note+1,strlen(note));
            int i=find(n1); if(i!=-1) strcpy(arr[i].note,note);
        } else if(!strcmp(cmd,"MERGE")) {
            scanf("%s %s", n1, n2);
            int i1=find(n1), i2=find(n2);
            if(i1!=-1 && i2!=-1) {
                if(arr[i1].note[0] && arr[i2].note[0]) { strcat(arr[i1].note," "); strcat(arr[i1].note,arr[i2].note); }
                else if(arr[i2].note[0]) strcpy(arr[i1].note,arr[i2].note);
                for(int j=i2;j<n-1;j++) arr[j]=arr[j+1]; n--;
            }
        } else if(!strcmp(cmd,"DELETE")) {
            scanf("%s", n1); int i=find(n1);
            if(i!=-1) { for(int j=i;j<n-1;j++) arr[j]=arr[j+1]; n--; }
        } else if(!strcmp(cmd,"FIND")) {
            scanf("%s", n1); int i=find(n1);
            if(i!=-1) printf("%s %s %s %s\n",arr[i].name,arr[i].email,arr[i].phone,arr[i].note);
        } else if(!strcmp(cmd,"LIST")) {
            for(int i=0;i<n;i++) printf("%s %s %s %s\n",arr[i].name,arr[i].email,arr[i].phone,arr[i].note);
        }
    }
    return 0;
}