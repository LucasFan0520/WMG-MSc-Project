// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key, *val; } Rec;
Rec *recs; int n_rec;

int main() {
    char cmd[20], key[100], val[100];
    while(scanf("%s", cmd)==1 && strcmp(cmd, "END")) {
        if(!strcmp(cmd, "ADD")) {
            scanf("%s %s", key, val);
            recs = realloc(recs, (n_rec+1)*sizeof(Rec));
            recs[n_rec].key = strdup(key);
            recs[n_rec].val = strdup(val);
            n_rec++;
        } else if(!strcmp(cmd, "DEDUP")) {
            for(int i=0; i<n_rec; ) {
                int dup = 0;
                for(int j=0; j<i; j++) {
                    if(!strcmp(recs[i].key, recs[j].key)) { dup = 1; break; }
                }
                if(dup) {
                    free(recs[i].key);
                    free(recs[i].val);
                    for(int k=i; k<n_rec-1; k++) recs[k] = recs[k+1];
                    n_rec--;
                } else i++;
            }
        } else if(!strcmp(cmd, "DELETE")) {
            scanf("%s", key);
            for(int i=0; i<n_rec; ) {
                if(!strcmp(recs[i].key, key)) {
                    free(recs[i].key);
                    free(recs[i].val);
                    for(int k=i; k<n_rec-1; k++) recs[k] = recs[k+1];
                    n_rec--;
                } else i++;
            }
        } else if(!strcmp(cmd, "FIND")) {
            scanf("%s", key);
            for(int i=0; i<n_rec; i++) {
                if(!strcmp(recs[i].key, key)) {
                    printf("%s\n", recs[i].val);
                }
            }
        } else if(!strcmp(cmd, "REPORT")) {
            for(int i=0; i<n_rec; i++) {
                printf("%s %s\n", recs[i].key, recs[i].val);
            }
        }
    }
    return 0;
}