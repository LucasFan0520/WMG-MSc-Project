// F027.c
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
    int priority;
    char *title;
    char *note;
    size_t order;
} Rem;
typedef struct {
    Rem *items;
    size_t size;
    size_t capacity;
    size_t next_order;
} RList;

int main(void) {
    RList list = {0};
    list.next_order = 0;
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
            int pri = atoi(p);
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            char *title = p;
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
                list.items = safe_realloc(list.items, list.capacity * sizeof(Rem));
            }
            list.items[list.size].priority = pri;
            list.items[list.size].title = safe_strdup(title);
            list.items[list.size].note = safe_strdup(p);
            list.items[list.size].order = list.next_order++;
            list.size++;
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *p = line + 4;
            while (*p == ' ')
                p++;
            char *title = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            int delta = atoi(p);
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].title, title) == 0) {
                    list.items[i].priority += delta;
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 6;
            while (*p == ' ')
                p++;
            char *title = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].title, title) == 0) {
                    free(list.items[i].title);
                    free(list.items[i].note);
                    for (size_t j = i; j < list.size - 1; j++)
                        list.items[j] = list.items[j + 1];
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            char *p = line + 3;
            while (*p == ' ')
                p++;
            int n = atoi(p);
            Rem *sorted = safe_malloc(list.size * sizeof(Rem));
            memcpy(sorted, list.items, list.size * sizeof(Rem));
            for (size_t i = 0; i < list.size; i++) {
                for (size_t j = i + 1; j < list.size; j++) {
                    if (sorted[i].priority < sorted[j].priority ||
                        (sorted[i].priority == sorted[j].priority &&
                         sorted[i].order > sorted[j].order)) {
                        Rem temp = sorted[i];
                        sorted[i] = sorted[j];
                        sorted[j] = temp;
                    }
                }
            }
            size_t limit = n < (int)list.size ? n : list.size;
            for (size_t i = 0; i < limit; i++)
                printf("%d %s %s\n", sorted[i].priority, sorted[i].title, sorted[i].note);
            free(sorted);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.size; i++)
                printf("%d %s %s\n", list.items[i].priority, list.items[i].title,
                       list.items[i].note);
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].title);
        free(list.items[i].note);
    }
    free(list.items);
    return 0;
}