// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *acc, *memo; int amt; } T;
T *a; int n;

void load(char *fname) {
    FILE *f = fopen(fname, "r");
    if(!f) return;
    char line[1000];
    while(fgets(line, 1000, f)) {
        line[strcspn(line, "\n")] = 0;
        char acc[100], memo[1000]; int amt;
        if(sscanf(line, "%s %d", acc, &amt)==2) {
            char *m = strchr(line, ' '); m = strchr(m+1, ' ');
            if(m) {
                a = realloc(a, (n+1)*sizeof(T));
                a[n].acc=strdup(acc); a[n].amt=amt; a[n].memo=strdup(m+1); n++;
            }
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if(argc>1) load(argv[1]);
    char c[20], acc[100], memo[1000]; int amt;
    while(scanf("%s", c)==1 && strcmp(c, "END")) {
        if(!strcmp(c, "ADD")) {
            scanf("%s %d", acc, &amt); fgets(memo, 1000, stdin);
            while(memo[0]==' ') memmove(memo, memo+1, strlen(memo));
            memo[strcspn(memo, "\n")] = 0;
            a = realloc(a, (n+1)*sizeof(T));
            a[n].acc=strdup(acc); a[n].amt=amt; a[n].memo=strdup(memo); n++;
        } else if(!strcmp(c, "REVERSE")) {
            scanf("%s %d", acc, &amt);
            for(int i=0; i<n; ) {
                if(!strcmp(a[i].acc, acc) && a[i].amt==amt) {
                    free(a[i].acc); free(a[i].memo);
                    for(int j=i; j<n-1; j++) a[j]=a[j+1];
                    n--;
                } else i++;
            }
        } else if(!strcmp(c, "BALANCE")) {
            scanf("%s", acc); int sum=0;
            for(int i=0; i<n; i++) if(!strcmp(a[i].acc, acc)) sum+=a[i].amt;
            printf("%d\n", sum);
        } else if(!strcmp(c, "REPORT")) {
            for(int i=0; i<n; i++) printf("%s %d %s\n", a[i].acc, a[i].amt, a[i].memo);
        }
    }
    return 0;
}