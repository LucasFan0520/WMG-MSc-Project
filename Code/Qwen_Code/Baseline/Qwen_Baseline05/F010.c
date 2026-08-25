// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_KEY 100
#define MAX_VAL 100

typedef struct {
    char value[MAX_VAL];
} Value;

typedef struct {
    char key[MAX_KEY];
    Value values[1000];
    int val_count;
} KeyEntry;

KeyEntry keys[1000];
int key_count = 0;

int find_key(const char *key) {
    for (int i = 0; i < key_count; i++) {
        if (strcmp(keys[i].key, key) == 0) return i;
    }
    return -1;
}

int main() {
    char cmd[20], key[MAX_KEY], value[MAX_VAL];
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s", key, value);
            int idx = find_key(key);
            if (idx == -1) {
                strcpy(keys[key_count].key, key);
                keys[key_count].val_count = 0;
                idx = key_count++;
            }
            strcpy(keys[idx].values[keys[idx].val_count].value, value);
            keys[idx].val_count++;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s %s", key, value);
            int idx = find_key(key);
            if (idx != -1) {
                for (int i = 0; i < keys[idx].val_count; i++) {
                    if (strcmp(keys[idx].values[i].value, value) == 0) {
                        for (int j = i; j < keys[idx].val_count - 1; j++) {
                            keys[idx].values[j] = keys[idx].values[j + 1];
                        }
                        keys[idx].val_count--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            scanf("%s", key);
            int idx = find_key(key);
            if (idx != -1) {
                for (int i = idx; i < key_count - 1; i++) {
                    keys[i] = keys[i + 1];
                }
                key_count--;
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            scanf("%s", key);
            int idx = find_key(key);
            if (idx != -1) {
                for (int i = 0; i < keys[idx].val_count; i++) {
                    printf("%s\n", keys[idx].values[i].value);
                }
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (int i = 0; i < key_count; i++) {
                printf("%s\n", keys[i].key);
            }
        }
    }
    
    return 0;
}