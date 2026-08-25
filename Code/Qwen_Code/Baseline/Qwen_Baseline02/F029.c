// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char *val; } Rec;
Rec recs[10000]; int nrecs = 0;

int main() {
    char cmd[20], key[100], val[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s", key);
            fgets(val, sizeof(val), stdin);
            val[strcspn(val, "\n")] = 0;
            if (val[0] == ' ') memmove(val, val + 1, strlen(val));
            recs[nrecs].key = strdup(key);
            recs[nrecs++].val = strdup(val);
        } else if (!strcmp(cmd, "DEDUP")) {
            for (int i = 0; i < nrecs; ) {
                int dup = 0;
                for (int j = 0; j < i; j++) {
                    if (!strcmp(recs[i].key, recs[j].key)) { dup = 1; break; }
                }
                if (dup) {
                    free(recs[i].key);
                    free(recs[i].val);
                    for (int j = i; j < nrecs - 1; j++) recs[j] = recs[j + 1];
                    nrecs--;
                } else {
                    i++;
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", key);
            for (int i = 0; i < nrecs; ) {
                if (!strcmp(recs[i].key, key)) {
                    free(recs[i].key);
                    free(recs[i].val);
                    for (int j = i; j < nrecs - 1; j++) recs[j] = recs[j + 1];
                    nrecs--;
                } else {
                    i++;
                }
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", key);
            for (int i = 0; i < nrecs; i++) {
                if (!strcmp(recs[i].key, key)) {
                    printf("%s\n", recs[i].val);
                }
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < nrecs; i++) {
                printf("%s %s\n", recs[i].key, recs[i].val);
            }
        }
    }
    for (int i = 0; i < nrecs; i++) {
        free(recs[i].key);
        free(recs[i].val);
    }
    return 0;
}