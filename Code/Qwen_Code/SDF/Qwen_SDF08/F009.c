// F009.c
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
    char *desc;
} Ev;

typedef struct {
    char *group;
    Ev *events;
    size_t ev_size;
    size_t ev_cap;
} Grp;

typedef struct {
    Grp *items;
    size_t size;
    size_t capacity;
} GList;

int main(void) {
    GList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "EVENT") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *g_start = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            
            int found = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].group, g_start) == 0) {
                    if (list.items[i].ev_size == list.items[i].ev_cap) {
                        list.items[i].ev_cap = list.items[i].ev_cap ? list.items[i].ev_cap * 2 : 4;
                        list.items[i].events = safe_realloc(list.items[i].events, list.items[i].ev_cap * sizeof(Ev));
                    }
                    list.items[i].events[list.items[i].ev_size++].desc = safe_strdup(p);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.items = safe_realloc(list.items, list.capacity * sizeof(Grp));
                }
                list.items[list.size].group = safe_strdup(g_start);
                list.items[list.size].events = safe_malloc(sizeof(Ev));
                list.items[list.size].events[0].desc = safe_strdup(p);
                list.items[list.size].ev_size = 1;
                list.items[list.size].ev_cap = 1;
                list.size++;
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *g1 = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            char *g2 = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            char *ng = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            Grp *src1 = NULL, *src2 = NULL;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].group, g1) == 0) src1 = &list.items[i];
                if (strcmp(list.items[i].group, g2) == 0) src2 = &list.items[i];
            }
            
            int found = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].group, ng) == 0) {
                    found = 1;
                    if (src1) {
                        for (size_t j = 0; j < src1->ev_size; j++) {
                            if (list.items[i].ev_size == list.items[i].ev_cap) {
                                list.items[i].ev_cap *= 2;
                                list.items[i].events = safe_realloc(list.items[i].events, list.items[i].ev_cap * sizeof(Ev));
                            }
                            list.items[i].events[list.items[i].ev_size++].desc = safe_strdup(src1->events[j].desc);
                        }
                    }
                    if (src2) {
                        for (size_t j = 0; j < src2->ev_size; j++) {
                            if (list.items[i].ev_size == list.items[i].ev_cap) {
                                list.items[i].ev_cap *= 2;
                                list.items[i].events = safe_realloc(list.items[i].events, list.items[i].ev_cap * sizeof(Ev));
                            }
                            list.items[i].events[list.items[i].ev_size++].desc = safe_strdup(src2->events[j].desc);
                        }
                    }
                    break;
                }
            }
            if (!found) {
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.items = safe_realloc(list.items, list.capacity * sizeof(Grp));
                }
                list.items[list.size].group = safe_strdup(ng);
                list.items[list.size].events = NULL;
                list.items[list.size].ev_size = 0;
                list.items[list.size].ev_cap = 0;
                
                if (src1) {
                    for (size_t j = 0; j < src1->ev_size; j++) {
                        if (list.items[list.size].ev_size == list.items[list.size].ev_cap) {
                            list.items[list.size].ev_cap = list.items[list.size].ev_cap ? list.items[list.size].ev_cap * 2 : 4;
                            list.items[list.size].events = safe_realloc(list.items[list.size].events, list.items[list.size].ev_cap * sizeof(Ev));
                        }
                        list.items[list.size].events[list.items[list.size].ev_size++].desc = safe_strdup(src1->events[j].desc);
                    }
                }
                if (src2) {
                    for (size_t j = 0; j < src2->ev_size; j++) {
                        if (list.items[list.size].ev_size == list.items[list.size].ev_cap) {
                            list.items[list.size].ev_cap = list.items[list.size].ev_cap ? list.items[list.size].ev_cap * 2 : 4;
                            list.items[list.size].events = safe_realloc(list.items[list.size].events, list.items[list.size].ev_cap * sizeof(Ev));
                        }
                        list.items[list.size].events[list.items[list.size].ev_size++].desc = safe_strdup(src2->events[j].desc);
                    }
                }
                list.size++;
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *p = line + 11;
            while (*p == ' ') p++;
            char *g_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].group, g_start) == 0) {
                    free(list.items[i].group);
                    for (size_t j = 0; j < list.items[i].ev_size; j++) {
                        free(list.items[i].events[j].desc);
                    }
                    free(list.items[i].events);
                    for (size_t j = i; j < list.size - 1; j++) {
                        list.items[j] = list.items[j + 1];
                    }
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            char *g_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].group, g_start) == 0) {
                    printf("%zu\n", list.items[i].ev_size);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s:", list.items[i].group);
                for (size_t j = 0; j < list.items[i].ev_size; j++) {
                    printf(" %s", list.items[i].events[j].desc);
                }
                printf("\n");
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].group);
        for (size_t j = 0; j < list.items[i].ev_size; j++) {
            free(list.items[i].events[j].desc);
        }
        free(list.items[i].events);
    }
    free(list.items);
    return 0;
}