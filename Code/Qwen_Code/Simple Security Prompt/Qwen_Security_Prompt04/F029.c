// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *val;
} Rec;

Rec recs[10000];
int rc = 0;

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char key[1000], val[1000];
            sscanf(rest, "%s %s", key, val);
            recs[rc].key = strdup(key);
            recs[rc].val = strdup(val);
            rc++;
        } else if (strcmp(cmd, "DEDUP") == 0) {
            for (int i = 0; i < rc; i++) {
                for (int j = i + 1; j < rc; j++) {
                    if (strcmp(recs[i].key, recs[j].key) == 0) {
                        free(recs[j].key);
                        free(recs[j].val);
                        for (int k = j; k < rc - 1; k++) recs[k] = recs[k+1];
                        rc--;
                        j--;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            for (int i = 0; i < rc; i++) {
                if (strcmp(recs[i].key, rest) == 0) {
                    free(recs[i].key);
                    free(recs[i].val);
                    for (int j = i; j < rc - 1; j++) recs[j] = recs[j+1];
                    rc--;
                    i--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            for (int i = 0; i < rc; i++) {
                if (strcmp(recs[i].key, rest) == 0) {
                    printf("%s\n", recs[i].val);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rc; i++) {
                printf("%s %s\n", recs[i].key, recs[i].val);
            }
        }
    }
    for (int i = 0; i < rc; i++) {
        free(recs[i].key);
        free(recs[i].val);
    }
    return 0;
}