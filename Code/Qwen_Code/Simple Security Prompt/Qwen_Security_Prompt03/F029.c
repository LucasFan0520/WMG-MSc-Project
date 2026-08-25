// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char *value; } Rec;
Rec *recs = NULL;
int nr = 0;

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], a[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s", a);
            char *t = strchr(line, ' '); t++; t = strchr(t, ' '); t++;
            recs = realloc(recs, (nr + 1) * sizeof(Rec));
            recs[nr].key = strdup(a);
            recs[nr].value = strdup(t);
            nr++;
        } else if (strcmp(cmd, "DEDUP") == 0) {
            for (int i = 0; i < nr; ) {
                int dup = 0;
                for (int j = 0; j < i; j++) {
                    if (strcmp(recs[i].key, recs[j].key) == 0) {
                        dup = 1;
                        break;
                    }
                }
                if (dup) {
                    free(recs[i].key);
                    free(recs[i].value);
                    for (int j = i; j < nr - 1; j++) recs[j] = recs[j + 1];
                    nr--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nr; ) {
                if (strcmp(recs[i].key, a) == 0) {
                    free(recs[i].key);
                    free(recs[i].value);
                    for (int j = i; j < nr - 1; j++) recs[j] = recs[j + 1];
                    nr--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            sscanf(line, "%*s %s", a);
            for (int i = 0; i < nr; i++) {
                if (strcmp(recs[i].key, a) == 0) {
                    printf("%s\n", recs[i].value);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < nr; i++) {
                printf("%s %s\n", recs[i].key, recs[i].value);
            }
        }
    }
    for (int i = 0; i < nr; i++) {
        free(recs[i].key);
        free(recs[i].value);
    }
    free(recs);
    return 0;
}