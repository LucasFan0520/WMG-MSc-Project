// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char **vals;
    int vcount;
} Entry;

Entry *entries = NULL;
int ecount = 0;

int find_key(const char *key) {
    for (int i = 0; i < ecount; i++) {
        if (strcmp(entries[i].key, key) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char key[100], val[1000];
            scanf("%s", key);
            scanf(" %[^\n]", val);
            int idx = find_key(key);
            if (idx == -1) {
                entries = realloc(entries, sizeof(Entry) * (ecount + 1));
                entries[ecount].key = strdup(key);
                entries[ecount].vals = NULL;
                entries[ecount].vcount = 0;
                idx = ecount++;
            }
            entries[idx].vals = realloc(entries[idx].vals, sizeof(char*) * (entries[idx].vcount + 1));
            entries[idx].vals[entries[idx].vcount++] = strdup(val);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char key[100], val[1000];
            scanf("%s", key);
            scanf(" %[^\n]", val);
            int idx = find_key(key);
            if (idx != -1) {
                for (int i = 0; i < entries[idx].vcount; i++) {
                    if (strcmp(entries[idx].vals[i], val) == 0) {
                        free(entries[idx].vals[i]);
                        for (int j = i; j < entries[idx].vcount - 1; j++) {
                            entries[idx].vals[j] = entries[idx].vals[j+1];
                        }
                        entries[idx].vcount--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char key[100];
            scanf("%s", key);
            int idx = find_key(key);
            if (idx != -1) {
                free(entries[idx].key);
                for (int i = 0; i < entries[idx].vcount; i++) free(entries[idx].vals[i]);
                free(entries[idx].vals);
                for (int j = idx; j < ecount - 1; j++) entries[j] = entries[j+1];
                ecount--;
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char key[100];
            scanf("%s", key);
            int idx = find_key(key);
            if (idx != -1) {
                for (int i = 0; i < entries[idx].vcount; i++) {
                    printf("%s\n", entries[idx].vals[i]);
                }
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (int i = 0; i < ecount; i++) {
                printf("%s\n", entries[i].key);
            }
        }
    }
    for (int i = 0; i < ecount; i++) {
        free(entries[i].key);
        for (int j = 0; j < entries[i].vcount; j++) free(entries[i].vals[j]);
        free(entries[i].vals);
    }
    free(entries);
    return 0;
}