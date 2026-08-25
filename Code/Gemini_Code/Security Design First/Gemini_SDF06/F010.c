// F010.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char **values;
    size_t val_count;
    size_t val_capacity;
} KeyIndex;

int main(void) {
    KeyIndex *index_arr = NULL;
    size_t key_count = 0;
    size_t key_capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "REMOVE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *value = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*key == '\0' || *value == '\0') continue;

            int found_idx = -1;
            for (size_t i = 0; i < key_count; i++) {
                if (strcmp(index_arr[i].key, key) == 0) {
                    found_idx = (int)i;
                    break;
                }
            }

            if (strcmp(cmd, "ADD") == 0) {
                if (found_idx == -1) {
                    if (key_count >= key_capacity) {
                        size_t new_cap = key_capacity == 0 ? 4 : key_capacity * 2;
                        KeyIndex *new_index = realloc(index_arr, new_cap * sizeof(KeyIndex));
                        if (!new_index) continue;
                        index_arr = new_index;
                        key_capacity = new_cap;
                    }
                    char *k = strdup(key);
                    if (k) {
                        index_arr[key_count].key = k;
                        index_arr[key_count].values = NULL;
                        index_arr[key_count].val_count = 0;
                        index_arr[key_count].val_capacity = 0;
                        found_idx = (int)key_count;
                        key_count++;
                    }
                }
                if (found_idx != -1) {
                    KeyIndex *ki = &index_arr[found_idx];
                    if (ki->val_count >= ki->val_capacity) {
                        size_t new_cap = ki->val_capacity == 0 ? 4 : ki->val_capacity * 2;
                        char **new_vals = realloc(ki->values, new_cap * sizeof(char *));
                        if (!new_vals) continue;
                        ki->values = new_vals;
                        ki->val_capacity = new_cap;
                    }
                    char *v = strdup(value);
                    if (v) {
                        ki->values[ki->val_count] = v;
                        ki->val_count++;
                    }
                }
            } else {
                if (found_idx != -1) {
                    KeyIndex *ki = &index_arr[found_idx];
                    for (size_t i = 0; i < ki->val_count; i++) {
                        if (strcmp(ki->values[i], value) == 0) {
                            free(ki->values[i]);
                            for (size_t j = i; j < ki->val_count - 1; j++) {
                                ki->values[j] = ki->values[j + 1];
                            }
                            ki->val_count--;
                            break;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*key == '\0') continue;

            for (size_t i = 0; i < key_count; i++) {
                if (strcmp(index_arr[i].key, key) == 0) {
                    free(index_arr[i].key);
                    for (size_t j = 0; j < index_arr[i].val_count; j++) {
                        free(index_arr[i].values[j]);
                    }
                    free(index_arr[i].values);
                    for (size_t j = i; j < key_count - 1; j++) {
                        index_arr[j] = index_arr[j + 1];
                    }
                    key_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *key = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*key == '\0') continue;

            for (size_t i = 0; i < key_count; i++) {
                if (strcmp(index_arr[i].key, key) == 0) {
                    for (size_t j = 0; j < index_arr[i].val_count; j++) {
                        printf("%s%s", index_arr[i].values[j], (j == index_arr[i].val_count - 1) ? "" : " ");
                    }
                    break;
                }
            }
            printf("\n");
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (size_t i = 0; i < key_count; i++) {
                printf("%s\n", index_arr[i].key);
            }
        }
    }

    for (size_t i = 0; i < key_count; i++) {
        free(index_arr[i].key);
        for (size_t j = 0; j < index_arr[i].val_count; j++) {
            free(index_arr[i].values[j]);
        }
        free(index_arr[i].values);
    }
    free(index_arr);
    free(line);
    return 0;
}
