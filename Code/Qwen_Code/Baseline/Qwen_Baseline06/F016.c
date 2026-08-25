// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *email, *phone; } C;
C *a; int n;

int find(char *nm) {
    for(int i=0; i<n; i++) if(!strcmp(a[i].name, nm)) return i;
    return -1;
}

void load(char *fname) {
    FILE *f = fopen(fname, "r");
    if(!f) return;
    char line[1000];
    while(fgets(line, 1000, f)) {
        line[strcspn(line, "\n")] = 0;
        char *nm = strtok(line, ","), *em = strtok(NULL, ","), *ph = strtok(NULL, ",");
        if(nm && em && ph) {
            a = realloc(a, (n+1)*sizeof(C));
            a[n].name=strdup(nm); a[n].email=strdup(em); a[n].phone=strdup(ph); n++;
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if(argc>1) load(argv[1]);
    char c[20], nm[100], em[100], ph[100], line[1000];
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "ADD")) {
            scanf("%s", line);
            char *nm=strtok(line, ","), *em=strtok(NULL, ","), *ph=strtok(NULL, ",");
            if(nm && em && ph) {
                a = realloc(a, (n+1)*sizeof(C));
                a[n].name=strdup(nm); a[n].email=strdup(em); a[n].phone=strdup(ph); n++;
            }
        } else if(!strcmp(c, "DELETE")) {
            scanf("%s", nm); int x = find(nm);
            if(x!=-1) {
                free(a[x].name); free(a[x].email); free(a[x].phone);
                for(int i=x; i<n-1; i++) a[i]=a[i+1];
                n--;
            }
        } else if(!strcmp(c, "FIND")) {
            scanf("%s", nm); int x = find(nm);
            if(x!=-1) printf("%s,%s,%s\n", a[x].name, a[x].email, a[x].phone);
        } else if(!strcmp(c, "REPORT")) {
            for(int i=0; i<n; i++) printf("%s,%s,%s\n", a[i].name, a[i].email, a[i].phone);
        }
    }
    return 0;
}