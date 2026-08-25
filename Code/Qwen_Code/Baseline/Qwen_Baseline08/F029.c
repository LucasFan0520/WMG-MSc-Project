// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REC 10000

typedef struct {
    char key[100];
    char value[100];
} Rec;

Rec recs[MAX_REC];
int rcount = 0;

int main() {
    char cmd[20], key[100], val[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s", key, val);
            if (rcount < MAX_REC) {
                strcpy(recs[rcount].key, key);
                strcpy(recs[rcount].value, val);
                rcount++;
            }
        } else if (strcmp(cmd, "DEDUP") == 0) {
            int new_count = 0;
            for (int i = 0; i < rcount; i++) {
                int found = 0;
                for (int j = 0; j < new_count; j++) {
                    if (strcmp(recs[j].key, recs[i].key) == 0) {
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    recs[new_count++] = recs[i];
                }
            }
            rcount = new_count;
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", key);
            for (int i = 0; i < rcount; ) {
                if (strcmp(recs[i].key, key) == 0) {
                    for (int j = i; j < rcount - 1; j++) recs[j] = recs[j + 1];
                    rcount--;
                } else {
                    i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", key);
            for (int i = 0; i < rcount; i++) {
                if (strcmp(recs[i].key, key) == 0) {
                    printf("%s\n", recs[i].value);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (int i = 0; i < rcount; i++) {
                printf("%s %s\n", recs[i].key, recs[i].value);
            }
        }
    }
    return 0;
}