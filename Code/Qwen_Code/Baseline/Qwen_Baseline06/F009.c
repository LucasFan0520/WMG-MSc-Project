// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *desc; } E;
typedef struct { char *group; E *events; int count; } G;
G *a; int n;

int find(char *g) {
    for(int i=0; i<n; i++) if(!strcmp(a[i].group, g)) return i;
    return -1;
}

int main() {
    char c[20], g1[100], g2[100], g3[100], desc[1000];
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "EVENT")) {
            scanf("%s", g1); fgets(desc, 1000, stdin);
            while(desc[0]==' ') memmove(desc, desc+1, strlen(desc));
            desc[strcspn(desc, "\n")] = 0;
            int x = find(g1);
            if(x==-1) {
                a = realloc(a, (n+1)*sizeof(G));
                x = n; a[n].group=strdup(g1); a[n].events=NULL; a[n].count=0; n++;
            }
            a[x].events = realloc(a[x].events, (a[x].count+1)*sizeof(E));
            a[x].events[a[x].count++].desc = strdup(desc);
        } else if(!strcmp(c, "MERGE")) {
            scanf("%s %s %s", g1, g2, g3);
            int i1 = find(g1), i2 = find(g2);
            int x = find(g3);
            if(x==-1) {
                a = realloc(a, (n+1)*sizeof(G));
                x = n; a[n].group=strdup(g3); a[n].events=NULL; a[n].count=0; n++;
            }
            if(i1!=-1) for(int i=0; i<a[i1].count; i++) {
                a[x].events = realloc(a[x].events, (a[x].count+1)*sizeof(E));
                a[x].events[a[x].count++].desc = strdup(a[i1].events[i].desc);
            }
            if(i2!=-1) for(int i=0; i<a[i2].count; i++) {
                a[x].events = realloc(a[x].events, (a[x].count+1)*sizeof(E));
                a[x].events[a[x].count++].desc = strdup(a[i2].events[i].desc);
            }
        } else if(!strcmp(c, "DELETEGROUP")) {
            scanf("%s", g1); int x = find(g1);
            if(x!=-1) {
                free(a[x].group);
                for(int i=0; i<a[x].count; i++) free(a[x].events[i].desc);
                free(a[x].events);
                for(int i=x; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "COUNT")) {
            scanf("%s", g1); int x = find(g1);
            printf("%d\n", x!=-1 ? a[x].count : 0);
        } else if(!strcmp(c, "REPORT")) {
            for(int i=0; i<n; i++) printf("%s %d\n", a[i].group, a[i].count);
        }
    }
    return 0;
}