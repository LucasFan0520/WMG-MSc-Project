// F010.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char **values;
    int val_count;
    int val_capacity;
} IndexEntry;

IndexEntry *index_table = NULL;
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

void add_kv(const char *key, const char *val) {
    int idx = find_key(key);
    if (idx == -1) {
        if (key_count >= key_capacity) {
            key_capacity = key_capacity == 0 ? 4 : key_capacity * 2;
            index_table = realloc(index_table, key_capacity * sizeof(IndexEntry));
        }
        index_table[key_count].key = strdup(key);
        index_table[key_count].values = NULL;
        index_table[key_count].val_count = 0;
        index_table[key_count].val_capacity = 0;
        idx = key_count;
        key_count++;
    }
    IndexEntry *e = &index_table[idx];
    if (e->val_count >= e->val_capacity) {
        e->val_capacity = e->val_capacity == 0 ? 4 : e->val_capacity * 2;
        e->values = realloc(e->values, e->val_capacity * sizeof(char *));
    }
    e->values[e->val_count] = strdup(val);
    e->val_count++;
}

void remove_kv(const char *key, const char *val) {
    int idx = find_key(key);
    if (idx != -1) {
        IndexEntry *e = &index_table[idx];
        for (int i = 0; i < e->val_count; i++) {
            if (strcmp(e->values[i], val) == 0) {
                free(e->values[i]);
                for (int j = i; j < e->val_count - 1; j++) {
                    e->values[j] = e->values[j + 1];
                }
                e->val_count--;
                break;
            }
        }
    }
}

void erase_k(const char *key) {
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
}

void print_values(const char *key) {
    int idx = find_key(key);
    if (idx != -1) {
        for (int i = 0; i < index_table[idx].val_count; i++) {
            printf("%s%s", index_table[idx].values[i], (i == index_table[idx].val_count - 1) ? "" : " ");
        }
        printf("\n");
    } else {
        printf("\n");
    }
}

void print_keys(void) {
    for (int i = 0; i < key_count; i++) {
        printf("%s\n", index_table[i].key);
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *key = strtok(NULL, " ");
            char *val = strtok(NULL, " ");
            if (key && val) add_kv(key, val);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *key = strtok(NULL, " ");
            char *val = strtok(NULL, " ");
            if (key && val) remove_kv(key, val);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *key = strtok(NULL, " ");
            if (key) erase_k(key);
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key = strtok(NULL, " ");
            if (key) print_values(key);
        } else if (strcmp(cmd, "KEYS") == 0) {
            print_keys();
        }
    }
    free(line);
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
