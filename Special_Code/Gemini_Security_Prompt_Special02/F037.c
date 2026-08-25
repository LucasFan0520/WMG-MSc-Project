// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    unsigned char *bytes;
    size_t size;
} HexBuffer;

unsigned char *parse_hex(const char *hex, size_t *out_len) {
    size_t len = strlen(hex);
    size_t count = len / 2;
    unsigned char *buf = malloc(count + 1);
    for (size_t i = 0; i < count; i++) {
        unsigned int val;
        sscanf(hex + 2 * i, "%2x", &val);
        buf[i] = (unsigned char)val;
    }
    *out_len = count;
    return buf;
}

int main() {
    HexBuffer *items = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *name = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "NEW") == 0) {
            while (*p == ' ') p++;
            size_t b_len = 0;
            unsigned char *b_data = parse_hex(p, &b_len);
            if (count >= cap) {
                cap = cap == 0 ? 4 : cap * 2;
                items = realloc(items, cap * sizeof(HexBuffer));
            }
            items[count].name = mystrdup(name);
            items[count].bytes = b_data;
            items[count].size = b_len;
            count++;
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*p == ' ') p++;
            size_t b_len = 0;
            unsigned char *b_data = parse_hex(p, &b_len);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    items[i].bytes = realloc(items[i].bytes, items[i].size + b_len + 1);
                    memcpy(items[i].bytes + items[i].size, b_data, b_len);
                    items[i].size += b_len;
                    break;
                }
            }
            free(b_data);
        } else if (strcmp(cmd, "SLICE") == 0) {
            while (*p == ' ') p++;
            char *off_s = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *len_s = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int offset = atoi(off_s);
            int length = atoi(len_s);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    if (offset >= 0 && (size_t)offset < items[i].size && length >= 0) {
                        size_t end = (size_t)offset + (size_t)length;
                        if (end > items[i].size) end = items[i].size;
                        for (size_t j = (size_t)offset; j < end; j++) {
                            printf("%02x", items[i].bytes[j]);
                        }
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
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
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].bytes);
    }
    free(items);
    return 0;
}
