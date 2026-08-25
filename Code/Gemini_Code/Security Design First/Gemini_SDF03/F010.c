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
} IndexKey;

int main(void) {
    IndexKey *arr = NULL;
    size_t count = 0;
    size_t capacity = 0;
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
        if (strncmp(line, "ADD ", 4) == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            char *val = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                val = p;
            }
            int k_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    k_idx = (int)i;
                    break;
                }
            }
            if (k_idx == -1) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    IndexKey *tmp = realloc(arr, capacity * sizeof(IndexKey));
                    if (!tmp) exit(1);
                    arr = tmp;
                }
                arr[count].key = strdup(key);
                arr[count].values = NULL;
                arr[count].val_count = 0;
                arr[count].val_capacity = 0;
                if (!arr[count].key) exit(1);
                k_idx = (int)count;
                count++;
            }
            IndexKey *k = &arr[k_idx];
            if (k->val_count >= k->val_capacity) {
                k->val_capacity = k->val_capacity == 0 ? 4 : k->val_capacity * 2;
                char **tmp = realloc(k->values, k->val_capacity * sizeof(char *));
                if (!tmp) exit(1);
                k->values = tmp;
            }
            k->values[k->val_count] = strdup(val);
            if (!k->values[k->val_count]) exit(1);
            k->val_count++;
        } else if (strncmp(line, "REMOVE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            char *val = "";
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
                val = p;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    IndexKey *k = &arr[i];
                    for (size_t j = 0; j < k->val_count; j++) {
                        if (strcmp(k->values[j], val) == 0) {
                            free(k->values[j]);
                            for (size_t m = j; m < k->val_count - 1; m++) {
                                k->values[m] = k->values[m + 1];
                            }
                            k->val_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *key = line + 6;
            while (*key == ' ') key++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    free(arr[i].key);
                    for (size_t j = 0; j < arr[i].val_count; j++) {
                        free(arr[i].values[j]);
                    }
                    free(arr[i].values);
                    for (size_t j = i; j < count - 1; j++) {
                        arr[j] = arr[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strncmp(line, "VALUES ", 7) == 0) {
            char *key = line + 7;
            while (*key == ' ') key++;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(arr[i].key, key) == 0) {
                    for (size_t j = 0; j < arr[i].val_count; j++) {
                        printf("%s ", arr[i].values[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(line, "KEYS") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", arr[i].key);
            }
        }
    }
    for (size_t i = 0; i < count; i++) {
        free(arr[i].key);
        for (size_t j = 0; j < arr[i].val_count; j++) {
            free(arr[i].values[j]);
        }
        free(arr[i].values);
    }
    free(arr);
    free(line);
    return 0;
}
