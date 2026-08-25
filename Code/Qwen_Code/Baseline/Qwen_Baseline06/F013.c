// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title, *note; } T;
T *todo, *doing, *done; int nt, nd, ndn;

int find(T *arr, int sz, char *t) {
    for(int i=0; i<sz; i++) if(!strcmp(arr[i].title, t)) return i;
    return -1;
}

void move(T **src, int *ss, T **dst, int *ds, char *t) {
    int x = find(*src, *ss, t);
    if(x!=-1) {
        *dst = realloc(*dst, (*ds+1)*sizeof(T));
        (*dst)[*ds] = (*src)[x]; (*ds)++;
        for(int i=x; i<*ss-1; i++) (*src)[i]=(*src)[i+1];
        (*ss)--;
    }
}

void remove_any(char *t) {
    int x;
    if((x=find(todo, nt, t))!=-1) { free(todo[x].title); free(todo[x].note); for(int i=x;i<nt-1;i++) todo[i]=todo[i+1]; nt--; }
    else if((x=find(doing, nd, t))!=-1) { free(doing[x].title); free(doing[x].note); for(int i=x;i<nd-1;i++) doing[i]=doing[i+1]; nd--; }
    else if((x=find(done, ndn, t))!=-1) { free(done[x].title); free(done[x].note); for(int i=x;i<ndn-1;i++) done[i]=done[i+1]; ndn--; }
}

int main() {
    char c[20], t[100], note[1000];
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "TODO")) {
            scanf("%s", t); fgets(note, 1000, stdin); while(note[0]==' ') memmove(note, note+1, strlen(note)); note[strcspn(note, "\n")]=0;
            todo = realloc(todo, (nt+1)*sizeof(T)); todo[nt].title=strdup(t); todo[nt].note=strdup(note); nt++;
        } else if(!strcmp(c, "START")) { scanf("%s", t); move(&todo, &nt, &doing, &nd, t); }
        else if(!strcmp(c, "DONE")) { scanf("%s", t); move(&doing, &nd, &done, &ndn, t); }
        else if(!strcmp(c, "REMOVE")) { scanf("%s", t); remove_any(t); }
        else if(!strcmp(c, "FIND")) {
            scanf("%s", t); int x;
            if((x=find(todo,nt,t))!=-1) printf("todo %s\n", todo[x].note);
            else if((x=find(doing,nd,t))!=-1) printf("doing %s\n", doing[x].note);
            else if((x=find(done,ndn,t))!=-1) printf("done %s\n", done[x].note);
        } else if(!strcmp(c, "REPORT")) {
            for(int i=0;i<nt;i++) printf("todo %s %s\n", todo[i].title, todo[i].note);
            for(int i=0;i<nd;i++) printf("doing %s %s\n", doing[i].title, doing[i].note);
            for(int i=0;i<ndn;i++) printf("done %s %s\n", done[i].title, done[i].note);
        }
    }
    return 0;
}