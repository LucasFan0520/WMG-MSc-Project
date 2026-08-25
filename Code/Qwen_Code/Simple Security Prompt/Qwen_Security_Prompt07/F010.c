// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *val; } Val;
typedef struct { char *key; Val *vals; int n, cap; } Key;
Key *keys = 0; int nk = 0, ck = 0;

int find_key(char *key) {
    for (int i=0; i<nk; i++) if (!strcmp(keys[i].key, key)) return i;
    return -1;
}

int main() {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[256];
        b[0] = 0;
        sscanf(line, "%s %s %s", cmd, a, b);
        if (!strcmp(cmd, "ADD")) {
            int i = find_key(a);
            if (i < 0) {
                if (nk == ck) { ck = ck ? ck*2 : 4; keys = realloc(keys, ck*sizeof(Key)); }
                i = nk++; keys[i].key = strdup(a); keys[i].vals = 0; keys[i].n = 0; keys[i].cap = 0;
            }
            if (keys[i].n == keys[i].cap) {
                keys[i].cap = keys[i].cap ? keys[i].cap*2 : 4;
                keys[i].vals = realloc(keys[i].vals, keys[i].cap*sizeof(Val));
            }
            keys[i].vals[keys[i].n++].val = strdup(b);
        } else if (!strcmp(cmd, "REMOVE")) {
            int i = find_key(a);
            if (i >= 0) {
                for (int j=0; j<keys[i].n; j++) {
                    if (!strcmp(keys[i].vals[j].val, b)) {
                        free(keys[i].vals[j].val);
                        for (int k=j; k<keys[i].n-1; k++) keys[i].vals[k] = keys[i].vals[k+1];
                        keys[i].n--; break;
                    }
                }
            }
        } else if (!strcmp(cmd, "ERASE")) {
            int i = find_key(a);
            if (i >= 0) {
                for (int j=0; j<keys[i].n; j++) free(keys[i].vals[j].val);
                free(keys[i].vals); free(keys[i].key);
                for (int j=i; j<nk-1; j++) keys[j] = keys[j+1]; nk--;
            }
        } else if (!strcmp(cmd, "VALUES")) {
            int i = find_key(a);
            if (i >= 0) for (int j=0; j<keys[i].n; j++) puts(keys[i].vals[j].val);
        } else if (!strcmp(cmd, "KEYS")) {
            for (int i=0; i<nk; i++) puts(keys[i].key);
        }
    }
    return 0;
}