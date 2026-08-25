// F037.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Buffer {
    char *name;
    unsigned char *bytes;
    size_t size;
} Buffer;

static Buffer *buffers = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_buffer(const char *name) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(buffers[i].name, name) == 0)
            return (ssize_t)i;
    return -1;
}

static unsigned char hex_to_byte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

static int parse_hex(const char *hex, unsigned char **out, size_t *out_len) {
    size_t len = strlen(hex);
    if (len % 2 != 0) return 0;
    *out_len = len / 2;
    *out = malloc(*out_len);
    if (!*out) { fprintf(stderr, "Memory error\n"); exit(1); }
    for (size_t i = 0; i < *out_len; i++) {
        (*out)[i] = (hex_to_byte(hex[2*i]) << 4) | hex_to_byte(hex[2*i+1]);
    }
    return 1;
}

static void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name) != -1) return;
    unsigned char *bytes;
    size_t size;
    if (!parse_hex(hex, &bytes, &size)) return;
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Buffer *new = realloc(buffers, capacity * sizeof(Buffer));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        buffers = new;
    }
    buffers[count].name = strdup(name);
    if (!buffers[count].name) { fprintf(stderr, "Memory error\n"); exit(1); }
    buffers[count].bytes = bytes;
    buffers[count].size = size;
    count++;
}

static void append_buffer(const char *name, const char *hex) {
    ssize_t idx = find_buffer(name);
    if (idx == -1) return;
    unsigned char *new_bytes;
    size_t new_size;
    if (!parse_hex(hex, &new_bytes, &new_size)) return;
    size_t old_size = buffers[idx].size;
    unsigned char *combined = malloc(old_size + new_size);
    if (!combined) { fprintf(stderr, "Memory error\n"); exit(1); }
    memcpy(combined, buffers[idx].bytes, old_size);
    memcpy(combined + old_size, new_bytes, new_size);
    free(buffers[idx].bytes);
    free(new_bytes);
    buffers[idx].bytes = combined;
    buffers[idx].size = old_size + new_size;
}

static void slice_buffer(const char *name, size_t offset, size_t length) {
    ssize_t idx = find_buffer(name);
    if (idx == -1) return;
    if (offset >= buffers[idx].size || offset + length > buffers[idx].size) return;
    for (size_t i = 0; i < length; i++)
        printf("%02x", buffers[idx].bytes[offset + i]);
    putchar('\n');
}

static void delete_buffer(const char *name) {
    ssize_t idx = find_buffer(name);
    if (idx == -1) return;
    free(buffers[idx].name);
    free(buffers[idx].bytes);
    for (size_t i = idx; i < count - 1; i++)
        buffers[i] = buffers[i + 1];
    count--;
}

static void list_buffers(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %zu\n", buffers[i].name, buffers[i].size);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], name[256], hex[8192];
    size_t offset, length;

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "NEW") == 0) {
            if (sscanf(line, "%*s %255s %8191[^\n]", name, hex) == 2)
                new_buffer(name, hex);
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(line, "%*s %255s %8191[^\n]", name, hex) == 2)
                append_buffer(name, hex);
        } else if (strcmp(cmd, "SLICE") == 0) {
            if (sscanf(line, "%*s %255s %zu %zu", name, &offset, &length) == 3)
                slice_buffer(name, offset, length);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_buffer(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }

    free(line);
    free_all();
    return 0;
}