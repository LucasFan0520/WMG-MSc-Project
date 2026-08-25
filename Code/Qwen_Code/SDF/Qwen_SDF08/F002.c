// F002.c
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
    char *tag;
    char *message;
} Record;

typedef struct {
    Record *items;
    size_t size;
    size_t capacity;
} RList;

int main(void) {
    RList list = {0};
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
            char *tag_start = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ') p++;
            }
            int found = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].tag, tag_start) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.items = safe_realloc(list.items, list.capacity * sizeof(Record));
                }
                list.items[list.size].tag = safe_strdup(tag_start);
                list.items[list.size].message = safe_strdup(p);
                list.size++;
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *t1 = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            char *t2 = p;
            while (*p && *p != ' ') p++;
            if (*p) { *p = '\0'; p++; while (*p == ' ') p++; }
            char *nt = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            
            char *m1 = NULL;
            char *m2 = NULL;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].tag, t1) == 0) m1 = list.items[i].message;
                if (strcmp(list.items[i].tag, t2) == 0) m2 = list.items[i].message;
            }
            char *combined = safe_strcat(m1, " ");
            char *final_msg = safe_strcat(combined, m2);
            free(combined);
            
            int found = 0;
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].tag, nt) == 0) {
                    free(list.items[i].message);
                    list.items[i].message = final_msg;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (list.size == list.capacity) {
                    list.capacity = list.capacity ? list.capacity * 2 : 4;
                    list.items = safe_realloc(list.items, list.capacity * sizeof(Record));
                }
                list.items[list.size].tag = safe_strdup(nt);
                list.items[list.size].message = final_msg;
                list.size++;
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *tag_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].tag, tag_start) == 0) {
                    free(list.items[i].tag);
                    free(list.items[i].message);
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
            char *tag_start = p;
            while (*p && *p != ' ') p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].tag, tag_start) == 0) {
                    for (char *c = list.items[i].message; *c; c++) {
                        putchar(*c == ' ' ? '_' : *c);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.size; i++) {
                printf("%s %s\n", list.items[i].tag, list.items[i].message);
            }
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].tag);
        free(list.items[i].message);
    }
    free(list.items);
    return 0;
}