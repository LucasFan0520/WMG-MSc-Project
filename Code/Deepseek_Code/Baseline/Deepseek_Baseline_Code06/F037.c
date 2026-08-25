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

Buffer *head = NULL;

Buffer* find_buffer(const char *name) {
    Buffer *b = head;
    while (b) {
        if (strcmp(b->name, name) == 0) return b;
        b = b->next;
    }
    return NULL;
}

int hex_to_byte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

unsigned char* parse_hex(const char *hex, int *out_len) {
    int len = strlen(hex);
    if (len % 2 != 0) { *out_len = 0; return NULL; }
    unsigned char *bytes = malloc(len / 2);
    int idx = 0;
    for (int i = 0; i < len; i += 2) {
        int hi = hex_to_byte(hex[i]);
        int lo = hex_to_byte(hex[i+1]);
        if (hi < 0 || lo < 0) { free(bytes); *out_len = 0; return NULL; }
        bytes[idx++] = (hi << 4) | lo;
    }
    *out_len = idx;
    return bytes;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    int len;
    unsigned char *bytes = parse_hex(hex, &len);
    if (!bytes) return;
    Buffer *b = malloc(sizeof(Buffer));
    b->name = strdup(name);
    b->bytes = bytes;
    b->size = len;
    b->next = head;
    head = b;
}

void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    int len;
    unsigned char *bytes = parse_hex(hex, &len);
    if (!bytes) return;
    b->bytes = realloc(b->bytes, b->size + len);
    memcpy(b->bytes + b->size, bytes, len);
    b->size += len;
    free(bytes);
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= b->size || length <= 0 || offset + length > b->size) return;
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
            free(b->name);
            free(b->bytes);
            free(b);
            return;
        }
        prev = b;
        b = b->next;
    }
}

void list_buffers() {
    Buffer *b = head;
    while (b) {
        printf("%s %d\n", b->name, b->size);
        b = b->next;
    }
}

void free_all() {
    while (head) {
        Buffer *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp->bytes);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], hex[4096];
        int offset, length;
        int n = sscanf(line, "%31s %255s %s", cmd, name, hex);
        if (strcmp(cmd, "NEW") == 0 && n == 3) {
            new_buffer(name, hex);
        } else if (strcmp(cmd, "APPEND") == 0 && n == 3) {
            append_buffer(name, hex);
        } else if (strcmp(cmd, "SLICE") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &offset, &length) == 3) {
                slice_buffer(name, offset, length);
            }
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_buffer(name);
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_buffers();
        }
    }
    free_all();
    return 0;
}