// F005.c
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
    char **lines;
    size_t size;
    size_t capacity;
} LList;

int main(void) {
    LList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "INSERT") == 0) {
            int idx;
            if (sscanf(line, "%*s %d", &idx) == 1) {
                char *p = line + 6;
                while (*p == ' ') p++;
                while (*p && *p != ' ') p++;
                while (*p == ' ') p++;
                
                if (idx < 0 || idx > (int)list.size) {
                    free(line);
                    continue;
                }
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.lines = safe_realloc(list.lines, list.capacity * sizeof(char *));
                }
                for (size_t i = list.size; i > (size_t)idx; i--) {
                    list.lines[i] = list.lines[i - 1];
                }
                list.lines[idx] = safe_strdup(p);
                list.size++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (list.size == list.capacity) {
                list.capacity = list.capacity ? list.capacity * 2 : 4;
                list.lines = safe_realloc(list.lines, list.capacity * sizeof(char *));
            }
            list.lines[list.size++] = safe_strdup(p);
        } else if (strcmp(cmd, "DELETE") == 0) {
            int idx;
            if (sscanf(line, "%*s %d", &idx) == 1) {
                if (idx >= 0 && idx < (int)list.size) {
                    free(list.lines[idx]);
                    for (size_t i = idx; i < list.size - 1; i++) {
                        list.lines[i] = list.lines[i + 1];
                    }
                    list.size--;
                }
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            int idx;
            if (sscanf(line, "%*s %d", &idx) == 1) {
                char *p = line + 5;
                while (*p == ' ') p++;
                while (*p && *p != ' ') p++;
                while (*p == ' ') p++;
                
                if (idx >= 0 && idx < (int)list.size) {
                    free(list.lines[idx]);
                    list.lines[idx] = safe_strdup(p);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                for (char *c = list.lines[i]; *c; c++) {
                    putchar(*c == ' ' ? '_' : *c);
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.lines[i]);
    }
    free(list.lines);
    return 0;
}