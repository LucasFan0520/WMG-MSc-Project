// F018.c
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
    char *word;
    int count;
} Wrd;

typedef struct {
    Wrd *items;
    size_t size;
    size_t capacity;
} WList;

void load_file(WList *list, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line;
    while ((line = read_line(f))) {
        if (strlen(line) > 0) {
            int found = 0;
            for (size_t i = 0; i < list->size; i++) {
                if (strcmp(list->items[i].word, line) == 0) {
                    list->items[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (list->size == list->capacity) {
                    list->capacity = list->capacity ? list->capacity * 2 : 4;
                    list->items = safe_realloc(list->items, list->capacity * sizeof(Wrd));
                }
                list->items[list->size].word = safe_strdup(line);
                list->items[list->size].count = 1;
                list->size++;
            }
        }
        free(line);
    }
    fclose(f);
}

int main(int argc, char **argv) {
    WList list = {0};
    if (argc >= 2) load_file(&list, argv[1]);
    
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "WORD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *w_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            int found = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].word, w_start) == 0) {
                    list.items[i].count++;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.items = safe_realloc(list.items, list.capacity * sizeof(Wrd));
                }
                list.items[list.size].word = safe_strdup(w_start);
                list.items[list.size].count = 1;
                list.size++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *w_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].word, w_start) == 0) {
                    free(list.items[i].word);
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
            char *w_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            int found = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].word, w_start) == 0) {
                    printf("%d\n", list.items[i].count);
                    found = 1;
                    break;
                }
            }
            if (!found) printf("0\n");
        } else if (strcmp(cmd, "TOP") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                for (size_t j = i + 1; j < list.size; j++) {
                    if (list.items[i].count < list.items[j].count) {
                        Wrd temp = list.items[i];
                        list.items[i] = list.items[j];
                        list.items[j] = temp;
                    }
                }
            }
            for (size_t i = 0; i < list.size; i++) {
                printf("%s %d\n", list.items[i].word, list.items[i].count);
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].word);
    }
    free(list.items);
    return 0;
}