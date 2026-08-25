// F020.c
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

static char *safe_strcat(const char *s1, const char *s2) {
    size_t len1 = s1 ? strlen(s1) : 0;
    size_t len2 = s2 ? strlen(s2) : 0;
    if (len1 > SIZE_MAX - len2 - 1) exit(EXIT_FAILURE);
    char *res = safe_malloc(len1 + len2 + 1);
    if (s1) memcpy(res, s1, len1);
    if (s2) memcpy(res + len1, s2, len2 + 1);
    else res[len1] = '\0';
    return res;
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
    char *body;
} Rec;

typedef struct {
    Rec *items;
    size_t size;
    size_t capacity;
} RList;

void load_file(RList *list, const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char *line;
    char *cur_title = NULL;
    char *cur_body = NULL;
    
    while ((line = read_line(f))) {
        if (strcmp(line, "--") == 0) {
            if (cur_title) {
                if (list->size == list->capacity) {
                    list->capacity = list->capacity ? list->capacity * 2 : 4;
                    list->items = safe_realloc(list->items, list->capacity * sizeof(Rec));
                }
                list->items[list->size].title = cur_title;
                list->items[list->size].body = cur_body ? cur_body : safe_strdup("");
                list->size++;
                cur_title = NULL;
                cur_body = NULL;
            }
        } else {
            if (!cur_title) {
                cur_title = safe_strdup(line);
            } else {
                char *temp = cur_body ? safe_strcat(cur_body, "\n") : safe_strdup("");
                char *temp2 = safe_strcat(temp, line);
                free(temp);
                if (cur_body) free(cur_body);
                cur_body = temp2;
            }
        }
        free(line);
    }
    if (cur_title) {
        if (list->size == list->capacity) {
            list->capacity = list->capacity ? list->capacity * 2 : 4;
            list->items = safe_realloc(list->items, list->capacity * sizeof(Rec));
        }
        list->items[list->size].title = cur_title;
        list->items[list->size].body = cur_body ? cur_body : safe_strdup("");
        list->size++;
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc < 2) return 0;
    RList list = {0};
    load_file(&list, argv[1]);
    
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "FIND") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *t_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].title, t_start) == 0) {
                    printf("%s\n%s\n", list.items[i].title, list.items[i].body);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *t_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].title, t_start) == 0) {
                    free(list.items[i].title);
                    free(list.items[i].body);
                    for (size_t j = i; j < list.size - 1; j++) {
                        list.items[j] = list.items[j + 1];
                    }
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            char *t_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            char *body = safe_strdup("");
            char *bline;
            while ((bline = read_line(stdin))) {
                if (strcmp(bline, "--") == 0) {
                    free(bline);
                    break;
                }
                char *temp = safe_strcat(body, "\n");
                char *temp2 = safe_strcat(temp, bline);
                free(temp);
                free(body);
                body = temp2;
                free(bline);
            }
            
            if (list.size == list.capacity) {
                list.capacity = list.capacity ? list.capacity * 2 : 4;
                list.items = safe_realloc(list.items, list.capacity * sizeof(Rec));
            }
            list.items[list.size].title = safe_strdup(t_start);
            list.items[list.size].body = body;
            list.size++;
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s\n%s\n--\n", list.items[i].title, list.items[i].body);
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].title);
        free(list.items[i].body);
    }
    free(list.items);
    return 0;
}