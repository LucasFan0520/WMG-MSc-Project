// F016.c
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
    char *name;
    char *email;
    char *phone;
} Con;

typedef struct {
    Con *items;
    size_t size;
    size_t capacity;
} CList;

void load_file(CList *list, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line;
    while ((line = read_line(f))) {
        char *n = line;
        char *e = strchr(n, ',');
        if (e) {
            *e = '\0';
            e++;
            char *p = strchr(e, ',');
            if (p) {
                *p = '\0';
                p++;
                if (list->size == list->capacity) {
                    list->capacity = list->capacity ? list->capacity * 2 : 4;
                    list->items = safe_realloc(list->items, list->capacity * sizeof(Con));
                }
                list->items[list->size].name = safe_strdup(n);
                list->items[list->size].email = safe_strdup(e);
                list->items[list->size].phone = safe_strdup(p);
                list->size++;
            }
        }
        free(line);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc < 2) return 0;
    CList list = {0};
    load_file(&list, argv[1]);
    
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
            char *n = p;
            char *e = strchr(n, ',');
            if (e) {
                *e = '\0';
                e++;
                char *ph = strchr(e, ',');
                if (ph) {
                    *ph = '\0';
                    ph++;
                    if (list.size == list.capacity) {
                        list.capacity = list.capacity ? list.capacity * 2 : 4;
                        list.items = safe_realloc(list.items, list.capacity * sizeof(Con));
                    }
                    list.items[list.size].name = safe_strdup(n);
                    list.items[list.size].email = safe_strdup(e);
                    list.items[list.size].phone = safe_strdup(ph);
                    list.size++;
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
                    free(list.items[i].email);
                    free(list.items[i].phone);
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
                    printf("%s,%s,%s\n", list.items[i].name, list.items[i].email, list.items[i].phone);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s,%s,%s\n", list.items[i].name, list.items[i].email, list.items[i].phone);
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].name);
        free(list.items[i].email);
        free(list.items[i].phone);
    }
    free(list.items);
    return 0;
}