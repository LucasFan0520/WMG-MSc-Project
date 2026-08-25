// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Buffer {
    char *name;
    unsigned char *data;
    size_t size;
    size_t capacity;
    struct Buffer *next;
} Buffer;

Buffer *buffers = NULL;

int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + c - 'a';
    if (c >= 'A' && c <= 'F') return 10 + c - 'A';
    return -1;
}

size_t parse_hex(const char *hex, unsigned char **out) {
    size_t len = strlen(hex);
    if (len % 2 != 0) return 0;
    size_t bytes = len / 2;
    unsigned char *buf = malloc(bytes);
    if (!buf) return 0;
    for (size_t i = 0; i < bytes; i++) {
        int h = hex_val(hex[2*i]);
        int l = hex_val(hex[2*i+1]);
        if (h < 0 || l < 0) { free(buf); return 0; }
        buf[i] = (h << 4) | l;
    }
    *out = buf;
    return bytes;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "NEW") == 0) {
            char name[256], hex[MAX_LINE];
            if (sscanf(rest, "%255s %4095s", name, hex) != 2) continue;
            unsigned char *data = NULL;
            size_t size = parse_hex(hex, &data);
            if (!data && size == 0 && strlen(hex) > 0) continue;
            Buffer *b = malloc(sizeof(Buffer));
            if (!b) { free(data); continue; }
            b->name = strdup(name);
            if (!b->name) { free(data); free(b); continue; }
            b->data = data;
            b->size = size;
            b->capacity = size;
            b->next = buffers;
            buffers = b;
        } else if (strcmp(cmd, "APPEND") == 0) {
            char name[256], hex[MAX_LINE];
            if (sscanf(rest, "%255s %4095s", name, hex) != 2) continue;
            unsigned char *data = NULL;
            size_t size = parse_hex(hex, &data);
            if (!data && size == 0 && strlen(hex) > 0) continue;
            for (Buffer *b = buffers; b; b = b->next) {
                if (strcmp(b->name, name) == 0) {
                    size_t new_size = b->size + size;
                    if (new_size > b->capacity) {
                        size_t new_cap = new_size * 2;
                        unsigned char *tmp = realloc(b->data, new_cap);
                        if (!tmp) { free(data); break; }
                        b->data = tmp;
                        b->capacity = new_cap;
                    }
                    memcpy(b->data + b->size, data, size);
                    b->size = new_size;
                    break;
                }
            }
            free(data);
        } else if (strcmp(cmd, "SLICE") == 0) {
            char name[256];
            int offset, length;
            if (sscanf(rest, "%255s %d %d", name, &offset, &length) != 3) continue;
            if (offset < 0 || length < 0) continue;
            for (Buffer *b = buffers; b; b = b->next) {
                if (strcmp(b->name, name) == 0) {
                    if ((size_t)offset + length > b->size) break;
                    for (int i = 0; i < length; i++) {
                        printf("%02x", b->data[offset + i]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Buffer *prev = NULL;
            for (Buffer *b = buffers; b; prev = b, b = b->next) {
                if (strcmp(b->name, name) == 0) {
                    if (prev) prev->next = b->next;
                    else buffers = b->next;
                    free(b->name); free(b->data); free(b);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Buffer *b = buffers; b; b = b->next) {
                printf("%s %zu\n", b->name, b->size);
            }
        }
    }
    while (buffers) {
        Buffer *b = buffers;
        buffers = buffers->next;
        free(b->name); free(b->data); free(b);
    }
    return 0;
}