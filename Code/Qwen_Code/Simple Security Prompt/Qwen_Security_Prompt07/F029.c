// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key, *val; } Rec;
Rec *recs = 0; int n = 0, cap = 0;

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[256];
        a[0] = b[0] = 0;
        sscanf(line, "%s %s %s", cmd, a, b);
        if (!strcmp(cmd, "ADD")) {
            if (n == cap) { cap = cap ? cap * 2 : 4; recs = realloc(recs, cap * sizeof(Rec)); }
            recs[n].key = strdup(a); recs[n].val = strdup(b); n++;
        } else if (!strcmp(cmd, "DEDUP")) {
            for (int i = 0; i < n; i++) {
                for (int j = i + 1; j < n; ) {
                    if (!strcmp(recs[i].key, recs[j].key)) {
                        free(recs[j].key); free(recs[j].val);
                        for (int k = j; k < n - 1; k++) recs[k] = recs[k + 1]; n--;
                    } else j++;
                }
            }
        } else if (!strcmp(cmd, "DELETE")) {
            for (int i = 0; i < n; ) {
                if (!strcmp(recs[i].key, a)) {
                    free(recs[i].key); free(recs[i].val);
                    for (int j = i; j < n - 1; j++) recs[j] = recs[j + 1]; n--;
                } else i++;
            }
        } else if (!strcmp(cmd, "FIND")) {
            for (int i = 0; i < n; i++) {
                if (!strcmp(recs[i].key, a)) puts(recs[i].val);
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (int i = 0; i < n; i++) printf("%s %s\n", recs[i].key, recs[i].val);
        }
    }
    return 0;
}