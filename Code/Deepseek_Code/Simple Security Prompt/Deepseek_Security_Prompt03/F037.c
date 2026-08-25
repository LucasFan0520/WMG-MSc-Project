/*
 * F037.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct buffer {
    char *name;
    unsigned char *bytes;
    int size;
    struct buffer *next;
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

int hex_to_byte(const char *s) {
    if (!isxdigit(s[0]) || !isxdigit(s[1])) return -1;
    int high = isdigit(s[0]) ? s[0]-'0' : tolower(s[0])-'a'+10;
    int low = isdigit(s[1]) ? s[1]-'0' : tolower(s[1])-'a'+10;
    return high*16 + low;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    int size = len / 2;
    unsigned char *bytes = malloc(size);
    for (int i = 0; i < size; i++) {
        int byte = hex_to_byte(hex + 2*i);
        if (byte < 0) { free(bytes); return; }
        bytes[i] = (unsigned char)byte;
    }
    Buffer *b = malloc(sizeof(Buffer));
    b->name = strdup(name);
    b->bytes = bytes;
    b->size = size;
    b->next = buffers;
    buffers = b;
}

void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    int add_size = len / 2;
    unsigned char *add = malloc(add_size);
    for (int i = 0; i < add_size; i++) {
        int byte = hex_to_byte(hex + 2*i);
        if (byte < 0) { free(add); return; }
        add[i] = (unsigned char)byte;
    }
    int new_size = b->size + add_size;
    unsigned char *new_bytes = realloc(b->bytes, new_size);
    if (!new_bytes) { free(add); return; }
    memcpy(new_bytes + b->size, add, add_size);
    b->bytes = new_bytes;
    b->size = new_size;
    free(add);
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || length < 0 || offset + length > b->size) return;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->bytes[offset + i]);
    }
    printf("\n");
}

void delete_buffer(const char *name) {
    Buffer *prev = NULL, *b = buffers;
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
        printf("%s %d\n", b->name, b->size);
        b = b->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], hex[4096];
        if (sscanf(line, "%31s %255s", cmd, name) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(name) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "NEW") == 0) {
            new_buffer(name, rest);
        } else if (strcmp(cmd, "APPEND") == 0) {
            append_buffer(name, rest);
        } else if (strcmp(cmd, "SLICE") == 0) {
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
    return 0;
}