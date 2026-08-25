// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title, *body; } D;
D *a, *trash; int n, tn;

int find(char *t) {
    for(int i=0; i<n; i++) if(!strcmp(a[i].title, t)) return i;
    return -1;
}

int main() {
    char c[20], t[100], b[1000];
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "CREATE")) {
            scanf("%s", t); fgets(b, 1000, stdin);
            while(b[0]==' ') memmove(b, b+1, strlen(b));
            b[strcspn(b, "\n")] = 0;
            a = realloc(a, (n+1)*sizeof(D));
            a[n].title=strdup(t); a[n].body=strdup(b); n++;
        } else if(!strcmp(c, "REVISE")) {
            scanf("%s", t); fgets(b, 1000, stdin);
            while(b[0]==' ') memmove(b, b+1, strlen(b));
            b[strcspn(b, "\n")] = 0;
            int x = find(t);
            if(x!=-1) { free(a[x].body); a[x].body=strdup(b); }
        } else if(!strcmp(c, "DISCARD")) {
            scanf("%s", t); int x = find(t);
            if(x!=-1) {
                trash = realloc(trash, (tn+1)*sizeof(D));
                trash[tn].title=a[x].title; trash[tn].body=a[x].body; tn++;
                for(int i=x; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "RESTORE")) {
            if(tn>0) {
                tn--;
                a = realloc(a, (n+1)*sizeof(D));
                a[n].title=trash[tn].title; a[n].body=trash[tn].body; n++;
            }
        } else if(!strcmp(c, "FIND")) {
            scanf("%s", t); int x = find(t);
            if(x!=-1) printf("%s\n", a[x].body);
        } else if(!strcmp(c, "PRINT")) {
            for(int i=0; i<n; i++) printf("%s %s\n", a[i].title, a[i].body);
        }
    }
    return 0;
}