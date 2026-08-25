// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *encoded; } Rle;
Rle *rles; int n_rle;

int find_rle(char *name) {
    for(int i=0; i<n_rle; i++) if(!strcmp(rles[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], encoded[2000];
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "STORE")) {
            scanf("%s %s", name, encoded);
            rles = realloc(rles, (n_rle+1)*sizeof(Rle));
            rles[n_rle].name = strdup(name);
            rles[n_rle].encoded = strdup(encoded);
            n_rle++;
        } else if(!strcmp(cmd, "DECODE")) {
            scanf("%s", name);
            int idx = find_rle(name);
            if(idx != -1) {
                char *p = rles[idx].encoded;
                while(*p) {
                    int count = 0;
                    while(*p >= '0' && *p <= '9') {
                        count = count * 10 + (*p - '0');
                        p++;
                        if(count > 100000) { count = -1; break; }
                    }
                    if(count > 0 && *p) {
                        for(int i=0; i<count; i++) {
                            putchar(*p == ' ' ? '_' : *p);
                        }
                        p++;
                    } else {
                        break;
                    }
                }
                putchar('\n');
            }
        } else if(!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            int idx = find_rle(name);
            if(idx != -1) {
                free(rles[idx].name); free(rles[idx].encoded);
                for(int k=idx; k<n_rle-1; k++) rles[k] = rles[k+1];
                n_rle--;
            }
        } else if(!strcmp(cmd, "LIST")) {
            for(int i=0; i<n_rle; i++) printf("%s\n", rles[i].name);
        }
    }
    return 0;
}