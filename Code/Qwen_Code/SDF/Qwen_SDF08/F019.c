// F019.c
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
    char *acc;
    int amt;
    char *memo;
} Tx;

typedef struct {
    Tx *items;
    size_t size;
    size_t capacity;
} TList;

void load_file(TList *list, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line;
    while ((line = read_line(f))) {
        char *a = line;
        char *p1 = strchr(a, ' ');
        if (p1) {
            *p1 = '\0';
            p1++;
            int amt = atoi(p1);
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = '\0';
                p2++;
                if (list->size == list->capacity) {
                    list->capacity = list->capacity ? list->capacity * 2 : 4;
                    list->items = safe_realloc(list->items, list->capacity * sizeof(Tx));
                }
                list->items[list->size].acc = safe_strdup(a);
                list->items[list->size].amt = amt;
                list->items[list->size].memo = safe_strdup(p2);
                list->size++;
            }
        }
        free(line);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc < 2) return 0;
    TList list = {0};
    load_file(&list, argv[1]);
    
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            char *a = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            int amt = atoi(p);
            while (*p && *p != ' ') p++;
            if (*p) { p++; while (*p == ' ') p++; }
            
            if (list.size == list.capacity) {
                list.capacity = list.capacity ? list.capacity * 2 : 4;
                list.items = safe_realloc(list.items, list.capacity * sizeof(Tx));
            }
            list.items[list.size].acc = safe_strdup(a);
            list.items[list.size].amt = amt;
            list.items[list.size].memo = safe_strdup(p);
            list.size++;
        } else if (strcmp(cmd, "REVERSE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *a = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            int amt = atoi(p);
            
            for (size_t i = list.size; i > 0; i--) {
                if (strcmp(list.items[i-1].acc, a) == 0 && list.items[i-1].amt == amt) {
                    for (size_t j = i - 1; j < list.size - 1; j++) {
                        list.items[j] = list.items[j + 1];
                    }
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *a = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            int bal = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].acc, a) == 0) {
                    bal += list.items[i].amt;
                }
            }
            printf("%d\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s %d %s\n", list.items[i].acc, list.items[i].amt, list.items[i].memo);
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].acc);
        free(list.items[i].memo);
    }
    free(list.items);
    return 0;
}