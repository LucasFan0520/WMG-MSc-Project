// F008.c
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
    int score;
    char *note;
} Stu;

typedef struct {
    Stu *items;
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
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            char *p = line + (strcmp(cmd, "ADD") == 0 ? 3 : 6);
            while (*p == ' ') p++;
            char *n_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            int sc = atoi(p);
            while (*p && *p != ' ') p++;
            if (*p) { p++; while (*p == ' ') p++; }
            
            int found = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, n_start) == 0) {
                    list.items[i].score = sc;
                    free(list.items[i].note);
                    list.items[i].note = safe_strdup(p);
                    found = 1;
                    break;
                }
            }
            if (!found && strcmp(cmd, "ADD") == 0) {
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.items = safe_realloc(list.items, list.capacity * sizeof(Stu));
                }
                list.items[list.size].name = safe_strdup(n_start);
                list.items[list.size].score = sc;
                list.items[list.size].note = safe_strdup(p);
                list.size++;
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *n_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, n_start) == 0) {
                    free(list.items[i].name);
                    free(list.items[i].note);
                    for (size_t j = i; j < list.size - 1; j++) {
                        list.items[j] = list.items[j + 1];
                    }
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (list.size == 0) {
                printf("0\n");
            } else {
                long sum = 0;
                for (size_t i = 0; i < list.size; i++) sum += list.items[i].score;
                printf("%ld\n", sum / (long)list.size);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s %d %s\n", list.items[i].name, list.items[i].score, list.items[i].note);
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
    }
    free(list.items);
    return 0;
}