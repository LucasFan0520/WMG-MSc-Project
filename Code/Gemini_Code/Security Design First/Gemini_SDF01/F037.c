// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    size_t size;
} ByteBuffer;

int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}

size_t parse_hex(const char *hex, unsigned char **out) {
    size_t len = strlen(hex);
    size_t count = len / 2;
    *out = malloc(count == 0 ? 1 : count);
    if (!*out) exit(1);
    size_t actual = 0;
    for (size_t i = 0; i < count; i++) {
        int h1 = hex_val(hex[i * 2]);
        int h2 = hex_val(hex[i * 2 + 1]);
        if (h1 != -1 && h2 != -1) {
            (*out)[actual++] = (unsigned char)((h1 << 4) | h2);
        } else {
            break;
        }
    }
    return actual;
}

int main(void) {
    ByteBuffer *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "NEW") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *hex = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            unsigned char *parsed = NULL;
            size_t sz = parse_hex(hex, &parsed);

            if (count >= capacity) {
                capacity = capacity == 0 ? 4 : capacity * 2;
                ByteBuffer *new_items = realloc(items, capacity * sizeof(ByteBuffer));
                if (!new_items) exit(1);
                items = new_items;
            }
            items[count].name = strdup(name);
            items[count].bytes = parsed;
            items[count].size = sz;
            if (!items[count].name) exit(1);
            count++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *hex = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    unsigned char *parsed = NULL;
                    size_t sz = parse_hex(hex, &parsed);
                    if (sz > 0) {
                        unsigned char *new_b = realloc(items[i].bytes, items[i].size + sz);
                        if (!new_b) exit(1);
                        memcpy(new_b + items[i].size, parsed, sz);
                        items[i].bytes = new_b;
                        items[i].size += sz;
                    }
                    free(parsed);
                    break;
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;

            char *off_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *len_str = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            long offset = strtol(off_str, NULL, 10);
            long length = strtol(len_str, NULL, 10);

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    if (offset >= 0 && length >= 0 && (size_t)offset < items[i].size && (size_t)(offset + length) <= items[i].size) {
                        for (long j = 0; j < length; j++) {
                            printf("%02X", items[i].bytes[offset + j]);
                        }
                        putchar('\n');
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].bytes);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %zu\n", items[i].name, items[i].size);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].bytes);
    }
    free(items);
    free(line);
    return 0;
}
