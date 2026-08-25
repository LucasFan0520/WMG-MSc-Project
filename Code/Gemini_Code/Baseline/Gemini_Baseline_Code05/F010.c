// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char **values;
    int val_count;
    int val_capacity;
} KeyIndex;

KeyIndex *index_table = NULL;
int key_count = 0;
int key_capacity = 0;

int find_key(const char *key) {
    for (int i = 0; i < key_count; i++) {
        if (strcmp(index_table[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

int add_key(const char *key) {
    int idx = find_key(key);
    if (idx != -1) {
        return idx;
    }
    if (key_count >= key_capacity) {
        key_capacity = key_capacity == 0 ? 4 : key_capacity * 2;
        index_table = realloc(index_table, key_capacity * sizeof(KeyIndex));
    }
    index_table[key_count].key = strdup(key);
    index_table[key_count].values = NULL;
    index_table[key_count].val_count = 0;
    index_table[key_count].val_capacity = 0;
    key_count++;
    return key_count - 1;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p1 = line + 4;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                int k_idx = add_key(p1);
                KeyIndex *ki = &index_table[k_idx];
                if (ki->val_count >= ki->val_capacity) {
                    ki->val_capacity = ki->val_capacity == 0 ? 4 : ki->val_capacity * 2;
                    ki->values = realloc(ki->values, ki->val_capacity * sizeof(char *));
                }
                ki->values[ki->val_count] = strdup(p2 + 1);
                ki->val_count++;
            }
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *p1 = line + 7;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                int k_idx = find_key(p1);
                if (k_idx != -1) {
                    KeyIndex *ki = &index_table[k_idx];
                    for (int i = 0; i < ki->val_count; i++) {
                        if (strcmp(ki->values[i], p2 + 1) == 0) {
                            free(ki->values[i]);
                            for (int j = i; j < ki->val_count - 1; j++) {
                                ki->values[j] = ki->values[j + 1];
                            }
                            ki->val_count--;
                            break;
                        }
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *key = line + 6;
            int idx = find_key(key);
            if (idx != -1) {
                free(index_table[idx].key);
                for (int i = 0; i < index_table[idx].val_count; i++) {
                    free(index_table[idx].values[i]);
                }
                free(index_table[idx].values);
                for (int i = idx; i < key_count - 1; i++) {
                    index_table[i] = index_table[i + 1];
                }
                key_count--;
            }
        } else if (strncmp(line, "VALUES ", 7) == 0) {
            char *key = line + 7;
            int idx = find_key(key);
            if (idx != -1) {
                for (int i = 0; i < index_table[idx].val_count; i++) {
                    printf("%s\n", index_table[idx].values[i]);
                }
            }
        } else if (strcmp(line, "KEYS") == 0) {
            for (int i = 0; i < key_count; i++) {
                printf("%s\n", index_table[i].key);
            }
        }
    }
    for (int i = 0; i < key_count; i++) {
        free(index_table[i].key);
        for (int j = 0; j < index_table[i].val_count; j++) {
            free(index_table[i].values[j]);
        }
        free(index_table[i].values);
    }
    free(index_table);
    return 0;
}
