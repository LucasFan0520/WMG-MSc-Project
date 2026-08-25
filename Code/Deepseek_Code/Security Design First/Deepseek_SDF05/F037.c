// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct Buffer {
    char *name;
    unsigned char *bytes;
    size_t size;
    struct Buffer *next;
} Buffer;

Buffer *buffers = NULL;

Buffer *find_buffer(const char *name) {
    Buffer *b = buffers;
    while (b) {
        if (strcmp(b->name, name) == 0) return b;
        b = b->next;
    }
    return NULL;
}

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

int hex_to_byte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

unsigned char *hex_to_bytes(const char *hex, size_t *out_len) {
    size_t len = strlen(hex);
    if (len % 2 != 0) return NULL;
    *out_len = len / 2;
    unsigned char *bytes = malloc(*out_len);
    if (!bytes) return NULL;
    for (size_t i = 0; i < *out_len; i++) {
        int hi = hex_to_byte(hex[2*i]);
        int lo = hex_to_byte(hex[2*i+1]);
        if (hi < 0 || lo < 0) { free(bytes); return NULL; }
        bytes[i] = (hi << 4) | lo;
    }
    return bytes;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    size_t size;
    unsigned char *bytes = hex_to_bytes(hex, &size);
    if (!bytes) return;
    Buffer *b = malloc(sizeof(Buffer));
    if (!b) { free(bytes); return; }
    b->name = safe_strdup(name);
    b->bytes = bytes;
    b->size = size;
    b->next = buffers;
    buffers = b;
}

void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    size_t add_size;
    unsigned char *add_bytes = hex_to_bytes(hex, &add_size);
    if (!add_bytes) return;
    unsigned char *new_bytes = realloc(b->bytes, b->size + add_size);
    if (!new_bytes) { free(add_bytes); return; }
    memcpy(new_bytes + b->size, add_bytes, add_size);
    b->bytes = new_bytes;
    b->size += add_size;
    free(add_bytes);
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) { printf("NOT_FOUND\n"); return; }
    if (offset < 0 || offset >= (int)b->size || length <= 0 || offset + length > (int)b->size) {
        printf("INVALID\n");
        return;
    }
    for (int i = 0; i < length; i++) {
        printf("%02X", b->bytes[offset + i]);
    }
    putchar('\n');
}

void delete_buffer(const char *name) {
    Buffer *prev = NULL, *cur = buffers;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else buffers = cur->next;
            free(cur->name);
            free(cur->bytes);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void list_buffers(void) {
    Buffer *b = buffers;
    while (b) {
        printf("%s %zu\n", b->name, b->size);
        b = b->next;
    }
}

void free_all(void) {
    Buffer *b = buffers;
    while (b) {
        Buffer *next = b->next;
        free(b->name);
        free(b->bytes);
        free(b);
        b = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512];
        int n = sscanf(line, "%31s %511s %511s", cmd, arg1, arg2);
        if (n < 1) continue;

        if (strcmp(cmd, "NEW") == 0 && n >= 3) {
            new_buffer(arg1, arg2);
        } else if (strcmp(cmd, "APPEND") == 0 && n >= 3) {
            append_buffer(arg1, arg2);
        } else if (strcmp(cmd, "SLICE") == 0 && n >= 3) {
            int offset, length;
            if (sscanf(arg2, "%d %d", &offset, &length) == 2) {
                slice_buffer(arg1, offset, length);
            }
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_buffer(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    free_all();
    return 0;
}