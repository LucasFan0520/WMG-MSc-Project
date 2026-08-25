// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key, *val; } KV;
KV *a; int n;

int find(char *k) {
    for(int i=0; i<n; i++) if(!strcmp(a[i].key, k)) return i;
    return -1;
}

void load(char *fname) {
    FILE *f = fopen(fname, "r");
    if(!f) return;
    char line[1000];
    while(fgets(line, 1000, f)) {
        line[strcspn(line, "\n")] = 0;
        char *eq = strchr(line, '=');
        if(eq) {
            *eq = 0;
            a = realloc(a, (n+1)*sizeof(KV));
            a[n].key=strdup(line); a[n].val=strdup(eq+1); n++;
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if(argc>1) load(argv[1]);
    char c[20], k[100], v[1000];
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "SET")) {
            scanf("%s", k); fgets(v, 1000, stdin);
            while(v[0]==' ') memmove(v, v+1, strlen(v));
            v[strcspn(v, "\n")] = 0;
            int x = find(k);
            if(x!=-1) { free(a[x].val); a[x].val=strdup(v); }
            else { a = realloc(a, (n+1)*sizeof(KV)); a[n].key=strdup(k); a[n].val=strdup(v); n++; }
        } else if(!strcmp(c, "UNSET")) {
            scanf("%s", k); int x = find(k);
            if(x!=-1) {
                free(a[x].key); free(a[x].val);
                for(int i=x; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "GET")) {
            scanf("%s", k); int x = find(k);
            if(x!=-1) printf("%s\n", a[x].val);
        } else if(!strcmp(c, "WRITE")) {
            for(int i=0; i<n; i++) printf("%s=%s\n", a[i].key, a[i].val);
        }
    }
    return 0;
}