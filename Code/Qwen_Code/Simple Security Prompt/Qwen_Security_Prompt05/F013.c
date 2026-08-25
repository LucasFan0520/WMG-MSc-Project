// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title, *note; } T;
T *todo=NULL, *doing=NULL, *done=NULL;
int nt=0, ndoing=0, ndone=0;

int find(T *l, int n, char *t) { for(int i=0;i<n;i++) if(!strcmp(l[i].title,t)) return i; return -1; }
void add(T **l, int *n, char *t, char *ntxt) { *l=realloc(*l,(*n+1)*sizeof(T)); (*l)[*n].title=strdup(t); (*l)[*n].note=strdup(ntxt); (*n)++; }
void rem(T *l, int *n, int i) { free(l[i].title); free(l[i].note); for(int j=i;j<*n-1;j++) l[j]=l[j+1]; (*n)--; }

int main() {
    char ln[1024];
    while(fgets(ln,1024,stdin)) {
        ln[strcspn(ln,"\n")]=0;
        if(!strcmp(ln,"END")) break;
        char cmd[20], title[256];
        sscanf(ln,"%s %s",cmd,title);
        char *n=strstr(ln,title); if(n) { n+=strlen(title); if(*n) n++; } else n="";
        int i;
        if(!strcmp(cmd,"TODO")) { add(&todo,&nt,title,n); }
        else if(!strcmp(cmd,"START")) { if((i=find(todo,nt,title))>=0) { add(&doing,&ndoing,todo[i].title,todo[i].note); rem(todo,&nt,i); } }
        else if(!strcmp(cmd,"DONE")) { if((i=find(doing,ndoing,title))>=0) { add(&done,&ndone,doing[i].title,doing[i].note); rem(doing,&ndoing,i); } }
        else if(!strcmp(cmd,"REMOVE")) { if((i=find(todo,nt,title))>=0) rem(todo,&nt,i); else if((i=find(doing,ndoing,title))>=0) rem(doing,&ndoing,i); else if((i=find(done,ndone,title))>=0) rem(done,&ndone,i); }
        else if(!strcmp(cmd,"FIND")) { if((i=find(todo,nt,title))>=0) printf("todo %s\n",todo[i].note); else if((i=find(doing,ndoing,title))>=0) printf("doing %s\n",doing[i].note); else if((i=find(done,ndone,title))>=0) printf("done %s\n",done[i].note); }
        else if(!strcmp(cmd,"REPORT")) { for(int j=0;j<nt;j++) printf("todo %s\n",todo[j].title); for(int j=0;j<ndoing;j++) printf("doing %s\n",doing[j].title); for(int j=0;j<ndone;j++) printf("done %s\n",done[j].title); }
    }
    for(int i=0;i<nt;i++) { free(todo[i].title); free(todo[i].note); } free(todo);
    for(int i=0;i<ndoing;i++) { free(doing[i].title); free(doing[i].note); } free(doing);
    for(int i=0;i<ndone;i++) { free(done[i].title); free(done[i].note); } free(done);
    return 0;
}