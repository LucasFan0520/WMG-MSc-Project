// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char **vals; int cnt; } Idx;
Idx *idx = NULL;
int n = 0;

int find_key(const char *k) {
    for (int i = 0; i < n; i++) if (strcmp(idx[i].key, k) == 0) return i;
    return -1;
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[20], k[256], v[256];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            sscanf(line, "%*s %s %s", k, v);
            int i = find_key(k);
            if (i == -1) { idx = realloc(idx, (n + 1) * sizeof(Idx)); idx[n].key = strdup(k); idx[n].vals = NULL; idx[n].cnt = 0; i = n++; }
            idx[i].vals = realloc(idx[i].vals, (idx[i].cnt + 1) * sizeof(char*));
            idx[i].vals[idx[i].cnt++] = strdup(v);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            sscanf(line, "%*s %s %s", k, v);
            int i = find_key(k);
            if (i != -1) {
                for (int j = 0; j < idx[i].cnt; j++) {
                    if (strcmp(idx[i].vals[j], v) == 0) {
                        free(idx[i].vals[j]);
                        for (int x = j; x < idx[i].cnt - 1; x++) idx[i].vals[x] = idx[i].vals[x + 1];
                        idx[i].cnt--; break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            sscanf(line, "%*s %s", k);
            int i = find_key(k);
            if (i != -1) {
                for (int j = 0; j < idx[i].cnt; j++) free(idx[i].vals[j]);
                free(idx[i].vals); free(idx[i].key);
                for (int j = i; j < n - 1; j++) idx[j] = idx[j + 1];
                n--;
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            sscanf(line, "%*s %s", k);
            int i = find_key(k);
            if (i != -1) for (int j = 0; j < idx[i].cnt; j++) printf("%s\n", idx[i].vals[j]);
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (int i = 0; i < n; i++) printf("%s\n", idx[i].key);
        }
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < idx[i].cnt; j++) free(idx[i].vals[j]);
        free(idx[i].vals); free(idx[i].key);
    }
    free(idx);
    return 0;
}