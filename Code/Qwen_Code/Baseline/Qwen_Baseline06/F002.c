// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *tag, *msg; } M;
M *a; int n;

int find(char *t) {
    for(int i=0; i<n; i++) if(!strcmp(a[i].tag, t)) return i;
    return -1;
}

int main() {
    char c[20], t1[100], t2[100], t3[100], msg[1000];
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "ADD")) {
            scanf("%s", t1); fgets(msg, 1000, stdin);
            while(msg[0]==' ') memmove(msg, msg+1, strlen(msg));
            msg[strcspn(msg, "\n")] = 0;
            a = realloc(a, (n+1)*sizeof(M));
            a[n].tag = strdup(t1); a[n].msg = strdup(msg); n++;
        } else if(!strcmp(c, "CONCAT")) {
            scanf("%s %s %s", t1, t2, t3);
            int i1 = find(t1), i2 = find(t2);
            if(i1!=-1 && i2!=-1) {
                char *nm = malloc(strlen(a[i1].msg)+strlen(a[i2].msg)+2);
                sprintf(nm, "%s %s", a[i1].msg, a[i2].msg);
                a = realloc(a, (n+1)*sizeof(M));
                a[n].tag = strdup(t3); a[n].msg = nm; n++;
            }
        } else if(!strcmp(c, "REMOVE")) {
            scanf("%s", t1); int x = find(t1);
            if(x!=-1) {
                free(a[x].tag); free(a[x].msg);
                for(int i=x; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "FIND")) {
            scanf("%s", t1); int x = find(t1);
            if(x!=-1) {
                for(char *p=a[x].msg; *p; p++) putchar(*p==' '?'_':*p);
                putchar('\n');
            }
        } else if(!strcmp(c, "REPORT")) {
            for(int i=0; i<n; i++) printf("%s %s\n", a[i].tag, a[i].msg);
        }
    }
    return 0;
}