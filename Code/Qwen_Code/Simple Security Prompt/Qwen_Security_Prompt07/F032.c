// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key, *val; } Set;
Set *sets = 0; int n = 0, cap = 0;
Set *staged = 0; int sn = 0, scap = 0;
int in_trans = 0;

int find(Set *arr, int sz, char *key) {
    for (int i = 0; i < sz; i++) if (!strcmp(arr[i].key, key)) return i;
    return -1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], rest[2048];
        a[0] = rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "SET") || !strcmp(cmd, "TSET") || !strcmp(cmd, "GET")) sscanf(line, "%*s %s %[^\n]", a, rest);

        if (!strcmp(cmd, "SET")) {
            int idx = find(sets, n, a);
            if (idx >= 0) { free(sets[idx].val); sets[idx].val = strdup(rest); }
            else {
                if (n == cap) { cap = cap ? cap * 2 : 4; sets = realloc(sets, cap * sizeof(Set)); }
                sets[n].key = strdup(a); sets[n].val = strdup(rest); n++;
            }
        } else if (!strcmp(cmd, "BEGIN")) {
            in_trans = 1; sn = 0;
        } else if (!strcmp(cmd, "TSET")) {
            if (in_trans) {
                int idx = find(staged, sn, a);
                if (idx >= 0) { free(staged[idx].val); staged[idx].val = strdup(rest); }
                else {
                    if (sn == scap) { scap = scap ? scap * 2 : 4; staged = realloc(staged, scap * sizeof(Set)); }
                    staged[sn].key = strdup(a); staged[sn].val = strdup(rest); sn++;
                }
            }
        } else if (!strcmp(cmd, "COMMIT")) {
            if (in_trans) {
                for (int i = 0; i < sn; i++) {
                    int idx = find(sets, n, staged[i].key);
                    if (idx >= 0) { free(sets[idx].val); sets[idx].val = staged[i].val; free(staged[i].key); }
                    else {
                        if (n == cap) { cap = cap ? cap * 2 : 4; sets = realloc(sets, cap * sizeof(Set)); }
                        sets[n++] = staged[i];
                    }
                }
                free(staged); staged = 0; sn = 0; scap = 0; in_trans = 0;
            }
        } else if (!strcmp(cmd, "ABORT")) {
            if (in_trans) {
                for (int i = 0; i < sn; i++) { free(staged[i].key); free(staged[i].val); }
                free(staged); staged = 0; sn = 0; scap = 0; in_trans = 0;
            }
        } else if (!strcmp(cmd, "GET")) {
            int idx = find(sets, n, a);
            if (idx >= 0) puts(sets[idx].val);
        } else if (!strcmp(cmd, "DUMP")) {
            for (int i = 0; i < n; i++) printf("%s %s\n", sets[i].key, sets[i].val);
        }
    }
    return 0;
}