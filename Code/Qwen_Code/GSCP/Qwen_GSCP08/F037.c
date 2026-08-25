// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Buffer {
    char *name;
    unsigned char *data;
    int size;
    struct Buffer *next;
} Buffer;

Buffer *head = NULL;

Buffer *find_buffer(char *name) {
    for (Buffer *b = head; b; b = b->next) {
        if (strcmp(b->name, name) == 0) return b;
    }
    return NULL;
}

int parse_hex(char *hex, unsigned char **out) {
    int len = strlen(hex);
    if (len % 2 != 0) return 0;
    int bytes = len / 2;
    *out = malloc(bytes);
    for (int i = 0; i < bytes; i++) {
        unsigned int val;
        if (sscanf(hex + i * 2, "%2x", &val) != 1) {
            free(*out);
            return 0;
        }
        (*out)[i] = val;
    }
    return bytes;
}

void new_buffer(char *name, char *hex) {
    if (find_buffer(name)) return;
    unsigned char *data;
    int size = parse_hex(hex, &data);
    if (size == 0) return;
    Buffer *b = malloc(sizeof(Buffer));
    b->name = strdup(name);
    b->data = data;
    b->size = size;
    b->next = head;
    head = b;
}

void append_buffer(char *name, char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    unsigned char *data;
    int size = parse_hex(hex, &data);
    if (size == 0) return;
    b->data = realloc(b->data, b->size + size);
    memcpy(b->data + b->size, data, size);
    b->size += size;
    free(data);
}

void slice_buffer(char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b || offset < 0 || length < 0 || offset + length > b->size) return;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->data[offset + i]);
    }
    printf("\n");
}

void delete_buffer(char *name) {
    Buffer *prev = NULL;
    Buffer *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->data);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void list_buffers() {
    for (Buffer *b = head; b; b = b->next) {
        printf("%s %d\n", b->name, b->size);
    }
}

int main() {
    char cmd[20], a[100], hex[2000];
    int off, len;
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "NEW") == 0) {
            scanf("%99s%1999s", a, hex);
            new_buffer(a, hex);
        } else if (strcmp(cmd, "APPEND") == 0) {
            scanf("%99s%1999s", a, hex);
            append_buffer(a, hex);
        } else if (strcmp(cmd, "SLICE") == 0) {
            scanf("%99s%d%d", a, &off, &len);
            slice_buffer(a, off, len);
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%99s", a);
            delete_buffer(a);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    return 0;
}