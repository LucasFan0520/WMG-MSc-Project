// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char **values;
    size_t val_count;
    size_t val_capacity;
} KeyList;

int main(void) {
    KeyList *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "ADD") == 0) {
            char *key = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *value = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (found == -1) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    KeyList *new_items = realloc(items, capacity * sizeof(KeyList));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].key = strdup(key);
                if (!items[count].key) exit(1);
                items[count].values = NULL;
                items[count].val_count = 0;
                items[count].val_capacity = 0;
                found = (int)count;
                count++;
            }

            KeyList *kl = &items[found];
            if (kl->val_count >= kl->val_capacity) {
                kl->val_capacity = kl->val_capacity == 0 ? 4 : kl->val_capacity * 2;
                char **nv = realloc(kl->values, kl->val_capacity * sizeof(char *));
                if (!nv) exit(1);
                kl->values = nv;
            }
            kl->values[kl->val_count] = strdup(value);
            if (!kl->values[kl->val_count]) exit(1);
            kl->val_count++;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *key = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *value = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    KeyList *kl = &items[i];
                    for (size_t j = 0; j < kl->val_count; j++) {
                        if (strcmp(kl->values[j], value) == 0) {
                            free(kl->values[j]);
                            for (size_t k = j; k < kl->val_count - 1; k++) {
                                kl->values[k] = kl->values[k + 1];
                            }
                            kl->val_count--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *key = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    free(items[i].key);
                    for (size_t j = 0; j < items[i].val_count; j++) {
                        free(items[i].values[j]);
                    }
                    free(items[i].values);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].key, key) == 0) {
                    for (size_t j = 0; j < items[i].val_count; j++) {
                        printf("%s%s", items[i].values[j], (j == items[i].val_count - 1) ? "" : " ");
                    }
                    break;
                }
            }
            putchar('\n');
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s\n", items[i].key);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].key);
        for (size_t j = 0; j < items[i].val_count; j++) {
            free(items[i].values[j]);
        }
        free(items[i].values);
    }
    free(items);
    free(line);
    return 0;
}
