// F037.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    size_t size;
    size_t capacity;
} ByteBuffer;

int main(void) {
    ByteBuffer *buffers = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "NEW") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *hex = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*name == '\0') continue;

            size_t hex_len = strlen(hex);
            size_t b_size = hex_len / 2;
            unsigned char *buf = malloc(b_size + 1);
            if (!buf) continue;

            size_t actual = 0;
            for (size_t i = 0; i < b_size; i++) {
                char tmp[3] = { hex[i*2], hex[i*2+1], '\0' };
                unsigned int val;
                if (sscanf(tmp, "%x", &val) == 1) {
                    buf[actual++] = (unsigned char)val;
                }
            }

            if (count >= capacity) {
                size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                ByteBuffer *new_bufs = realloc(buffers, new_cap * sizeof(ByteBuffer));
                if (new_bufs) {
                    buffers = new_bufs;
                    capacity = new_cap;
                } else {
                    free(buf);
                    continue;
                }
            }
            char *nm = strdup(name);
            if (nm) {
                buffers[count].name = nm;
                buffers[count].bytes = buf;
                buffers[count].size = actual;
                buffers[count].capacity = b_size + 1;
                count++;
            } else {
                free(buf);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *hex = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    ByteBuffer *b = &buffers[i];
                    size_t hex_len = strlen(hex);
                    size_t b_size = hex_len / 2;
                    if (b->size + b_size >= b->capacity) {
                        size_t new_cap = b->capacity + b_size + 4;
                        unsigned char *new_bytes = realloc(b->bytes, new_cap);
                        if (!new_bytes) break;
                        b->bytes = new_bytes;
                        b->capacity = new_cap;
                    }
                    for (size_t j = 0; j < b_size; j++) {
                        char tmp[3] = { hex[j*2], hex[j*2+1], '\0' };
                        unsigned int val;
                        if (sscanf(tmp, "%x", &val) == 1) {
                            b->bytes[b->size++] = (unsigned char)val;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *off_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *len_str = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*name == '\0' || *off_str == '\0' || *len_str == '\0') continue;
            long offset = strtol(off_str, NULL, 10);
            long length = strtol(len_str, NULL, 10);

            for (size_t i = 0; i < count; i++) {
                if (strcmp(buffers[i].name, name) == 0) {
                    ByteBuffer *b = &buffers[i];
                    if (offset >= 0 && length > 0 && (size_t)offset < b->size) {
                        size_t limit = (size_t)offset + (size_t)length;
                        if (limit > b->size) limit = b->size;
                        for (size_t j = (size_t)offset; j < limit; j++) {
                            printf("%02x", b->bytes[j]);
                        }
                        printf("\n");
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

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
                printf("%s %zu\n", buffers[i].name, buffers[i].size);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    free(line);
    return 0;
}
