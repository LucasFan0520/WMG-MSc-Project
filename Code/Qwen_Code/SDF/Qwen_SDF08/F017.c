// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static void *safe_malloc(size_t size) {
    void *ptr = malloc(size ? size : 1);
    if (!ptr) exit(EXIT_FAILURE);
    return ptr;
}

static void *safe_realloc(void *ptr, size_t size) {
    void *temp = realloc(ptr, size ? size : 1);
    if (!temp) exit(EXIT_FAILURE);
    return temp;
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    char *ptr = strdup(s);
    if (!ptr) exit(EXIT_FAILURE);
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
            if (capacity > SIZE_MAX / 2) { free(buffer); return NULL; }
            capacity *= 2;
            buffer = safe_realloc(buffer, capacity);
        }
    }
    if (length > 0) return buffer;
    free(buffer);
    return NULL;
}

typedef struct {
    char *key;
    char *val;
} KV;

typedef struct {
    KV *items;
    size_t size;
    size_t capacity;
} KVList;

void load_file(KVList *list, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line;
    while ((line = read_line(f))) {
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            if (list->size == list->capacity) {
                list->capacity = list->capacity ? list->capacity * 2 : 4;
                list->items = safe_realloc(list->items, list->capacity * sizeof(KV));
            }
            list->items[list->size].key = safe_strdup(line);
            list->items[list->size].val = safe_strdup(eq + 1);
            list->size++;
        }
        free(line);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc < 2) return 0;
    KVList list = {0};
    load_file(&list, argv[1]);
    
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            char *k_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            
            int found = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].key, k_start) == 0) {
                    free(list.items[i].val);
                    list.items[i].val = safe_strdup(p);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.items = safe_realloc(list.items, list.capacity * sizeof(KV));
                }
                list.items[list.size].key = safe_strdup(k_start);
                list.items[list.size].val = safe_strdup(p);
                list.size++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *k_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].key, k_start) == 0) {
                    free(list.items[i].key);
                    free(list.items[i].val);
                    for (size_t j = i; j < list.size - 1; j++) {
                        list.items[j] = list.items[j + 1];
                    }
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            char *k_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].key, k_start) == 0) {
                    printf("%s\n", list.items[i].val);
                    break;
                }
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s=%s\n", list.items[i].key, list.items[i].val);
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].key);
        free(list.items[i].val);
    }
    free(list.items);
    return 0;
}