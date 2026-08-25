// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *val;
} Record;

Record *recs = NULL;
int rcount = 0;

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char key[100], val[1000];
            scanf("%s %s", key, val);
            recs = realloc(recs, sizeof(Record) * (rcount + 1));
            recs[rcount].key = strdup(key);
            recs[rcount].val = strdup(val);
            rcount++;
        } else if (strcmp(cmd, "DEDUP") == 0) {
            for (int i = 0; i < rcount; ) {
                int dup = 0;
                for (int j = 0; j < i; j++) {
                    if (strcmp(recs[i].key, recs[j].key) == 0) {
                        dup = 1;
                        break;
                    }
                }
                if (dup) {
                    free(recs[i].key);
                    free(recs[i].val);
                    for (int j = i; j < rcount - 1; j++) recs[j] = recs[j+1];
                    rcount--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char key[100];
            scanf("%s", key);
            for (int i = 0; i < rcount; ) {
                if (strcmp(recs[i].key, key) == 0) {
                    free(recs[i].key);
                    free(recs[i].val);
                    for (int j = i; j < rcount - 1; j++) recs[j] = recs[j+1];
                    rcount--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char key[100];
            scanf("%s", key);
            for (int i = 0; i < rcount; i++) {
                if (strcmp(recs[i].key, key) == 0) {
                    printf("%s\n", recs[i].val);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s %s\n", recs[i].key, recs[i].val);
            }
        }
    }
    for (int i = 0; i < rcount; i++) {
        free(recs[i].key);
        free(recs[i].val);
    }
    free(recs);
    return 0;
}