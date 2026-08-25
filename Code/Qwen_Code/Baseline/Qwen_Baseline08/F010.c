// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEYS 1000
#define MAX_VALS 10000

typedef struct {
    char key[100];
    char vals[MAX_VALS][100];
    int vcount;
} KeyNode;

KeyNode keys[MAX_KEYS];
int kcount = 0;

int find_key(const char *key) {
    for (int i = 0; i < kcount; i++) if (strcmp(keys[i].key, key) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], k[100], v[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s", k, v);
            int idx = find_key(k);
            if (idx == -1 && kcount < MAX_KEYS) {
                strcpy(keys[kcount].key, k);
                keys[kcount].vcount = 0;
                idx = kcount++;
            }
            if (idx != -1 && keys[idx].vcount < MAX_VALS) {
                strcpy(keys[idx].vals[keys[idx].vcount++], v);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s %s", k, v);
            int idx = find_key(k);
            if (idx != -1) {
                for (int i = 0; i < keys[idx].vcount; i++) {
                    if (strcmp(keys[idx].vals[i], v) == 0) {
                        for (int j = i; j < keys[idx].vcount - 1; j++)
                            strcpy(keys[idx].vals[j], keys[idx].vals[j + 1]);
                        keys[idx].vcount--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            scanf("%s", k);
            int idx = find_key(k);
            if (idx != -1) {
                for (int i = idx; i < kcount - 1; i++) keys[i] = keys[i + 1];
                kcount--;
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            scanf("%s", k);
            int idx = find_key(k);
            if (idx != -1) {
                for (int i = 0; i < keys[idx].vcount; i++) printf("%s\n", keys[idx].vals[i]);
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (int i = 0; i < kcount; i++) printf("%s\n", keys[i].key);
        }
    }
    return 0;
}