// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_NAME 64
#define MAX_BUFFER 4096

typedef struct Buffer {
    char name[MAX_NAME];
    unsigned char data[MAX_BUFFER];
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

void free_all() {
    Buffer *b = head;
    while (b) {
        Buffer *tmp = b;
        b = b->next;
        free(tmp);
    }
    head = NULL;
}

int hex_to_byte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

int parse_hex_bytes(const char *hex, unsigned char *out, size_t max_len, size_t *out_len) {
    int len = strlen(hex);
    if (len % 2 != 0) return 0;
    size_t count = 0;
    for (int i = 0; i < len && count < max_len; i += 2) {
        int hi = hex_to_byte(hex[i]);
        int lo = hex_to_byte(hex[i+1]);
        if (hi < 0 || lo < 0) return 0;
        out[count++] = (hi << 4) | lo;
    }
    *out_len = count;
    return 1;
}

void handle_new(char *line) {
    char name[MAX_NAME], hex[MAX_LINE];
    if (sscanf(line, "NEW %63s %4095s", name, hex) != 2) return;
    if (find_buffer(name)) return;
    unsigned char data[MAX_BUFFER];
    size_t size;
    if (!parse_hex_bytes(hex, data, MAX_BUFFER, &size)) return;
    Buffer *b = malloc(sizeof(Buffer));
    if (!b) return;
    strcpy(b->name, name);
    memcpy(b->data, data, size);
    b->size = size;
    b->next = head;
    head = b;
}

void handle_append(char *line) {
    char name[MAX_NAME], hex[MAX_LINE];
    if (sscanf(line, "APPEND %63s %4095s", name, hex) != 2) return;
    Buffer *b = find_buffer(name);
    if (!b) return;
    unsigned char data[MAX_BUFFER];
    size_t size;
    if (!parse_hex_bytes(hex, data, MAX_BUFFER, &size)) return;
    if (b->size + size > MAX_BUFFER) return;
    memcpy(b->data + b->size, data, size);
    b->size += size;
}

void handle_slice(char *line) {
    char name[MAX_NAME];
    int offset, length;
    if (sscanf(line, "SLICE %63s %d %d", name, &offset, &length) != 3) return;
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= b->size) return;
    if (length < 0) return;
    if (offset + length > b->size) length = b->size - offset;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->data[offset + i]);
    }
    putchar('\n');
}

void handle_delete(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "DELETE %63s", name) != 1) return;
    Buffer *b = head, *prev = NULL;
    while (b) {
        if (strcmp(b->name, name) == 0) {
            if (prev) prev->next = b->next;
            else head = b->next;
            free(b);
            return;
        }
        prev = b;
        b = b->next;
    }
}

void handle_list() {
    Buffer *b = head;
    while (b) {
        printf("%s %zu\n", b->name, b->size);
        b = b->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "NEW ", 4) == 0) handle_new(line);
        else if (strncmp(line, "APPEND ", 7) == 0) handle_append(line);
        else if (strncmp(line, "SLICE ", 6) == 0) handle_slice(line);
        else if (strncmp(line, "DELETE ", 7) == 0) handle_delete(line);
        else if (strcmp(line, "LIST") == 0) handle_list();
    }
    free_all();
    return 0;
}