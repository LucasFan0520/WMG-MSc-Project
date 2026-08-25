// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Buffer { char *name; unsigned char *bytes; int size; struct Buffer *next; } Buffer;
Buffer *buffers = NULL;

Buffer *find_buffer(const char *name) {
    for (Buffer *b = buffers; b; b = b->next)
        if (strcmp(b->name, name) == 0) return b;
    return NULL;
}

int hex_to_byte(const char *s) {
    int val = 0;
    for (int i = 0; i < 2; i++) {
        char c = s[i];
        if (c >= '0' && c <= '9') val = val * 16 + (c - '0');
        else if (c >= 'A' && c <= 'F') val = val * 16 + (c - 'A' + 10);
        else if (c >= 'a' && c <= 'f') val = val * 16 + (c - 'a' + 10);
        else return -1;
    }
    return val;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    unsigned char *bytes = malloc(len / 2);
    if (!bytes) return;
    int ok = 1;
    for (int i = 0; i < len; i += 2) {
        int b = hex_to_byte(hex + i);
        if (b < 0) { ok = 0; break; }
        bytes[i/2] = (unsigned char)b;
    }
    if (!ok) { free(bytes); return; }
    Buffer *b = malloc(sizeof(Buffer));
    if (!b) { free(bytes); return; }
    b->name = strdup(name);
    b->bytes = bytes;
    b->size = len / 2;
    b->next = buffers;
    buffers = b;
}

void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    unsigned char *newbytes = realloc(b->bytes, b->size + len/2);
    if (!newbytes) return;
    b->bytes = newbytes;
    int ok = 1;
    for (int i = 0; i < len; i += 2) {
        int val = hex_to_byte(hex + i);
        if (val < 0) { ok = 0; break; }
        b->bytes[b->size + i/2] = (unsigned char)val;
    }
    if (!ok) {
        // rollback? we already realloced, but we can shrink back? Hard.
        // We'll just leave it but invalid data.
        // Better to check first before realloc.
        // Simpler: check all hex first.
        for (int i = 0; i < len; i += 2) {
            if (hex_to_byte(hex + i) < 0) return;
        }
        // then realloc and append
        unsigned char *tmp = realloc(b->bytes, b->size + len/2);
        if (!tmp) return;
        b->bytes = tmp;
        for (int i = 0; i < len; i += 2) {
            b->bytes[b->size + i/2] = (unsigned char)hex_to_byte(hex + i);
        }
        b->size += len/2;
    }
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= b->size) return;
    if (length < 0 || offset + length > b->size) return;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->bytes[offset + i]);
    }
    putchar('\n');
}

void delete_buffer(const char *name) {
    Buffer *prev = NULL;
    for (Buffer *b = buffers; b; prev = b, b = b->next) {
        if (strcmp(b->name, name) == 0) {
            if (prev) prev->next = b->next; else buffers = b->next;
            free(b->name); free(b->bytes); free(b);
            return;
        }
    }
}

void list_buffers(void) {
    for (Buffer *b = buffers; b; b = b->next)
        printf("%s %d\n", b->name, b->size);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "NEW") == 0) {
            char name[256], hex[4096];
            if (sscanf(rest, "%255s", name) == 1) {
                char *h = rest + strlen(name);
                while (*h == ' ') h++;
                strcpy(hex, h);
                new_buffer(name, hex);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char name[256], hex[4096];
            if (sscanf(rest, "%255s", name) == 1) {
                char *h = rest + strlen(name);
                while (*h == ' ') h++;
                strcpy(hex, h);
                append_buffer(name, hex);
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char name[256]; int offset, length;
            if (sscanf(rest, "%255s %d %d", name, &offset, &length) == 3)
                slice_buffer(name, offset, length);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) delete_buffer(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    while (buffers) { Buffer *b = buffers; buffers = b->next; free(b->name); free(b->bytes); free(b); }
    return 0;
}