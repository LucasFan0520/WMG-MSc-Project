// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

static void *safe_malloc(size_t size) {
    void *ptr = malloc(size ? size : 1);
    if (!ptr)
        exit(EXIT_FAILURE);
    return ptr;
}
static void *safe_realloc(void *ptr, size_t size) {
    void *temp = realloc(ptr, size ? size : 1);
    if (!temp)
        exit(EXIT_FAILURE);
    return temp;
}
static char *safe_strdup(const char *s) {
    if (!s)
        return NULL;
    char *ptr = strdup(s);
    if (!ptr)
        exit(EXIT_FAILURE);
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
            if (capacity > SIZE_MAX / 2) {
                free(buffer);
                return NULL;
            }
            capacity *= 2;
            buffer = safe_realloc(buffer, capacity);
        }
    }
    if (length > 0)
        return buffer;
    free(buffer);
    return NULL;
}

typedef struct {
    char *old_str;
    char *new_str;
} Rule;
typedef struct {
    Rule *items;
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
            while (*p == ' ')
                p++;
            char *old_str = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            if (list.size == list.capacity) {
                list.capacity = list.capacity ? list.capacity * 2 : 4;
                list.items = safe_realloc(list.items, list.capacity * sizeof(Rule));
            }
            list.items[list.size].old_str = safe_strdup(old_str);
            list.items[list.size].new_str = safe_strdup(p);
            list.size++;
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *p = line + 5;
            while (*p == ' ')
                p++;
            char *text = p;
            int applied = 0;
            for (size_t i = 0; i < list.size; i++) {
                char *pos = strstr(text, list.items[i].old_str);
                if (pos) {
                    size_t pre_len = pos - text;
                    size_t old_len = strlen(list.items[i].old_str);
                    size_t new_len = strlen(list.items[i].new_str);
                    size_t total_len = pre_len + new_len + strlen(pos + old_len);
                    char *res = safe_malloc(total_len + 1);
                    memcpy(res, text, pre_len);
                    memcpy(res + pre_len, list.items[i].new_str, new_len);
                    strcpy(res + pre_len + new_len, pos + old_len);
                    printf("%s\n", res);
                    free(res);
                    applied = 1;
                    break;
                }
            }
            if (!applied)
                printf("%s\n", text);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ')
                p++;
            char *old_str = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].old_str, old_str) == 0) {
                    free(list.items[i].old_str);
                    free(list.items[i].new_str);
                    for (size_t j = i; j < list.size - 1; j++)
                        list.items[j] = list.items[j + 1];
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *p = line + 4;
            while (*p == ' ')
                p++;
            int i = atoi(p);
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            int j = atoi(p);
            if (i >= 0 && i < (int)list.size && j >= 0 && j < (int)list.size && i != j) {
                Rule temp = list.items[i];
                list.items[i] = list.items[j];
                list.items[j] = temp;
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < list.size; i++)
                printf("%s %s\n", list.items[i].old_str, list.items[i].new_str);
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].old_str);
        free(list.items[i].new_str);
    }
    free(list.items);
    return 0;
}