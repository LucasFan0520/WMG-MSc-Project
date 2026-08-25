// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Buffer {
    char *name;
    unsigned char *data;
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

int hex_to_byte(const char *s) {
    if (!isxdigit(s[0]) || !isxdigit(s[1])) return -1;
    int hi = isdigit(s[0]) ? s[0] - '0' : tolower(s[0]) - 'a' + 10;
    int lo = isdigit(s[1]) ? s[1] - '0' : tolower(s[1]) - 'a' + 10;
    return hi * 16 + lo;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    unsigned char *data = malloc(len / 2);
    int valid = 1;
    for (int i = 0; i < len; i += 2) {
        int b = hex_to_byte(hex + i);
        if (b < 0) { valid = 0; break; }
        data[i/2] = b;
    }
    if (!valid) { free(data); return; }
    Buffer *b = malloc(sizeof(Buffer));
    b->name = strdup(name);
    b->data = data;
    b->size = len / 2;
    b->next = buffers;
    buffers = b;
}

void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    unsigned char *newdata = malloc(b->size + len/2);
    memcpy(newdata, b->data, b->size);
    int valid = 1;
    for (int i = 0; i < len; i += 2) {
        int byte = hex_to_byte(hex + i);
        if (byte < 0) { valid = 0; break; }
        newdata[b->size + i/2] = byte;
    }
    if (!valid) { free(newdata); return; }
    free(b->data);
    b->data = newdata;
    b->size += len/2;
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= b->size) return;
    if (length < 0 || offset + length > b->size) return;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->data[offset + i]);
    }
    putchar('\n');
}

void delete_buffer(const char *name) {
    Buffer *prev = NULL;
    Buffer *b = buffers;
    while (b) {
        if (strcmp(b->name, name) == 0) {
            if (prev) prev->next = b->next;
            else buffers = b->next;
            free(b->name);
            free(b->data);
            free(b);
            return;
        }
        prev = b;
        b = b->next;
    }
}

void list_buffers() {
    Buffer *b = buffers;
    while (b) {
        printf("%s:%d\n", b->name, b->size);
        b = b->next;
    }
}

void free_all() {
    Buffer *b = buffers;
    while (b) {
        Buffer *next = b->next;
        free(b->name);
        free(b->data);
        free(b);
        b = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "NEW") == 0) {
            char name[256], hex[4096];
            if (sscanf(rest, "%255s %[^\n]", name, hex) == 2) {
                new_buffer(name, hex);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char name[256], hex[4096];
            if (sscanf(rest, "%255s %[^\n]", name, hex) == 2) {
                append_buffer(name, hex);
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char name[256];
            int offset, length;
            if (sscanf(rest, "%255s %d %d", name, &offset, &length) == 3) {
                slice_buffer(name, offset, length);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_buffer(rest);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    free_all();
    return 0;
}