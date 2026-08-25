// F001.c
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
    char *id;
    char *text;
} Para;

typedef struct {
    Para *items;
    size_t size;
    size_t capacity;
} PList;

int main(void) {
    PList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "NEW") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            char *id_start = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            int found = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].id, id_start) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.items = safe_realloc(list.items, list.capacity * sizeof(Para));
                }
                list.items[list.size].id = safe_strdup(id_start);
                list.items[list.size].text = safe_strdup(p);
                list.size++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *id_start = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].id, id_start) == 0) {
                    char *old = list.items[i].text;
                    list.items[i].text = safe_strcat(old, p);
                    free(old);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPLACE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *id_start = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].id, id_start) == 0) {
                    free(list.items[i].text);
                    list.items[i].text = safe_strdup(p);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *id_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].id, id_start) == 0) {
                    free(list.items[i].id);
                    free(list.items[i].text);
                    for (size_t j = i; j < list.size - 1; j++) {
                        list.items[j] = list.items[j + 1];
                    }
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *id_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            int found = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].id, id_start) == 0) {
                    for (char *c = list.items[i].text; *c; c++) {
                        putchar(*c == ' ' ? '_' : *c);
                    }
                    putchar('\n');
                    found = 1;
                    break;
                }
            }
            if (!found) printf("NOT_FOUND\n");
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s\n", list.items[i].id);
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].id);
        free(list.items[i].text);
    }
    free(list.items);
    return 0;
}