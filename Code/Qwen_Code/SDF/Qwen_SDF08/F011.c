// F011.c
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
    char *title;
    char *body;
} Draft;

typedef struct {
    Draft *items;
    size_t size;
    size_t capacity;
} DList;

typedef struct {
    Draft d;
} Trash;

typedef struct {
    Trash *items;
    size_t size;
    size_t capacity;
} TList;

int main(void) {
    DList list = {0};
    TList trash = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "CREATE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *t_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            
            int found = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].title, t_start) == 0) { found = 1; break; }
            }
            if (!found) {
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.items = safe_realloc(list.items, list.capacity * sizeof(Draft));
                }
                list.items[list.size].title = safe_strdup(t_start);
                list.items[list.size].body = safe_strdup(p);
                list.size++;
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *t_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].title, t_start) == 0) {
                    free(list.items[i].body);
                    list.items[i].body = safe_strdup(p);
                    break;
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *t_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].title, t_start) == 0) {
                    if (trash.size == trash.capacity) {
                        trash.capacity = trash.capacity ? trash.capacity * 2 : 4;
                        trash.items = safe_realloc(trash.items, trash.capacity * sizeof(Trash));
                    }
                    trash.items[trash.size++].d = list.items[i];
                    
                    for (size_t j = i; j < list.size - 1; j++) {
                        list.items[j] = list.items[j + 1];
                    }
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (trash.size > 0) {
                trash.size--;
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.items = safe_realloc(list.items, list.capacity * sizeof(Draft));
                }
                list.items[list.size++] = trash.items[trash.size].d;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *t_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].title, t_start) == 0) {
                    printf("%s\n", list.items[i].body);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s %s\n", list.items[i].title, list.items[i].body);
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].title);
        free(list.items[i].body);
    }
    free(list.items);
    for (size_t i = 0; i < trash.size; i++) {
        free(trash.items[i].d.title);
        free(trash.items[i].d.body);
    }
    free(trash.items);
    return 0;
}