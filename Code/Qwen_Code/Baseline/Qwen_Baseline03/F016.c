// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char name[100]; char email[100]; char phone[100]; } C;
C arr[1000]; int n=0;
int find(char *name) { for(int i=0;i<n;i++) if(!strcmp(arr[i].name,name)) return i; return -1; }

void load(char *fname) {
    FILE *f=fopen(fname,"r"); if(!f) return;
    char line[300];
    while(fgets(line,300,f)) {
        line[strcspn(line,"\n")]=0;
        char *t1=strtok(line,","), *t2=strtok(NULL,","), *t3=strtok(NULL,",");
        if(t1 && t2 && t3) { strcpy(arr[n].name,t1); strcpy(arr[n].email,t2); strcpy(arr[n].phone,t3); n++; }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if(argc>1) load(argv[1]);
    char cmd[20], name[100], line[300];
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"ADD")) {
            scanf("%s", line);
            char *t1=strtok(line,","), *t2=strtok(NULL,","), *t3=strtok(NULL,",");
            if(t1 && t2 && t3) { strcpy(arr[n].name,t1); strcpy(arr[n].email,t2); strcpy(arr[n].phone,t3); n++; }
        } else if(!strcmp(cmd,"DELETE")) {
            scanf("%s", name); int i=find(name);
            if(i!=-1) { for(int j=i;j<n-1;j++) arr[j]=arr[j+1]; n--; }
        } else if(!strcmp(cmd,"FIND")) {
            scanf("%s", name); int i=find(name);
            if(i!=-1) printf("%s,%s,%s\n",arr[i].name,arr[i].email,arr[i].phone);
        } else if(!strcmp(cmd,"REPORT")) {
            for(int i=0;i<n;i++) printf("%s,%s,%s\n",arr[i].name,arr[i].email,arr[i].phone);
        }
    }
    return 0;
}