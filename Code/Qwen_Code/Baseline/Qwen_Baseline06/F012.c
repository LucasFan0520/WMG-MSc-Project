// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *note; } Q;
Q *a; int n;

int find(char *nm) {
    for(int i=0; i<n; i++) if(!strcmp(a[i].name, nm)) return i;
    return -1;
}

int main() {
    char c[20], nm[100], note[1000];
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "JOIN") || !strcmp(c, "REJOIN")) {
            scanf("%s", nm); fgets(note, 1000, stdin);
            while(note[0]==' ') memmove(note, note+1, strlen(note));
            note[strcspn(note, "\n")] = 0;
            a = realloc(a, (n+1)*sizeof(Q));
            a[n].name=strdup(nm); a[n].note=strdup(note); n++;
        } else if(!strcmp(c, "SERVE")) {
            if(n>0) {
                printf("%s\n", a[0].name);
                free(a[0].name); free(a[0].note);
                for(int i=0; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "CANCEL")) {
            scanf("%s", nm); int x = find(nm);
            if(x!=-1) {
                free(a[x].name); free(a[x].note);
                for(int i=x; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "FIND")) {
            scanf("%s", nm); int x = find(nm);
            if(x!=-1) printf("%s\n", a[x].note);
        } else if(!strcmp(c, "QUEUE")) {
            for(int i=0; i<n; i++) printf("%s\n", a[i].name);
        }
    }
    return 0;
}