// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *val;
} Value;

typedef struct {
    char *key;
    Value *vals;
    int count;
    int cap;
} Key;

Key *keys = NULL;
int total = 0;

int find_key(const char *key) {
    for (int i = 0; i < total; i++) {
        if (strcmp(keys[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

void add_val(int idx, const char *val) {
    if (keys[idx].count == keys[idx].cap) {
        keys[idx].cap = keys[idx].cap ? keys[idx].cap * 2 : 4;
        keys[idx].vals = realloc(keys[idx].vals, sizeof(Value) * keys[idx].cap);
    }
    keys[idx].vals[keys[idx].count++].val = strdup(val);
}

int main() {
    char cmd[20];
    char key[1000];
    char val[1000];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s", key, val);
            int idx = find_key(key);
            if (idx == -1) {
                keys = realloc(keys, sizeof(Key) * (total + 1));
                keys[total].key = strdup(key);
                keys[total].vals = NULL;
                keys[total].count = 0;
                keys[total].cap = 0;
                idx = total++;
            }
            add_val(idx, val);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s %s", key, val);
            int idx = find_key(key);
            if (idx != -1) {
                for (int i = 0; i < keys[idx].count; i++) {
                    if (strcmp(keys[idx].vals[i].val, val) == 0) {
                        free(keys[idx].vals[i].val);
                        for (int j = i; j < keys[idx].count - 1; j++) {
                            keys[idx].vals[j] = keys[idx].vals[j + 1];
                        }
                        keys[idx].count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            scanf("%s", key);
            int idx = find_key(key);
            if (idx != -1) {
                free(keys[idx].key);
                for (int i = 0; i < keys[idx].count; i++) {
                    free(keys[idx].vals[i].val);
                }
                free(keys[idx].vals);
                for (int i = idx; i < total - 1; i++) {
                    keys[i] = keys[i + 1];
                }
                total--;
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            scanf("%s", key);
            int idx = find_key(key);
            if (idx != -1) {
                for (int i = 0; i < keys[idx].count; i++) {
                    printf("%s\n", keys[idx].vals[i].val);
                }
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (int i = 0; i < total; i++) {
                printf("%s\n", keys[i].key);
            }
        }
    }

    for (int i = 0; i < total; i++) {
        free(keys[i].key);
        for (int j = 0; j < keys[i].count; j++) {
            free(keys[i].vals[j].val);
        }
        free(keys[i].vals);
    }
    free(keys);

    return 0;
}