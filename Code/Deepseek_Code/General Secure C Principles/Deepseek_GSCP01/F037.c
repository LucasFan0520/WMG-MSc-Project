/* F037.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Buffer {
    char *name;
    unsigned char *bytes;
    int size;
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
    int val = 0;
    for (int i = 0; i < 2; i++) {
        char c = hex[i];
        if (c >= '0' && c <= '9') val = val * 16 + (c - '0');
        else if (c >= 'a' && c <= 'f') val = val * 16 + (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') val = val * 16 + (c - 'A' + 10);
        else return -1;
    }
    return val;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    unsigned char *bytes = malloc(len / 2);
    for (int i = 0; i < len / 2; i++) {
        int b = hex_to_byte(hex + 2 * i);
        if (b < 0) { free(bytes); return; }
        bytes[i] = (unsigned char)b;
    }
    Buffer *b = malloc(sizeof(Buffer));
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
    unsigned char *new_bytes = malloc(b->size + len / 2);
    memcpy(new_bytes, b->bytes, b->size);
    for (int i = 0; i < len / 2; i++) {
        int val = hex_to_byte(hex + 2 * i);
        if (val < 0) { free(new_bytes); return; }
        new_bytes[b->size + i] = (unsigned char)val;
    }
    free(b->bytes);
    b->bytes = new_bytes;
    b->size += len / 2;
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || length < 0 || offset + length > b->size) return;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->bytes[offset + i]);
    }
    putchar('\n');
}

void delete_buffer(const char *name) {
    Buffer *b = buffers, *prev = NULL;
    while (b && strcmp(b->name, name) != 0) {
        prev = b;
        b = b->next;
    }
    if (!b) return;
    if (prev) prev->next = b->next;
    else buffers = b->next;
    free(b->name);
    free(b->bytes);
    free(b);
}

void list_buffers(void) {
    Buffer *b = buffers;
    while (b) {
        printf("%s %d\n", b->name, b->size);
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
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "NEW") == 0) {
            char name[256];
            char *p = line + 4;
            if (sscanf(p, "%255s", name) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            new_buffer(name, p);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char name[256];
            char *p = line + 7;
            if (sscanf(p, "%255s", name) != 1) continue;
            while (*p && *p != ' ') p++;
            while (*p == ' ') p++;
            append_buffer(name, p);
        } else if (strcmp(cmd, "SLICE") == 0) {
            char name[256];
            int offset, length;
            char *p = line + 6;
            if (sscanf(p, "%255s %d %d", name, &offset, &length) == 3)
                slice_buffer(name, offset, length);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_buffer(p);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    free_all();
    free(line);
    return 0;
}