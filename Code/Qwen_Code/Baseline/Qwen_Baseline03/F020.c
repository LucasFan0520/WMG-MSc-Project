// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char title[100]; char body[2000]; } C;
C arr[1000]; int n=0;
int find(char *t) { for(int i=0;i<n;i++) if(!strcmp(arr[i].title,t)) return i; return -1; }

void load(char *fname) {
    FILE *f=fopen(fname,"r"); if(!f) return;
    char line[1000];
    while(fgets(line,1000,f)) {
        line[strcspn(line,"\n")]=0;
        strcpy(arr[n].title,line); arr[n].body[0]=0;
        while(fgets(line,1000,f)) {
            line[strcspn(line,"\n")]=0;
            if(strcmp(line,"--")==0) break;
            if(arr[n].body[0]) strcat(arr[n].body,"\n");
            strcat(arr[n].body,line);
        }
        n++;
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if(argc>1) load(argv[1]);
    char cmd[20], title[100], line[1000];
    while(scanf("%s", cmd)==1 && strcmp(cmd,"END")) {
        if(!strcmp(cmd,"FIND")) {
            scanf("%s", title); int i=find(title);
            if(i!=-1) printf("%s\n%s\n",arr[i].title,arr[i].body);
        } else if(!strcmp(cmd,"DELETE")) {
            scanf("%s", title); int i=find(title);
            if(i!=-1) { for(int j=i;j<n-1;j++) arr[j]=arr[j+1]; n--; }
        } else if(!strcmp(cmd,"ADD")) {
            scanf("%s", title); strcpy(arr[n].title,title); arr[n].body[0]=0;
            while(fgets(line,1000,stdin)) {
                line[strcspn(line,"\n")]=0;
                if(strcmp(line,"--")==0) break;
                if(arr[n].body[0]) strcat(arr[n].body,"\n");
                strcat(arr[n].body,line);
            }
            n++;
        } else if(!strcmp(cmd,"PRINT")) {
            for(int i=0;i<n;i++) printf("%s\n%s\n--\n",arr[i].title,arr[i].body);
        }
    }
    return 0;
}