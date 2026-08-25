// F014.c
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
    char *label;
} Lbl;

typedef struct {
    char *name;
    char *note;
    Lbl *labels;
    size_t l_size;
    size_t l_cap;
} Item;

typedef struct {
    Item *items;
    size_t size;
    size_t capacity;
} IList;

int main(void) {
    IList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "ITEM") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *n_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            
            if (list.size == list.capacity) {
                list.capacity = list.capacity ? list.capacity * 2 : 4;
                list.items = safe_realloc(list.items, list.capacity * sizeof(Item));
            }
            list.items[list.size].name = safe_strdup(n_start);
            list.items[list.size].note = safe_strdup(p);
            list.items[list.size].labels = NULL;
            list.items[list.size].l_size = 0;
            list.items[list.size].l_cap = 0;
            list.size++;
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *n_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            char *l_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, n_start) == 0) {
                    if (list.items[i].l_size == list.items[i].l_cap) {
                        list.items[i].l_cap = list.items[i].l_cap ? list.items[i].l_cap * 2 : 4;
                        list.items[i].labels = safe_realloc(list.items[i].labels, list.items[i].l_cap * sizeof(Lbl));
                    }
                    list.items[i].labels[list.items[i].l_size++].label = safe_strdup(l_start);
                    break;
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *n_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            char *l_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, n_start) == 0) {
                    for (size_t j = 0; j < list.items[i].l_size; j++) {
                        if (strcmp(list.items[i].labels[j].label, l_start) == 0) {
                            free(list.items[i].labels[j].label);
                            for (size_t k = j; k < list.items[i].l_size - 1; k++) {
                                list.items[i].labels[k] = list.items[i].labels[k + 1];
                            }
                            list.items[i].l_size--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *n_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, n_start) == 0) {
                    free(list.items[i].name);
                    free(list.items[i].note);
                    for (size_t j = 0; j < list.items[i].l_size; j++) {
                        free(list.items[i].labels[j].label);
                    }
                    free(list.items[i].labels);
                    for (size_t j = i; j < list.size - 1; j++) {
                        list.items[j] = list.items[j + 1];
                    }
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *n_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, n_start) == 0) {
                    printf("%s %s", list.items[i].name, list.items[i].note);
                    for (size_t j = 0; j < list.items[i].l_size; j++) {
                        printf(" %s", list.items[i].labels[j].label);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s %s", list.items[i].name, list.items[i].note);
                for (size_t j = 0; j < list.items[i].l_size; j++) {
                    printf(" %s", list.items[i].labels[j].label);
                }
                printf("\n");
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].name);
        free(list.items[i].note);
        for (size_t j = 0; j < list.items[i].l_size; j++) {
            free(list.items[i].labels[j].label);
        }
        free(list.items[i].labels);
    }
    free(list.items);
    return 0;
}