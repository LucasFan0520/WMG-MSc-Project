// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *id, *text; } P;
P *a; int n;

void add(char *i, char *t) {
    a = realloc(a, (n+1)*sizeof(P));
    a[n].id = strdup(i); a[n].text = strdup(t); n++;
}

int find(char *i) {
    for(int j=0; j<n; j++) if(!strcmp(a[j].id, i)) return j;
    return -1;
}

void read_text(char *t) {
    fgets(t, 1000, stdin);
    while(t[0]==' ') memmove(t, t+1, strlen(t));
    t[strcspn(t, "\n")] = 0;
}

int main() {
    char c[20], i[100], t[1000];
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "NEW")) {
            scanf("%s", i); read_text(t); add(i, t);
        } else if(!strcmp(c, "APPEND")) {
            scanf("%s", i); read_text(t);
            int x = find(i);
            if(x!=-1) {
                char *nt = malloc(strlen(a[x].text)+strlen(t)+2);
                sprintf(nt, "%s %s", a[x].text, t);
                free(a[x].text); a[x].text = nt;
            }
        } else if(!strcmp(c, "REPLACE")) {
            scanf("%s", i); read_text(t);
            int x = find(i);
            if(x!=-1) { free(a[x].text); a[x].text = strdup(t); }
        } else if(!strcmp(c, "DELETE")) {
            scanf("%s", i); int x = find(i);
            if(x!=-1) {
                free(a[x].id); free(a[x].text);
                for(int j=x; j<n-1; j++) a[j]=a[j+1];
                n--;
            }
        } else if(!strcmp(c, "SHOW")) {
            scanf("%s", i); int x = find(i);
            if(x!=-1) {
                for(char *p=a[x].text; *p; p++) putchar(*p==' '?'_':*p);
                putchar('\n');
            } else printf("NOT_FOUND\n");
        } else if(!strcmp(c, "LIST")) {
            for(int j=0; j<n; j++) printf("%s\n", a[j].id);
        }
    }
    return 0;
}