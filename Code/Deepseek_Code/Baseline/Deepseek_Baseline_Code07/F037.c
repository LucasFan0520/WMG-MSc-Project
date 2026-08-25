// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Buffer {
    char *name;
    unsigned char *bytes;
    int size;
    struct Buffer *next;
} Buffer;

Buffer *head = NULL;
Buffer *tail = NULL;

Buffer *find_buffer(const char *name) {
    Buffer *b = head;
    while (b) {
        if (strcmp(b->name, name) == 0) return b;
        b = b->next;
    }
    return NULL;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    Buffer *b = malloc(sizeof(Buffer));
    b->name = malloc(strlen(name)+1);
    strcpy(b->name, name);
    int len = strlen(hex);
    if (len % 2 != 0) {
        free(b->name);
        free(b);
        return;
    }
    b->size = len / 2;
    b->bytes = malloc(b->size);
    for (int i = 0; i < b->size; i++) {
        unsigned int byte;
        sscanf(hex + 2*i, "%2x", &byte);
        b->bytes[i] = (unsigned char)byte;
    }
    b->next = NULL;
    if (tail) tail->next = b;
    else head = b;
    tail = b;
}

void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    int add = len / 2;
    b->bytes = realloc(b->bytes, b->size + add);
    for (int i = 0; i < add; i++) {
        unsigned int byte;
        sscanf(hex + 2*i, "%2x", &byte);
        b->bytes[b->size + i] = (unsigned char)byte;
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
    Buffer *prev = NULL;
    Buffer *b = head;
    while (b) {
        if (strcmp(b->name, name) == 0) {
            if (prev) prev->next = b->next;
            else head = b->next;
            if (b == tail) tail = prev;
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
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "NEW") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *hex = strtok(NULL, "");
            if (!hex) hex = "";
            new_buffer(name, hex);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *hex = strtok(NULL, "");
            if (hex) append_buffer(name, hex);
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *name = strtok(rest, " ");
            if (!name) continue;
            char *off = strtok(NULL, " ");
            char *len = strtok(NULL, " ");
            if (off && len) slice_buffer(name, atoi(off), atoi(len));
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(rest, " ");
            if (name) delete_buffer(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    free_all();
    return 0;
}