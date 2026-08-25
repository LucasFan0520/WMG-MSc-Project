// F022.c
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
    char *title;
    char *note;
    int done;
} Task;

typedef struct {
    char *name;
    Task *tasks;
    size_t t_size;
    size_t t_cap;
} Proj;

typedef struct {
    Proj *items;
    size_t size;
    size_t capacity;
} PList;

int main(void) {
    PList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "PROJECT") == 0) {
            char *p = line + 7;
            while (*p == ' ')
                p++;
            char *name = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            int found = 0;
            for (size_t i = 0; i < list.size; i++)
                if (strcmp(list.items[i].name, name) == 0) {
                    found = 1;
                    break;
                }
            if (!found) {
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.items = safe_realloc(list.items, list.capacity * sizeof(Proj));
                }
                list.items[list.size].name = safe_strdup(name);
                list.items[list.size].tasks = NULL;
                list.items[list.size].t_size = 0;
                list.items[list.size].t_cap = 0;
                list.size++;
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char *p = line + 4;
            while (*p == ' ')
                p++;
            char *proj = p;
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
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, proj) == 0) {
                    if (list.items[i].t_size == list.items[i].t_cap) {
                        list.items[i].t_cap = list.items[i].t_cap ? list.items[i].t_cap * 2 : 4;
                        list.items[i].tasks =
                            safe_realloc(list.items[i].tasks, list.items[i].t_cap * sizeof(Task));
                    }
                    list.items[i].tasks[list.items[i].t_size].title = safe_strdup(title);
                    list.items[i].tasks[list.items[i].t_size].note = safe_strdup(p);
                    list.items[i].tasks[list.items[i].t_size].done = 0;
                    list.items[i].t_size++;
                    break;
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *p = line + 8;
            while (*p == ' ')
                p++;
            char *from = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            char *to = p;
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
            *p = '\0';
            Proj *src = NULL, *dst = NULL;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, from) == 0)
                    src = &list.items[i];
                if (strcmp(list.items[i].name, to) == 0)
                    dst = &list.items[i];
            }
            if (src && dst) {
                for (size_t i = 0; i < src->t_size; i++) {
                    if (strcmp(src->tasks[i].title, title) == 0) {
                        if (dst->t_size == dst->t_cap) {
                            dst->t_cap = dst->t_cap ? dst->t_cap * 2 : 4;
                            dst->tasks = safe_realloc(dst->tasks, dst->t_cap * sizeof(Task));
                        }
                        dst->tasks[dst->t_size++] = src->tasks[i];
                        for (size_t j = i; j < src->t_size - 1; j++)
                            src->tasks[j] = src->tasks[j + 1];
                        src->t_size--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *p = line + 8;
            while (*p == ' ')
                p++;
            char *proj = p;
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
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, proj) == 0) {
                    for (size_t j = 0; j < list.items[i].t_size; j++) {
                        if (strcmp(list.items[i].tasks[j].title, title) == 0) {
                            list.items[i].tasks[j].done = 1;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *p = line + 13;
            while (*p == ' ')
                p++;
            char *name = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, name) == 0) {
                    free(list.items[i].name);
                    for (size_t j = 0; j < list.items[i].t_size; j++) {
                        free(list.items[i].tasks[j].title);
                        free(list.items[i].tasks[j].note);
                    }
                    free(list.items[i].tasks);
                    for (size_t j = i; j < list.size - 1; j++)
                        list.items[j] = list.items[j + 1];
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s\n", list.items[i].name);
                for (size_t j = 0; j < list.items[i].t_size; j++) {
                    printf("  %s %s %s\n", list.items[i].tasks[j].title,
                           list.items[i].tasks[j].note,
                           list.items[i].tasks[j].done ? "DONE" : "TODO");
                }
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].name);
        for (size_t j = 0; j < list.items[i].t_size; j++) {
            free(list.items[i].tasks[j].title);
            free(list.items[i].tasks[j].note);
        }
        free(list.items[i].tasks);
    }
    free(list.items);
    return 0;
}