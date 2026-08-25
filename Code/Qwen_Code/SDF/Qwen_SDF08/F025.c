// F025.c
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
    char *note;
} Stu;
typedef struct {
    char *name;
    Stu *students;
    size_t s_size;
    size_t s_cap;
} Class;
typedef struct {
    Class *items;
    size_t size;
    size_t capacity;
} CList;

int main(void) {
    CList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "CLASS") == 0) {
            char *p = line + 5;
            while (*p == ' ')
                p++;
            char *name = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            if (list.size == list.capacity) {
                list.capacity = list.capacity ? list.capacity * 2 : 4;
                list.items = safe_realloc(list.items, list.capacity * sizeof(Class));
            }
            list.items[list.size].name = safe_strdup(name);
            list.items[list.size].students = NULL;
            list.items[list.size].s_size = 0;
            list.items[list.size].s_cap = 0;
            list.size++;
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *p = line + 7;
            while (*p == ' ')
                p++;
            char *cls = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            char *stu = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, cls) == 0) {
                    if (list.items[i].s_size == list.items[i].s_cap) {
                        list.items[i].s_cap = list.items[i].s_cap ? list.items[i].s_cap * 2 : 4;
                        list.items[i].students =
                            safe_realloc(list.items[i].students, list.items[i].s_cap * sizeof(Stu));
                    }
                    list.items[i].students[list.items[i].s_size].name = safe_strdup(stu);
                    list.items[i].students[list.items[i].s_size].note = safe_strdup(p);
                    list.items[i].s_size++;
                    break;
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
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
            char *stu = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            Class *src = NULL, *dst = NULL;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, from) == 0)
                    src = &list.items[i];
                if (strcmp(list.items[i].name, to) == 0)
                    dst = &list.items[i];
            }
            if (src && dst) {
                for (size_t i = 0; i < src->s_size; i++) {
                    if (strcmp(src->students[i].name, stu) == 0) {
                        if (dst->s_size == dst->s_cap) {
                            dst->s_cap = dst->s_cap ? dst->s_cap * 2 : 4;
                            dst->students = safe_realloc(dst->students, dst->s_cap * sizeof(Stu));
                        }
                        dst->students[dst->s_size++] = src->students[i];
                        for (size_t j = i; j < src->s_size - 1; j++)
                            src->students[j] = src->students[j + 1];
                        src->s_size--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *p = line + 4;
            while (*p == ' ')
                p++;
            char *cls = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            char *stu = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, cls) == 0) {
                    for (size_t j = 0; j < list.items[i].s_size; j++) {
                        if (strcmp(list.items[i].students[j].name, stu) == 0) {
                            free(list.items[i].students[j].name);
                            free(list.items[i].students[j].note);
                            for (size_t k = j; k < list.items[i].s_size - 1; k++)
                                list.items[i].students[k] = list.items[i].students[k + 1];
                            list.items[i].s_size--;
                            break;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *p = line + 11;
            while (*p == ' ')
                p++;
            char *cls = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, cls) == 0) {
                    free(list.items[i].name);
                    for (size_t j = 0; j < list.items[i].s_size; j++) {
                        free(list.items[i].students[j].name);
                        free(list.items[i].students[j].note);
                    }
                    free(list.items[i].students);
                    for (size_t j = i; j < list.size - 1; j++)
                        list.items[j] = list.items[j + 1];
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s\n", list.items[i].name);
                for (size_t j = 0; j < list.items[i].s_size; j++)
                    printf("  %s %s\n", list.items[i].students[j].name,
                           list.items[i].students[j].note);
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].name);
        for (size_t j = 0; j < list.items[i].s_size; j++) {
            free(list.items[i].students[j].name);
            free(list.items[i].students[j].note);
        }
        free(list.items[i].students);
    }
    free(list.items);
    return 0;
}