// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *val;
} Val;

typedef struct {
    char *key;
    Val *vals;
    int n;
} Key;

Key *keys = NULL;
int nk = 0;

int find_key(const char *k) {
    for (int i = 0; i < nk; i++)
        if (strcmp(keys[i].key, k) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000], arg2[1000];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s %s", arg1, arg2);
            int i = find_key(arg1);
            if (i == -1) {
                keys = realloc(keys, sizeof(Key) * (nk + 1));
                keys[nk].key = strdup(arg1);
                keys[nk].vals = NULL;
                keys[nk].n = 0;
                i = nk++;
            }
            keys[i].vals = realloc(keys[i].vals, sizeof(Val) * (keys[i].n + 1));
            keys[i].vals[keys[i].n++].val = strdup(arg2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            sscanf(line, "%*s %s %s", arg1, arg2);
            int i = find_key(arg1);
            if (i != -1) {
                for (int j = 0; j < keys[i].n; j++) {
                    if (strcmp(keys[i].vals[j].val, arg2) == 0) {
                        free(keys[i].vals[j].val);
                        for (int k = j; k < keys[i].n - 1; k++) keys[i].vals[k] = keys[i].vals[k + 1];
                        keys[i].n--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_key(arg1);
            if (i != -1) {
                free(keys[i].key);
                for (int j = 0; j < keys[i].n; j++) free(keys[i].vals[j].val);
                free(keys[i].vals);
                for (int j = i; j < nk - 1; j++) keys[j] = keys[j + 1];
                nk--;
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_key(arg1);
            if (i != -1) {
                for (int j = 0; j < keys[i].n; j++) printf("%s\n", keys[i].vals[j].val);
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (int i = 0; i < nk; i++) printf("%s\n", keys[i].key);
        }
    }
    for (int i = 0; i < nk; i++) {
        free(keys[i].key);
        for (int j = 0; j < keys[i].n; j++) free(keys[i].vals[j].val);
        free(keys[i].vals);
    }
    free(keys);
    return 0;
}