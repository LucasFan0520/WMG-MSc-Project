// F013.c
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
    char *note;
    char *list_name;
} Task;

typedef struct {
    Task *items;
    size_t size;
    size_t capacity;
} TList;

int main(void) {
    TList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "TODO") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *t_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            
            if (list.size == list.capacity) {
                list.capacity = list.capacity ? list.capacity * 2 : 4;
                list.items = safe_realloc(list.items, list.capacity * sizeof(Task));
            }
            list.items[list.size].title = safe_strdup(t_start);
            list.items[list.size].note = safe_strdup(p);
            list.items[list.size].list_name = safe_strdup("todo");
            list.size++;
        } else if (strcmp(cmd, "START") == 0 || strcmp(cmd, "DONE") == 0 || strcmp(cmd, "REMOVE") == 0) {
            char *p = line + (strcmp(cmd, "START") == 0 ? 5 : (strcmp(cmd, "DONE") == 0 ? 4 : 6));
            while (*p == ' ') p++;
            char *t_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].title, t_start) == 0) {
                    if (strcmp(cmd, "REMOVE") == 0) {
                        free(list.items[i].title);
                        free(list.items[i].note);
                        free(list.items[i].list_name);
                        for (size_t j = i; j < list.size - 1; j++) {
                            list.items[j] = list.items[j + 1];
                        }
                        list.size--;
                    } else {
                        free(list.items[i].list_name);
                        list.items[i].list_name = safe_strdup(strcmp(cmd, "START") == 0 ? "doing" : "done");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *t_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].title, t_start) == 0) {
                    printf("%s %s\n", list.items[i].list_name, list.items[i].note);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            printf("todo:");
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].list_name, "todo") == 0) printf(" %s", list.items[i].title);
            }
            printf("\ndoing:");
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].list_name, "doing") == 0) printf(" %s", list.items[i].title);
            }
            printf("\ndone:");
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].list_name, "done") == 0) printf(" %s", list.items[i].title);
            }
            printf("\n");
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].title);
        free(list.items[i].note);
        free(list.items[i].list_name);
    }
    free(list.items);
    return 0;
}