// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *exp; } A;
A *a; int n;

int find(char *nm) {
    for(int i=0; i<n; i++) if(!strcmp(a[i].name, nm)) return i;
    return -1;
}

int main() {
    char c[20], nm[100], old[100], nw[100], exp[1000];
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "DEFINE")) {
            scanf("%s", nm); fgets(exp, 1000, stdin);
            while(exp[0]==' ') memmove(exp, exp+1, strlen(exp));
            exp[strcspn(exp, "\n")] = 0;
            a = realloc(a, (n+1)*sizeof(A));
            a[n].name = strdup(nm); a[n].exp = strdup(exp); n++;
        } else if(!strcmp(c, "EXPAND")) {
            scanf("%s", nm); int x = find(nm);
            if(x!=-1) {
                for(char *p=a[x].exp; *p; p++) putchar(*p==' '?'_':*p);
                putchar('\n');
            }
        } else if(!strcmp(c, "RENAME")) {
            scanf("%s %s", old, nw); int x = find(old);
            if(x!=-1) { free(a[x].name); a[x].name = strdup(nw); }
        } else if(!strcmp(c, "ERASE")) {
            scanf("%s", nm); int x = find(nm);
            if(x!=-1) {
                free(a[x].name); free(a[x].exp);
                for(int i=x; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "DUMP")) {
            for(int i=0; i<n; i++) printf("%s %s\n", a[i].name, a[i].exp);
        }
    }
    return 0;
}