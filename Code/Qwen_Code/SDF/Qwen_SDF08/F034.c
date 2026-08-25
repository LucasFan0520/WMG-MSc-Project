// F034.c
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
} Rec;
typedef struct {
    Rec *items;
    size_t size;
    size_t capacity;
} RList;

int main(int argc, char **argv) {
    RList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "LOAD") == 0) {
            if (argc < 2) {
                free(line);
                continue;
            }
            FILE *f = fopen(argv[1], "r");
            if (!f) {
                free(line);
                continue;
            }
            RList temp = {0};
            int valid = 1;
            char *fline;
            while ((fline = read_line(f))) {
                char *name = fline;
                char *note = strchr(fline, ' ');
                if (note) {
                    *note = '\0';
                    note++;
                    if (temp.size == temp.capacity) {
                        temp.capacity = temp.capacity ? temp.capacity * 2 : 4;
                        temp.items = safe_realloc(temp.items, temp.capacity * sizeof(Rec));
                    }
                    temp.items[temp.size].name = safe_strdup(name);
                    temp.items[temp.size].note = safe_strdup(note);
                    temp.size++;
                } else {
                    valid = 0;
                }
                free(fline);
            }
            fclose(f);
            if (valid) {
                for (size_t i = 0; i < list.size; i++) {
                    free(list.items[i].name);
                    free(list.items[i].note);
                }
                free(list.items);
                list = temp;
            } else {
                for (size_t i = 0; i < temp.size; i++) {
                    free(temp.items[i].name);
                    free(temp.items[i].note);
                }
                free(temp.items);
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ')
                p++;
            char *name = p;
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
                list.items = safe_realloc(list.items, list.capacity * sizeof(Rec));
            }
            list.items[list.size].name = safe_strdup(name);
            list.items[list.size].note = safe_strdup(p);
            list.size++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ')
                p++;
            char *name = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, name) == 0) {
                    free(list.items[i].name);
                    free(list.items[i].note);
                    for (size_t j = i; j < list.size - 1; j++)
                        list.items[j] = list.items[j + 1];
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.size; i++)
                printf("%s %s\n", list.items[i].name, list.items[i].note);
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].name);
        free(list.items[i].note);
    }
    free(list.items);
    return 0;
}