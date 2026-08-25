// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *item; int count; } I;
I *a; int n;

int find(char *it) {
    for(int i=0; i<n; i++) if(!strcmp(a[i].item, it)) return i;
    return -1;
}

int main() {
    char c[20], it[100]; int cnt;
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "ADD")) {
            scanf("%s %d", it, &cnt);
            int x = find(it);
            if(x!=-1) a[x].count += cnt;
            else { a = realloc(a, (n+1)*sizeof(I)); a[n].item=strdup(it); a[n].count=cnt; n++; }
        } else if(!strcmp(c, "TAKE")) {
            scanf("%s %d", it, &cnt);
            int x = find(it);
            if(x!=-1) { a[x].count -= cnt; if(a[x].count<0) a[x].count=0; }
        } else if(!strcmp(c, "REMOVE")) {
            scanf("%s", it); int x = find(it);
            if(x!=-1) {
                free(a[x].item);
                for(int i=x; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "COUNT")) {
            scanf("%s", it); int x = find(it);
            printf("%d\n", x!=-1 ? a[x].count : 0);
        } else if(!strcmp(c, "REPORT")) {
            for(int i=0; i<n; i++) printf("%s %d\n", a[i].item, a[i].count);
        }
    }
    return 0;
}