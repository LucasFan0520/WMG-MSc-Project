// F037.c
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

Buffer *head = NULL, *tail = NULL;

Buffer *find_buffer(const char *name) {
    Buffer *b = head;
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
        else if (c >= 'A' && c <= 'F') val = val * 16 + (c - 'A' + 10);
        else if (c >= 'a' && c <= 'f') val = val * 16 + (c - 'a' + 10);
        else return -1;
    }
    return val;
}

unsigned char *parse_hex(const char *hex, int *out_len) {
    int len = strlen(hex);
    if (len % 2 != 0) return NULL;
    unsigned char *bytes = malloc(len / 2);
    for (int i = 0; i < len; i += 2) {
        int b = hex_to_byte(hex + i);
        if (b < 0) { free(bytes); return NULL; }
        bytes[i / 2] = (unsigned char)b;
    }
    *out_len = len / 2;
    return bytes;
}

void new_buffer(const char *name, const char *hex) {
    int len;
    unsigned char *bytes = parse_hex(hex, &len);
    if (!bytes) return;
    Buffer *b = malloc(sizeof(Buffer));
    b->name = malloc(strlen(name) + 1);
    strcpy(b->name, name);
    b->bytes = bytes;
    b->size = len;
    b->next = NULL;
    if (tail) {
        tail->next = b;
        tail = b;
    } else {
        head = tail = b;
    }
}

void append_bytes(const char *name, const char *hex) {
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
        if (i < length - 1) putchar(' ');
    }
    putchar('\n');
}

void delete_buffer(const char *name) {
    Buffer *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            free(curr->name);
            free(curr->bytes);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void list_buffers(void) {
    Buffer *b = head;
    while (b) {
        printf("%s %d\n", b->name, b->size);
        b = b->next;
    }
}

void free_all(void) {
    Buffer *b = head;
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
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "NEW") == 0) {
            char *name = strtok(NULL, " ");
            char *hex = strtok(NULL, " ");
            if (name && hex) new_buffer(name, hex);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *name = strtok(NULL, " ");
            char *hex = strtok(NULL, " ");
            if (name && hex) append_bytes(name, hex);
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *name = strtok(NULL, " ");
            char *off = strtok(NULL, " ");
            char *len = strtok(NULL, " ");
            if (name && off && len) slice_buffer(name, atoi(off), atoi(len));
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_buffer(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    free(line);
    free_all();
    return 0;
}