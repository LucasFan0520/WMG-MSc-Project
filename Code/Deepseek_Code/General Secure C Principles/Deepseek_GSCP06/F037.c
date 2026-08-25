// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Buffer {
    char *name;
    unsigned char *bytes;
    int size; // in bytes
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

int hex_to_byte(const char *hex) {
    int val;
    sscanf(hex, "%2x", &val);
    return val;
}

void new_buffer(const char *name, const char *hexstr) {
    if (find_buffer(name)) return;
    int len = strlen(hexstr);
    if (len % 2 != 0) return;
    Buffer *b = malloc(sizeof(Buffer));
    b->name = strdup(name);
    b->size = len / 2;
    b->bytes = malloc(b->size);
    for (int i = 0; i < b->size; i++) {
        char hex[3] = {hexstr[2*i], hexstr[2*i+1], 0};
        b->bytes[i] = hex_to_byte(hex);
    }
    b->next = buffers;
    buffers = b;
}

void append_bytes(const char *name, const char *hexstr) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    int len = strlen(hexstr);
    if (len % 2 != 0) return;
    int add = len / 2;
    b->bytes = realloc(b->bytes, b->size + add);
    for (int i = 0; i < add; i++) {
        char hex[3] = {hexstr[2*i], hexstr[2*i+1], 0};
        b->bytes[b->size + i] = hex_to_byte(hex);
    }
    b->size += add;
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= b->size || length < 0 || offset + length > b->size) return;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->bytes[offset + i]);
    }
    putchar('\n');
}

void delete_buffer(const char *name) {
    Buffer *cur = buffers, *prev = NULL;
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
        printf("%s %d\n", b->name, b->size);
        b = b->next;
    }
}

void cleanup_buffers(void) {
    Buffer *b = buffers;
    while (b) {
        Buffer *tmp = b;
        b = b->next;
        free(tmp->name);
        free(tmp->bytes);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], rest[4096];
        int n = sscanf(line, "%31s %255s %4095s", cmd, name, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "NEW") == 0 && n >= 3) {
            new_buffer(name, rest);
        } else if (strcmp(cmd, "APPEND") == 0 && n >= 3) {
            append_bytes(name, rest);
        } else if (strcmp(cmd, "SLICE") == 0 && n >= 3) {
            int offset, length;
            if (sscanf(rest, "%d %d", &offset, &length) == 2) {
                slice_buffer(name, offset, length);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_buffer(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    cleanup_buffers();
    return 0;
}