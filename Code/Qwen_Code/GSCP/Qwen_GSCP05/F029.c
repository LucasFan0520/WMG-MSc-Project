// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Record;

Record *recs = NULL;
int rc = 0;

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[512];
        char a2[512];
        a1[0] = '\0';
        a2[0] = '\0';
        int n = sscanf(line, "%31s %511s %511[^\n]", cmd, a1, a2);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            recs = realloc(recs, (rc + 1) * sizeof(Record));
            if (!recs) return 1;
            recs[rc].key = strdup(a1);
            recs[rc].value = strdup(a2);
            rc++;
        } else if (strcmp(cmd, "DEDUP") == 0) {
            for (int i = 0; i < rc; i++) {
                for (int j = i + 1; j < rc; ) {
                    if (strcmp(recs[i].key, recs[j].key) == 0) {
                        free(recs[j].key);
                        free(recs[j].value);
                        for (int k = j; k < rc - 1; k++) {
                            recs[k] = recs[k + 1];
                        }
                        rc--;
                    } else {
                        j++;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            for (int i = 0; i < rc; ) {
                if (strcmp(recs[i].key, a1) == 0) {
                    free(recs[i].key);
                    free(recs[i].value);
                    for (int k = i; k < rc - 1; k++) {
                        recs[k] = recs[k + 1];
                    }
                    rc--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0 && n >= 2) {
            for (int i = 0; i < rc; i++) {
                if (strcmp(recs[i].key, a1) == 0) {
                    printf("%s\n", recs[i].value);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rc; i++) {
                printf("%s %s\n", recs[i].key, recs[i].value);
            }
        }
    }
    for (int i = 0; i < rc; i++) {
        free(recs[i].key);
        free(recs[i].value);
    }
    free(recs);
    return 0;
}