// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *title, *body; } C;
C *a; int n;

int find(char *t) {
    for(int i=0; i<n; i++) if(!strcmp(a[i].title, t)) return i;
    return -1;
}

void load(char *fname) {
    FILE *f = fopen(fname, "r");
    if(!f) return;
    char line[1000];
    while(fgets(line, 1000, f)) {
        line[strcspn(line, "\n")] = 0;
        if(strcmp(line, "--")==0) continue;
        a = realloc(a, (n+1)*sizeof(C));
        a[n].title=strdup(line); a[n].body=strdup("");
        while(fgets(line, 1000, f)) {
            line[strcspn(line, "\n")] = 0;
            if(strcmp(line, "--")==0) break;
            char *nb = malloc(strlen(a[n].body)+strlen(line)+2);
            sprintf(nb, "%s%s%s", a[n].body, a[n].body[0]?"\n":"", line);
            free(a[n].body); a[n].body=nb;
        }
        n++;
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if(argc>1) load(argv[1]);
    char c[20], t[1000], line[1000];
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "FIND")) {
            fgets(t, 1000, stdin);
            while(t[0]==' ') memmove(t, t+1, strlen(t));
            t[strcspn(t, "\n")] = 0;
            int x = find(t);
            if(x!=-1) printf("%s\n%s\n", a[x].title, a[x].body);
        } else if(!strcmp(c, "DELETE")) {
            fgets(t, 1000, stdin);
            while(t[0]==' ') memmove(t, t+1, strlen(t));
            t[strcspn(t, "\n")] = 0;
            int x = find(t);
            if(x!=-1) {
                free(a[x].title); free(a[x].body);
                for(int i=x; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "ADD")) {
            fgets(t, 1000, stdin);
            while(t[0]==' ') memmove(t, t+1, strlen(t));
            t[strcspn(t, "\n")] = 0;
            a = realloc(a, (n+1)*sizeof(C));
            a[n].title=strdup(t); a[n].body=strdup("");
            while(fgets(line, 1000, stdin)) {
                line[strcspn(line, "\n")] = 0;
                if(strcmp(line, "--")==0) break;
                char *nb = malloc(strlen(a[n].body)+strlen(line)+2);
                sprintf(nb, "%s%s%s", a[n].body, a[n].body[0]?"\n":"", line);
                free(a[n].body); a[n].body=nb;
            }
            n++;
        } else if(!strcmp(c, "PRINT")) {
            for(int i=0; i<n; i++) printf("%s\n%s\n--\n", a[i].title, a[i].body);
        }
    }
    return 0;
}