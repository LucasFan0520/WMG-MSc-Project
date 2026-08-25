/* F037.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct buffer {
    char *name;
    unsigned char *bytes;
    size_t size;
    struct buffer *next;
} buffer_t;

buffer_t *head = NULL;
buffer_t *tail = NULL;

buffer_t *find_buffer(const char *name) {
    buffer_t *b = head;
    while (b) {
        if (strcmp(b->name, name) == 0) return b;
        b = b->next;
    }
    return NULL;
}

int hex_to_byte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

unsigned char *parse_hex(const char *hex, size_t *out_len) {
    size_t len = strlen(hex);
    if (len % 2 != 0) { *out_len = 0; return NULL; }
    unsigned char *data = malloc(len / 2);
    if (!data) { *out_len = 0; return NULL; }
    for (size_t i = 0; i < len / 2; i++) {
        int hi = hex_to_byte(hex[2*i]);
        int lo = hex_to_byte(hex[2*i+1]);
        if (hi < 0 || lo < 0) {
            free(data);
            *out_len = 0;
            return NULL;
        }
        data[i] = (hi << 4) | lo;
    }
    *out_len = len / 2;
    return data;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    size_t size;
    unsigned char *data = parse_hex(hex, &size);
    if (!data) return;
    buffer_t *b = malloc(sizeof(buffer_t));
    if (!b) { free(data); return; }
    b->name = strdup(name);
    b->bytes = data;
    b->size = size;
    b->next = NULL;
    if (!b->name) { free(b->name); free(b->bytes); free(b); return; }
    if (tail) tail->next = b;
    else head = b;
    tail = b;
}

void append_buffer(const char *name, const char *hex) {
    buffer_t *b = find_buffer(name);
    if (!b) return;
    size_t size;
    unsigned char *data = parse_hex(hex, &size);
    if (!data) return;
    unsigned char *new = realloc(b->bytes, b->size + size);
    if (!new) { free(data); return; }
    memcpy(new + b->size, data, size);
    b->bytes = new;
    b->size += size;
    free(data);
}

void slice_buffer(const char *name, int offset, int length) {
    buffer_t *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= (int)b->size) return;
    if (length < 0) return;
    int end = offset + length;
    if (end > (int)b->size) end = b->size;
    for (int i = offset; i < end; i++) {
        printf("%02X", b->bytes[i]);
    }
    putchar('\n');
}

void delete_buffer(const char *name) {
    buffer_t *prev = NULL, *b = head;
    while (b) {
        if (strcmp(b->name, name) == 0) {
            if (prev) prev->next = b->next;
            else head = b->next;
            if (b == tail) tail = prev;
            free(b->name);
            free(b->bytes);
            free(b);
            return;
        }
        prev = b;
        b = b->next;
    }
}

void list_buffers(void) {
    buffer_t *b = head;
    while (b) {
        printf("%s %zu\n", b->name, b->size);
        b = b->next;
    }
}

void free_all(void) {
    buffer_t *b = head;
    while (b) {
        buffer_t *nxt = b->next;
        free(b->name);
        free(b->bytes);
        free(b);
        b = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256], b[256];
        int offset, length;
        if (sscanf(line, "%9s %255s %d %d %255s", cmd, a, &offset, &length, b) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "NEW") == 0) {
                new_buffer(a, rest);
            } else if (strcmp(cmd, "APPEND") == 0) {
                append_buffer(a, rest);
            } else if (strcmp(cmd, "SLICE") == 0) {
                if (sscanf(line, "%*s %255s %d %d", a, &offset, &length) == 3)
                    slice_buffer(a, offset, length);
            } else if (strcmp(cmd, "DELETE") == 0) {
                delete_buffer(a);
            } else if (strcmp(cmd, "LIST") == 0) {
                list_buffers();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}