// F007.c
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
    int time;
    char *label;
    char *note;
} Act;

typedef struct {
    Act *items;
    size_t size;
    size_t capacity;
} AList;

int main(void) {
    AList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "ADD") == 0) {
            int t;
            char *p = line + 3;
            while (*p == ' ') p++;
            char *t_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            t = atoi(t_start);
            
            char *l_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            
            if (list.size == list.capacity) {
                list.capacity = list.capacity ? list.capacity * 2 : 4;
                list.items = safe_realloc(list.items, list.capacity * sizeof(Act));
            }
            list.items[list.size].time = t;
            list.items[list.size].label = safe_strdup(l_start);
            list.items[list.size].note = safe_strdup(p);
            list.size++;
        } else if (strcmp(cmd, "DELETE") == 0) {
            int t;
            char *p = line + 6;
            while (*p == ' ') p++;
            char *t_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            t = atoi(t_start);
            char *l_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (list.items[i].time == t && strcmp(list.items[i].label, l_start) == 0) {
                    free(list.items[i].label);
                    free(list.items[i].note);
                    for (size_t j = i; j < list.size - 1; j++) {
                        list.items[j] = list.items[j + 1];
                    }
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *l_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            int min_t = -1;
            size_t min_i = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].label, l_start) == 0) {
                    if (min_t == -1 || list.items[i].time < min_t) {
                        min_t = list.items[i].time;
                        min_i = i;
                    }
                }
            }
            if (min_t != -1) {
                printf("%d %s %s\n", list.items[min_i].time, list.items[min_i].label, list.items[min_i].note);
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                for (size_t j = i + 1; j < list.size; j++) {
                    int swap = 0;
                    if (list.items[i].time > list.items[j].time) swap = 1;
                    else if (list.items[i].time == list.items[j].time && strcmp(list.items[i].label, list.items[j].label) > 0) swap = 1;
                    
                    if (swap) {
                        Act temp = list.items[i];
                        list.items[i] = list.items[j];
                        list.items[j] = temp;
                    }
                }
            }
            for (size_t i = 0; i < list.size; i++) {
                printf("%d %s %s\n", list.items[i].time, list.items[i].label, list.items[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                free(list.items[i].label);
                free(list.items[i].note);
            }
            list.size = 0;
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].label);
        free(list.items[i].note);
    }
    free(list.items);
    return 0;
}