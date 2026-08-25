// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *val; } V;
typedef struct { char *key; V *vals; int count; } K;
K *a; int n;

int find(char *k) {
    for(int i=0; i<n; i++) if(!strcmp(a[i].key, k)) return i;
    return -1;
}

int main() {
    char c[20], k[100], v[100];
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "ADD")) {
            scanf("%s %s", k, v);
            int x = find(k);
            if(x==-1) {
                a = realloc(a, (n+1)*sizeof(K));
                x = n; a[n].key=strdup(k); a[n].vals=NULL; a[n].count=0; n++;
            }
            a[x].vals = realloc(a[x].vals, (a[x].count+1)*sizeof(V));
            a[x].vals[a[x].count++].val = strdup(v);
        } else if(!strcmp(c, "REMOVE")) {
            scanf("%s %s", k, v); int x = find(k);
            if(x!=-1) {
                for(int i=0; i<a[x].count; i++) {
                    if(!strcmp(a[x].vals[i].val, v)) {
                        free(a[x].vals[i].val);
                        for(int j=i; j<a[x].count-1; j++) a[x].vals[j]=a[x].vals[j+1];
                        a[x].count--; break;
                    }
                }
            }
        } else if(!strcmp(c, "ERASE")) {
            scanf("%s", k); int x = find(k);
            if(x!=-1) {
                free(a[x].key);
                for(int i=0; i<a[x].count; i++) free(a[x].vals[i].val);
                free(a[x].vals);
                for(int i=x; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "VALUES")) {
            scanf("%s", k); int x = find(k);
            if(x!=-1) for(int i=0; i<a[x].count; i++) printf("%s\n", a[x].vals[i].val);
        } else if(!strcmp(c, "KEYS")) {
            for(int i=0; i<n; i++) printf("%s\n", a[i].key);
        }
    }
    return 0;
}