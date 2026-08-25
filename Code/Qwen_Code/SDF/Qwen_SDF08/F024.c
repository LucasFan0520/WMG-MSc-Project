// F024.c
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
    char *name;
} Auth;
typedef struct {
    char *title;
    int year;
    Auth *auths;
    size_t a_size;
    size_t a_cap;
} Book;
typedef struct {
    Book *items;
    size_t size;
    size_t capacity;
} BList;

int main(void) {
    BList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "BOOK") == 0) {
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
            int year = atoi(p);
            if (list.size == list.capacity) {
                list.capacity = list.capacity ? list.capacity * 2 : 4;
                list.items = safe_realloc(list.items, list.capacity * sizeof(Book));
            }
            list.items[list.size].title = safe_strdup(title);
            list.items[list.size].year = year;
            list.items[list.size].auths = NULL;
            list.items[list.size].a_size = 0;
            list.items[list.size].a_cap = 0;
            list.size++;
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *p = line + 6;
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
            char *name = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].title, title) == 0) {
                    if (list.items[i].a_size == list.items[i].a_cap) {
                        list.items[i].a_cap = list.items[i].a_cap ? list.items[i].a_cap * 2 : 4;
                        list.items[i].auths =
                            safe_realloc(list.items[i].auths, list.items[i].a_cap * sizeof(Auth));
                    }
                    list.items[i].auths[list.items[i].a_size++].name = safe_strdup(name);
                    break;
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *p = line + 12;
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
            char *name = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].title, title) == 0) {
                    for (size_t j = 0; j < list.items[i].a_size; j++) {
                        if (strcmp(list.items[i].auths[j].name, name) == 0) {
                            free(list.items[i].auths[j].name);
                            for (size_t k = j; k < list.items[i].a_size - 1; k++)
                                list.items[i].auths[k] = list.items[i].auths[k + 1];
                            list.items[i].a_size--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
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
                    for (size_t j = 0; j < list.items[i].a_size; j++)
                        free(list.items[i].auths[j].name);
                    free(list.items[i].auths);
                    for (size_t j = i; j < list.size - 1; j++)
                        list.items[j] = list.items[j + 1];
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ')
                p++;
            char *title = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].title, title) == 0) {
                    for (size_t j = 0; j < list.items[i].a_size; j++)
                        printf("%s\n", list.items[i].auths[j].name);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.size; i++)
                printf("%s %d\n", list.items[i].title, list.items[i].year);
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].title);
        for (size_t j = 0; j < list.items[i].a_size; j++)
            free(list.items[i].auths[j].name);
        free(list.items[i].auths);
    }
    free(list.items);
    return 0;
}