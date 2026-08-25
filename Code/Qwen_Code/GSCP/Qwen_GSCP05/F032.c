// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Setting;

Setting *committed = NULL;
int cc = 0;
Setting *staged = NULL;
int sc = 0;
int in_transaction = 0;

int find_setting(Setting *arr, int cnt, const char *key) {
    for (int i = 0; i < cnt; i++) {
        if (strcmp(arr[i].key, key) == 0) return i;
    }
    return -1;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a1[256];
        char rest[2048];
        a1[0] = '\0';
        rest[0] = '\0';
        int n = sscanf(line, "%31s %255s %2047[^\n]", cmd, a1, rest);
        if (n < 1) continue;

        if (strcmp(cmd, "SET") == 0 && n >= 3) {
            int ci = find_setting(committed, cc, a1);
            if (ci >= 0) {
                free(committed[ci].value);
                committed[ci].value = strdup(rest);
            } else {
                committed = realloc(committed, (cc + 1) * sizeof(Setting));
                if (!committed) return 1;
                committed[cc].key = strdup(a1);
                committed[cc].value = strdup(rest);
                cc++;
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_transaction = 1;
            for (int i = 0; i < sc; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            sc = 0;
        } else if (strcmp(cmd, "TSET") == 0 && n >= 3 && in_transaction) {
            int si = find_setting(staged, sc, a1);
            if (si >= 0) {
                free(staged[si].value);
                staged[si].value = strdup(rest);
            } else {
                staged = realloc(staged, (sc + 1) * sizeof(Setting));
                if (!staged) return 1;
                staged[sc].key = strdup(a1);
                staged[sc].value = strdup(rest);
                sc++;
            }
        } else if (strcmp(cmd, "COMMIT") == 0 && in_transaction) {
            for (int i = 0; i < sc; i++) {
                int ci = find_setting(committed, cc, staged[i].key);
                if (ci >= 0) {
                    free(committed[ci].value);
                    committed[ci].value = strdup(staged[i].value);
                } else {
                    committed = realloc(committed, (cc + 1) * sizeof(Setting));
                    if (!committed) return 1;
                    committed[cc].key = strdup(staged[i].key);
                    committed[cc].value = strdup(staged[i].value);
                    cc++;
                }
                free(staged[i].key);
                free(staged[i].value);
            }
            sc = 0;
            in_transaction = 0;
        } else if (strcmp(cmd, "ABORT") == 0 && in_transaction) {
            for (int i = 0; i < sc; i++) {
                free(staged[i].key);
                free(staged[i].value);
            }
            sc = 0;
            in_transaction = 0;
        } else if (strcmp(cmd, "GET") == 0 && n >= 2) {
            int ci = find_setting(committed, cc, a1);
            if (ci >= 0) {
                printf("%s\n", committed[ci].value);
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < cc; i++) {
                printf("%s %s\n", committed[i].key, committed[i].value);
            }
        }
    }
    for (int i = 0; i < cc; i++) {
        free(committed[i].key);
        free(committed[i].value);
    }
    free(committed);
    for (int i = 0; i < sc; i++) {
        free(staged[i].key);
        free(staged[i].value);
    }
    free(staged);
    return 0;
}