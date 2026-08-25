// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    size_t size;
    size_t cap;
} ByteBuffer;

char *read_line(FILE *fp) {
    size_t size = 16;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *nb = realloc(buf, size);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        len--;
    }
    buf[len] = '\0';
    return buf;
}

unsigned char parse_hex_pair(char h1, char h2) {
    char s[3] = {h1, h2, '\0'};
    return (unsigned char)strtol(s, NULL, 16);
}

int main(void) {
    ByteBuffer *buffers = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "NEW") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *hex = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t hlen = strlen(hex);
            if (hlen % 2 == 0) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    ByteBuffer *nb = realloc(buffers, capacity * sizeof(ByteBuffer));
                    if (!nb) break;
                    buffers = nb;
                }
                buffers[count].name = strdup(name);
                buffers[count].size = hlen / 2;
                buffers[count].cap = buffers[count].size == 0 ? 4 : buffers[count].size;
                buffers[count].bytes = malloc(buffers[count].cap);
                for (size_t i = 0; i < buffers[count].size; i++) {
                    buffers[count].bytes[i] = parse_hex_pair(hex[i * 2], hex[i * 2 + 1]);
                }
                count++;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *hex = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            size_t hlen = strlen(hex);
            if (hlen % 2 == 0) {
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(buffers[i].name, name) == 0) {
                        ByteBuffer *b = &buffers[i];
                        size_t added = hlen / 2;
                        if (b->size + added > b->cap) {
                            b->cap = b->size + added + 4;
                            unsigned char *nb = realloc(b->bytes, b->cap);
                            if (!nb) break;
                            b->bytes = nb;
                        }
                        for (size_t j = 0; j < added; j++) {
                            b->bytes[b->size + j] = parse_hex_pair(hex[j * 2], hex[j * 2 + 1]);
                        }
                        b->size += added;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *off_str = p;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *len_str = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            int offset = atoi(off_str);
            int length = atoi(len_str);
            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    if (offset >= 0 && length >= 0 && (size_t)offset + length <= buffers[i].size) {
                        for (int j = 0; j < length; j++) {
                            printf("%02X", buffers[i].bytes[offset + j]);
                        }
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    free(buffers[i].name);
                    free(buffers[i].bytes);
                    for (size_t j = i; j < count - 1; j++) {
                        buffers[j] = buffers[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %d\n", buffers[i].name, (int)buffers[i].size);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    return 0;
}
