// F010.c
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
    char *val;
} Val;

typedef struct {
    char *key;
    Val *vals;
    size_t v_size;
    size_t v_cap;
} Key;

typedef struct {
    Key *items;
    size_t size;
    size_t capacity;
} KList;

int main(void) {
    KList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            char *k_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            
            int found = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].key, k_start) == 0) {
                    if (list.items[i].v_size == list.items[i].v_cap) {
                        list.items[i].v_cap = list.items[i].v_cap ? list.items[i].v_cap * 2 : 4;
                        list.items[i].vals = safe_realloc(list.items[i].vals, list.items[i].v_cap * sizeof(Val));
                    }
                    list.items[i].vals[list.items[i].v_size++].val = safe_strdup(p);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.items = safe_realloc(list.items, list.capacity * sizeof(Key));
                }
                list.items[list.size].key = safe_strdup(k_start);
                list.items[list.size].vals = safe_malloc(sizeof(Val));
                list.items[list.size].vals[0].val = safe_strdup(p);
                list.items[list.size].v_size = 1;
                list.items[list.size].v_cap = 1;
                list.size++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *k_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            char *v_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].key, k_start) == 0) {
                    for (size_t j = 0; j < list.items[i].v_size; j++) {
                        if (strcmp(list.items[i].vals[j].val, v_start) == 0) {
                            free(list.items[i].vals[j].val);
                            for (size_t k = j; k < list.items[i].v_size - 1; k++) {
                                list.items[i].vals[k] = list.items[i].vals[k + 1];
                            }
                            list.items[i].v_size--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *k_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].key, k_start) == 0) {
                    free(list.items[i].key);
                    for (size_t j = 0; j < list.items[i].v_size; j++) {
                        free(list.items[i].vals[j].val);
                    }
                    free(list.items[i].vals);
                    for (size_t j = i; j < list.size - 1; j++) {
                        list.items[j] = list.items[j + 1];
                    }
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *k_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].key, k_start) == 0) {
                    for (size_t j = 0; j < list.items[i].v_size; j++) {
                        printf("%s\n", list.items[i].vals[j].val);
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s\n", list.items[i].key);
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].key);
        for (size_t j = 0; j < list.items[i].v_size; j++) {
            free(list.items[i].vals[j].val);
        }
        free(list.items[i].vals);
    }
    free(list.items);
    return 0;
}