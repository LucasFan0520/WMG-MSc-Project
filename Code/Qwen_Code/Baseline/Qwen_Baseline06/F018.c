// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *word; int count; } W;
W *a; int n;

int find(char *w) {
    for(int i=0; i<n; i++) if(!strcmp(a[i].word, w)) return i;
    return -1;
}

int cmp(const void *x, const void *y) {
    return ((W*)y)->count - ((W*)x)->count;
}

void load(char *fname) {
    FILE *f = fopen(fname, "r");
    if(!f) return;
    char line[1000];
    while(fgets(line, 1000, f)) {
        line[strcspn(line, "\n")] = 0;
        int x = find(line);
        if(x!=-1) a[x].count++;
        else { a = realloc(a, (n+1)*sizeof(W)); a[n].word=strdup(line); a[n].count=1; n++; }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if(argc>1) load(argv[1]);
    char c[20], w[1000];
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "WORD")) {
            fgets(w, 1000, stdin);
            while(w[0]==' ') memmove(w, w+1, strlen(w));
            w[strcspn(w, "\n")] = 0;
            int x = find(w);
            if(x!=-1) a[x].count++;
            else { a = realloc(a, (n+1)*sizeof(W)); a[n].word=strdup(w); a[n].count=1; n++; }
        } else if(!strcmp(c, "REMOVE")) {
            fgets(w, 1000, stdin);
            while(w[0]==' ') memmove(w, w+1, strlen(w));
            w[strcspn(w, "\n")] = 0;
            int x = find(w);
            if(x!=-1) {
                free(a[x].word);
                for(int i=x; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "COUNT")) {
            fgets(w, 1000, stdin);
            while(w[0]==' ') memmove(w, w+1, strlen(w));
            w[strcspn(w, "\n")] = 0;
            int x = find(w);
            printf("%d\n", x!=-1 ? a[x].count : 0);
        } else if(!strcmp(c, "TOP")) {
            qsort(a, n, sizeof(W), cmp);
            for(int i=0; i<n; i++) printf("%s %d\n", a[i].word, a[i].count);
        }
    }
    return 0;
}