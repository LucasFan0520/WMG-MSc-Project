// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static void *safe_malloc(size_t size) {
    void *ptr = malloc(size ? size : 1);
    if (!ptr)
        exit(EXIT_FAILURE);
    return ptr;
}
static void *safe_realloc(void *ptr, size_t size) {
    void *temp = realloc(ptr, size ? size : 1);
    if (!temp)
        exit(EXIT_FAILURE);
    return temp;
}
static char *safe_strdup(const char *s) {
    if (!s)
        return NULL;
    char *ptr = strdup(s);
    if (!ptr)
        exit(EXIT_FAILURE);
    return ptr;
}
static char *read_line(FILE *stream) {
    size_t capacity = 256, length = 0;
    char *buffer = safe_malloc(capacity);
    while (fgets(buffer + length, capacity - length, stream)) {
        length += strlen(buffer + length);
        if (length > 0 && buffer[length - 1] == '\n') {
            buffer[length - 1] = '\0';
            return buffer;
        }
        if (length == capacity - 1) {
            if (capacity > SIZE_MAX / 2) {
                free(buffer);
                return NULL;
            }
            capacity *= 2;
            buffer = safe_realloc(buffer, capacity);
        }
    }
    if (length > 0)
        return buffer;
    free(buffer);
    return NULL;
}

typedef struct {
    char *key;
    char *value;
} KV;
typedef struct {
    KV *items;
    size_t size;
    size_t capacity;
} KVList;

int main(void) {
    KVList committed = {0};
    KVList staged = {0};
    int in_transaction = 0;
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 3;
            while (*p == ' ')
                p++;
            char *key = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            KVList *target = in_transaction ? &staged : &committed;
            int found = 0;
            for (size_t i = 0; i < target->size; i++) {
                if (strcmp(target->items[i].key, key) == 0) {
                    free(target->items[i].value);
                    target->items[i].value = safe_strdup(p);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (target->size == target->capacity) {
                    target->capacity = target->capacity ? target->capacity * 2 : 4;
                    target->items = safe_realloc(target->items, target->capacity * sizeof(KV));
                }
                target->items[target->size].key = safe_strdup(key);
                target->items[target->size].value = safe_strdup(p);
                target->size++;
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_transaction = 1;
        } else if (strcmp(cmd, "TSET") == 0) {
            char *p = line + 4;
            while (*p == ' ')
                p++;
            char *key = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            int found = 0;
            for (size_t i = 0; i < staged.size; i++) {
                if (strcmp(staged.items[i].key, key) == 0) {
                    free(staged.items[i].value);
                    staged.items[i].value = safe_strdup(p);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (staged.size == staged.capacity) {
                    staged.capacity = staged.capacity ? staged.capacity * 2 : 4;
                    staged.items = safe_realloc(staged.items, staged.capacity * sizeof(KV));
                }
                staged.items[staged.size].key = safe_strdup(key);
                staged.items[staged.size].value = safe_strdup(p);
                staged.size++;
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            for (size_t i = 0; i < staged.size; i++) {
                int found = 0;
                for (size_t j = 0; j < committed.size; j++) {
                    if (strcmp(committed.items[j].key, staged.items[i].key) == 0) {
                        free(committed.items[j].value);
                        committed.items[j].value = safe_strdup(staged.items[i].value);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (committed.size == committed.capacity) {
                        committed.capacity = committed.capacity ? committed.capacity * 2 : 4;
                        committed.items =
                            safe_realloc(committed.items, committed.capacity * sizeof(KV));
                    }
                    committed.items[committed.size].key = safe_strdup(staged.items[i].key);
                    committed.items[committed.size].value = safe_strdup(staged.items[i].value);
                    committed.size++;
                }
            }
            for (size_t i = 0; i < staged.size; i++) {
                free(staged.items[i].key);
                free(staged.items[i].value);
            }
            staged.size = 0;
            in_transaction = 0;
        } else if (strcmp(cmd, "ABORT") == 0) {
            for (size_t i = 0; i < staged.size; i++) {
                free(staged.items[i].key);
                free(staged.items[i].value);
            }
            staged.size = 0;
            in_transaction = 0;
        } else if (strcmp(cmd, "GET") == 0) {
            char *p = line + 3;
            while (*p == ' ')
                p++;
            char *key = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < committed.size; i++) {
                if (strcmp(committed.items[i].key, key) == 0) {
                    printf("%s\n", committed.items[i].value);
                    break;
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < committed.size; i++)
                printf("%s=%s\n", committed.items[i].key, committed.items[i].value);
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < committed.size; i++) {
        free(committed.items[i].key);
        free(committed.items[i].value);
    }
    free(committed.items);
    for (size_t i = 0; i < staged.size; i++) {
        free(staged.items[i].key);
        free(staged.items[i].value);
    }
    free(staged.items);
    return 0;
}