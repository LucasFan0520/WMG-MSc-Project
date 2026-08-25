// F037.c
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
    char *name;
    uint8_t *data;
    size_t size;
    size_t capacity;
} Buf;
typedef struct {
    Buf *items;
    size_t size;
    size_t capacity;
} BList;

static int hex_val(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return 10 + c - 'a';
    if (c >= 'A' && c <= 'F')
        return 10 + c - 'A';
    return -1;
}

int main(void) {
    BList list = {0};
    char *line;
    while ((line = read_line(stdin))) {
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) {
            free(line);
            continue;
        }
        if (strcmp(cmd, "NEW") == 0) {
            char *p = line + 3;
            while (*p == ' ')
                p++;
            char *name = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            size_t len = strlen(p);
            if (len % 2 == 0) {
                size_t bytes = len / 2;
                uint8_t *data = safe_malloc(bytes);
                int valid = 1;
                for (size_t i = 0; i < bytes; i++) {
                    int h = hex_val(p[2 * i]), l = hex_val(p[2 * i + 1]);
                    if (h < 0 || l < 0) {
                        valid = 0;
                        break;
                    }
                    data[i] = (h << 4) | l;
                }
                if (valid) {
                    if (list.size == list.capacity) {
                        list.capacity = list.capacity ? list.capacity * 2 : 4;
                        list.items = safe_realloc(list.items, list.capacity * sizeof(Buf));
                    }
                    list.items[list.size].name = safe_strdup(name);
                    list.items[list.size].data = data;
                    list.items[list.size].size = bytes;
                    list.items[list.size].capacity = bytes;
                    list.size++;
                } else
                    free(data);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p = line + 6;
            while (*p == ' ')
                p++;
            char *name = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            size_t len = strlen(p);
            if (len % 2 == 0) {
                size_t bytes = len / 2;
                for (size_t i = 0; i < list.size; i++) {
                    if (strcmp(list.items[i].name, name) == 0) {
                        if (list.items[i].size + bytes > list.items[i].capacity) {
                            size_t new_cap = list.items[i].size + bytes;
                            uint8_t *temp = safe_realloc(list.items[i].data, new_cap);
                            list.items[i].data = temp;
                            list.items[i].capacity = new_cap;
                        }
                        int valid = 1;
                        for (size_t j = 0; j < bytes; j++) {
                            int h = hex_val(p[2 * j]), l = hex_val(p[2 * j + 1]);
                            if (h < 0 || l < 0) {
                                valid = 0;
                                break;
                            }
                            list.items[i].data[list.items[i].size + j] = (h << 4) | l;
                        }
                        if (valid)
                            list.items[i].size += bytes;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *p = line + 5;
            while (*p == ' ')
                p++;
            char *name = p;
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            int offset = atoi(p);
            while (*p && *p != ' ')
                p++;
            if (*p) {
                *p = '\0';
                p++;
                while (*p == ' ')
                    p++;
            }
            int length = atoi(p);
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, name) == 0) {
                    if (offset >= 0 && length >= 0 &&
                        (size_t)(offset + length) <= list.items[i].size) {
                        for (int j = 0; j < length; j++)
                            printf("%02x", list.items[i].data[offset + j]);
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ')
                p++;
            char *name = p;
            while (*p && *p != ' ')
                p++;
            *p = '\0';
            for (size_t i = 0; i < list.size; i++) {
                if (strcmp(list.items[i].name, name) == 0) {
                    free(list.items[i].name);
                    free(list.items[i].data);
                    for (size_t j = i; j < list.size - 1; j++)
                        list.items[j] = list.items[j + 1];
                    list.size--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.size; i++)
                printf("%s %zu\n", list.items[i].name, list.items[i].size);
        } else if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        free(line);
    }
    for (size_t i = 0; i < list.size; i++) {
        free(list.items[i].name);
        free(list.items[i].data);
    }
    free(list.items);
    return 0;
}