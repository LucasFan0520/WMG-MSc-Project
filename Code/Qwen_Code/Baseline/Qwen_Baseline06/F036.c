// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *data; } Rec;
Rec *recs; int n_rec;

int find_rec(char *name) {
    for(int i=0; i<n_rec; i++) if(!strcmp(recs[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], name[100], raw[2000];
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "ADD")) {
            scanf("%s %s", name, raw);
            char *colon = strchr(raw, ':');
            if(colon) {
                *colon = 0;
                int len = atoi(raw);
                char *data = colon + 1;
                if((int)strlen(data) == len) {
                    recs = realloc(recs, (n_rec+1)*sizeof(Rec));
                    recs[n_rec].name = strdup(name);
                    recs[n_rec].data = strdup(data);
                    n_rec++;
                }
            }
        } else if(!strcmp(cmd, "DELETE")) {
            scanf("%s", name);
            int idx = find_rec(name);
            if(idx != -1) {
                free(recs[idx].name); free(recs[idx].data);
                for(int k=idx; k<n_rec-1; k++) recs[k] = recs[k+1];
                n_rec--;
            }
        } else if(!strcmp(cmd, "SHOW")) {
            scanf("%s", name);
            int idx = find_rec(name);
            if(idx != -1) {
                for(char *p = recs[idx].data; *p; p++) {
                    putchar(*p == ' ' ? '_' : *p);
                }
                putchar('\n');
            }
        } else if(!strcmp(cmd, "REPORT")) {
            for(int i=0; i<n_rec; i++) printf("%s\n", recs[i].name);
        }
    }
    return 0;
}