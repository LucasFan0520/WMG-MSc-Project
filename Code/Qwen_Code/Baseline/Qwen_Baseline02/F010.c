// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char **vals; int nv; } Idx;
Idx idx[1000]; int ni = 0;

int find(char *key) {
    for (int i = 0; i < ni; i++) if (!strcmp(idx[i].key, key)) return i;
    return -1;
}

int main() {
    char cmd[20], key[100], val[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s %s", key, val);
            int i = find(key);
            if (i == -1) {
                idx[ni].key = strdup(key); idx[ni].vals = malloc(sizeof(char*));
                idx[ni].vals[0] = strdup(val); idx[ni++].nv = 1;
            } else {
                idx[i].vals = realloc(idx[i].vals, sizeof(char*) * (idx[i].nv + 1));
                idx[i].vals[idx[i].nv++] = strdup(val);
            }
        } else if (!strcmp(cmd, "REMOVE")) {
            scanf("%s %s", key, val); int i = find(key);
            if (i != -1) {
                for (int j = 0; j < idx[i].nv; j++) {
                    if (!strcmp(idx[i].vals[j], val)) {
                        free(idx[i].vals[j]);
                        for (int k = j; k < idx[i].nv - 1; k++) idx[i].vals[k] = idx[i].vals[k + 1];
                        idx[i].nv--; break;
                    }
                }
            }
        } else if (!strcmp(cmd, "ERASE")) {
            scanf("%s", key); int i = find(key);
            if (i != -1) {
                free(idx[i].key);
                for (int j = 0; j < idx[i].nv; j++) free(idx[i].vals[j]);
                free(idx[i].vals);
                for (int j = i; j < ni - 1; j++) idx[j] = idx[j + 1];
                ni--;
            }
        } else if (!strcmp(cmd, "VALUES")) {
            scanf("%s", key); int i = find(key);
            if (i != -1) for (int j = 0; j < idx[i].nv; j++) printf("%s\n", idx[i].vals[j]);
        } else if (!strcmp(cmd, "KEYS")) {
            for (int i = 0; i < ni; i++) printf("%s\n", idx[i].key);
        }
    }
    for (int i = 0; i < ni; i++) {
        free(idx[i].key);
        for (int j = 0; j < idx[i].nv; j++) free(idx[i].vals[j]);
        free(idx[i].vals);
    }
    return 0;
}