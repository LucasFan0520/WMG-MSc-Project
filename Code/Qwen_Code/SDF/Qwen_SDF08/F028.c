// F028.c
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
    char *stop;
    char *note;
} Stop;
typedef struct {
    Stop *items;
    size_t size;
    size_t capacity;
} SList;

int main(void) {
    SList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ')
                p++;
            char *stop = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            if (list.size == list.capacity) {
                list.capacity = list.capacity ? list.capacity * 2 : 4;
                list.items = safe_realloc(list.items, list.capacity * sizeof(Stop));
            }
            list.items[list.size].stop = safe_strdup(stop);
            list.items[list.size].note = safe_strdup(p);
            list.size++;
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *p = line + 6;
            while (*p == ' ')
                p++;
            int idx = atoi(p);
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            char *stop = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            if (idx >= 0 && idx <= (int)list.size) {
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.items = safe_realloc(list.items, list.capacity * sizeof(Stop));
                }
                for (size_t i = list.size; i > (size_t)idx; i--)
                    list.items[i] = list.items[i - 1];
                list.items[idx].stop = safe_strdup(stop);
                list.items[idx].note = safe_strdup(p);
                list.size++;
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *p = line + 4;
            while (*p == ' ')
                p++;
            int from = atoi(p);
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            int to = atoi(p);
            if (from >= 0 && from < (int)list.size && to >= 0 && to < (int)list.size &&
                from != to) {
                Stop temp = list.items[from];
                if (from < to) {
                    for (size_t i = from; i < (size_t)to; i++)
                        list.items[i] = list.items[i + 1];
                } else {
                    for (size_t i = from; i > (size_t)to; i--)
                        list.items[i] = list.items[i - 1];
                }
                list.items[to] = temp;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 6;
            while (*p == ' ')
                p++;
            int idx = atoi(p);
            if (idx >= 0 && idx < (int)list.size) {
                free(list.items[idx].stop);
                free(list.items[idx].note);
                for (size_t i = idx; i < list.size - 1; i++)
                    list.items[i] = list.items[i + 1];
                list.size--;
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ')
                p++;
            char *stop = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].stop, stop) == 0) {
                    printf("%s\n", list.items[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < list.size; i++)
                printf("%s %s\n", list.items[i].stop, list.items[i].note);
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].stop);
        free(list.items[i].note);
    }
    free(list.items);
    return 0;
}