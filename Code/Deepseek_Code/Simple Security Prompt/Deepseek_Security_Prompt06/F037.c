// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

unsigned char hex_to_byte(const char *s) {
    unsigned char val = 0;
    for (int i = 0; i < 2; i++) {
        char c = s[i];
        if (c >= '0' && c <= '9') val = val * 16 + (c - '0');
        else if (c >= 'A' && c <= 'F') val = val * 16 + (c - 'A' + 10);
        else if (c >= 'a' && c <= 'f') val = val * 16 + (c - 'a' + 10);
        else return 0xFF; // invalid
    }
    return val;
}

void new_buffer(const char *name, const char *hexstr) {
    if (find_buffer(name)) return;
    size_t len = strlen(hexstr);
    if (len % 2 != 0) return;
    size_t num_bytes = len / 2;
    unsigned char *data = malloc(num_bytes);
    int valid = 1;
    for (size_t i = 0; i < num_bytes; i++) {
        unsigned char b = hex_to_byte(hexstr + 2*i);
        if (b == 0xFF) { valid = 0; break; }
        data[i] = b;
    }
    if (!valid) { free(data); return; }
    Buffer *b = malloc(sizeof(Buffer));
    b->name = malloc(strlen(name) + 1);
    strcpy(b->name, name);
    b->bytes = data;
    b->size = num_bytes;
    b->next = buffers;
    buffers = b;
}

void append_buffer(const char *name, const char *hexstr) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    size_t len = strlen(hexstr);
    if (len % 2 != 0) return;
    size_t num_bytes = len / 2;
    unsigned char *data = malloc(num_bytes);
    int valid = 1;
    for (size_t i = 0; i < num_bytes; i++) {
        unsigned char by = hex_to_byte(hexstr + 2*i);
        if (by == 0xFF) { valid = 0; break; }
        data[i] = by;
    }
    if (!valid) { free(data); return; }
    b->bytes = realloc(b->bytes, b->size + num_bytes);
    memcpy(b->bytes + b->size, data, num_bytes);
    b->size += num_bytes;
    free(data);
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= (int)b->size) return;
    if (length < 0 || offset + length > (int)b->size) return;
    for (int i = 0; i < length; i++) {
        printf("%02X", b->bytes[offset + i]);
    }
    putchar('\n');
}

void delete_buffer(const char *name) {
    Buffer *b = buffers, *prev = NULL;
    while (b) {
        if (strcmp(b->name, name) == 0) {
            if (prev) prev->next = b->next;
            else buffers = b->next;
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
    Buffer *b = buffers;
    while (b) {
        printf("%s %zu\n", b->name, b->size);
        b = b->next;
    }
}

void free_all(void) {
    Buffer *b = buffers;
    while (b) {
        Buffer *tmp = b->next;
        free(b->name);
        free(b->bytes);
        free(b);
        b = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], hexstr[4096];
        int offset, length;
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "NEW") == 0) {
            if (sscanf(rest, "%255s", name) == 1) {
                const char *h = rest + strlen(name) + 1;
                while (*h == ' ') h++;
                if (*h) new_buffer(name, h);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(rest, "%255s", name) == 1) {
                const char *h = rest + strlen(name) + 1;
                while (*h == ' ') h++;
                if (*h) append_buffer(name, h);
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            if (sscanf(rest, "%255s %d %d", name, &offset, &length) == 3) {
                slice_buffer(name, offset, length);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(rest, "%255s", name) == 1) delete_buffer(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    free_all();
    return 0;
}