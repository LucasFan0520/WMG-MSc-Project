// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Buffer {
    char *name;
    unsigned char *bytes;
    size_t size;
    struct Buffer *next;
} Buffer;

Buffer *head = NULL;

Buffer *find_buffer(const char *name) {
    Buffer *b = head;
    while (b) {
        if (strcmp(b->name, name) == 0) return b;
        b = b->next;
    }
    return NULL;
}

void free_buffer(Buffer *b) {
    if (!b) return;
    free(b->name);
    free(b->bytes);
    free(b);
}

unsigned char hex_to_byte(const char *s) {
    unsigned char val = 0;
    for (int i = 0; i < 2 && s[i]; i++) {
        char c = s[i];
        if (c >= '0' && c <= '9') val = val * 16 + (c - '0');
        else if (c >= 'a' && c <= 'f') val = val * 16 + (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') val = val * 16 + (c - 'A' + 10);
        else return 0; // invalid
    }
    return val;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    size_t len = strlen(hex);
    if (len % 2 != 0) return;
    size_t byte_len = len / 2;
    if (byte_len == 0) return;
    Buffer *b = malloc(sizeof(Buffer));
    if (!b) return;
    b->name = strdup(name);
    b->bytes = malloc(byte_len);
    b->size = byte_len;
    b->next = NULL;
    if (!b->name || !b->bytes) {
        free(b->name); free(b->bytes); free(b);
        return;
    }
    for (size_t i = 0; i < byte_len; i++) {
        b->bytes[i] = hex_to_byte(hex + 2*i);
    }
    if (!head) head = b;
    else {
        Buffer *last = head;
        while (last->next) last = last->next;
        last->next = b;
    }
}

void append_bytes(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    size_t len = strlen(hex);
    if (len % 2 != 0) return;
    size_t byte_len = len / 2;
    if (byte_len == 0) return;
    unsigned char *newbytes = realloc(b->bytes, b->size + byte_len);
    if (!newbytes) return;
    b->bytes = newbytes;
    for (size_t i = 0; i < byte_len; i++) {
        b->bytes[b->size + i] = hex_to_byte(hex + 2*i);
    }
    b->size += byte_len;
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= (int)b->size || length <= 0) return;
    if (offset + length > (int)b->size) length = b->size - offset;
    for (int i = 0; i < length; i++) {
        printf("%02X", b->bytes[offset + i]);
    }
    printf("\n");
}

void delete_buffer(const char *name) {
    Buffer *b = head, *prev = NULL;
    while (b) {
        if (strcmp(b->name, name) == 0) {
            if (prev) prev->next = b->next;
            else head = b->next;
            free_buffer(b);
            return;
        }
        prev = b;
        b = b->next;
    }
}

void list_buffers(void) {
    Buffer *b = head;
    while (b) {
        printf("%s %zu\n", b->name, b->size);
        b = b->next;
    }
}

void free_all(void) {
    Buffer *b = head;
    while (b) {
        Buffer *next = b->next;
        free_buffer(b);
        b = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], name[256], hex[4096];
        int offset, length;
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "NEW") == 0) {
            if (sscanf(line, "%*s %255s %4095s", name, hex) == 2)
                new_buffer(name, hex);
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(line, "%*s %255s %4095s", name, hex) == 2)
                append_bytes(name, hex);
        } else if (strcmp(cmd, "SLICE") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &offset, &length) == 3)
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