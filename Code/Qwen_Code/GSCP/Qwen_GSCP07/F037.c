// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Buffer {
    char *name;
    unsigned char *data;
    size_t size;
    struct Buffer *next;
} Buffer;

Buffer *buffers = NULL;

int parse_hex(const char *hex, unsigned char **out, size_t *out_size) {
    size_t len = strlen(hex);
    if (len % 2 != 0) return 0;
    *out_size = len / 2;
    *out = malloc(*out_size);
    for (size_t i = 0; i < *out_size; i++) {
        char byte_str[3] = {hex[i*2], hex[i*2+1], 0};
        char *end;
        long val = strtol(byte_str, &end, 16);
        if (*end != 0 || val < 0 || val > 255) {
            free(*out);
            return 0;
        }
        (*out)[i] = (unsigned char)val;
    }
    return 1;
}

void new_buffer(const char *name, const char *hex) {
    unsigned char *data;
    size_t size;
    if (!parse_hex(hex, &data, &size)) return;
    Buffer *b = malloc(sizeof(Buffer));
    b->name = strdup(name);
    b->data = data;
    b->size = size;
    b->next = buffers;
    buffers = b;
}

void append_buffer(const char *name, const char *hex) {
    for (Buffer *b = buffers; b; b = b->next) {
        if (strcmp(b->name, name) == 0) {
            unsigned char *data;
            size_t size;
            if (!parse_hex(hex, &data, &size)) return;
            b->data = realloc(b->data, b->size + size);
            memcpy(b->data + b->size, data, size);
            b->size += size;
            free(data);
            return;
        }
    }
}

void slice_buffer(const char *name, int offset, int length) {
    for (Buffer *b = buffers; b; b = b->next) {
        if (strcmp(b->name, name) == 0) {
            if (offset < 0 || length < 0 || offset + length > (int)b->size) return;
            for (int i = 0; i < length; i++) {
                printf("%02x", b->data[offset + i]);
            }
            printf("\n");
            return;
        }
    }
}

void delete_buffer(const char *name) {
    Buffer **bp = &buffers;
    while (*bp) {
        if (strcmp((*bp)->name, name) == 0) {
            Buffer *tmp = *bp;
            *bp = tmp->next;
            free(tmp->name);
            free(tmp->data);
            free(tmp);
            return;
        }
        bp = &(*bp)->next;
    }
}

void list_buffers() {
    for (Buffer *b = buffers; b; b = b->next) {
        printf("%s %zu\n", b->name, b->size);
    }
}

void cleanup() {
    while (buffers) {
        Buffer *tmp = buffers->next;
        free(buffers->name);
        free(buffers->data);
        free(buffers);
        buffers = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[8192];
        int offset, length;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "NEW") == 0) {
            if (sscanf(line, "%*s %255s %8191s", a1, a2) == 2) new_buffer(a1, a2);
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(line, "%*s %255s %8191s", a1, a2) == 2) append_buffer(a1, a2);
        } else if (strcmp(cmd, "SLICE") == 0) {
            if (sscanf(line, "%*s %255s %d %d", a1, &offset, &length) == 3) slice_buffer(a1, offset, length);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) delete_buffer(a1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    cleanup();
    return 0;
}