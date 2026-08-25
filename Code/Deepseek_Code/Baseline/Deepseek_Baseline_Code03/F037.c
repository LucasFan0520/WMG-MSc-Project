// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct buffer {
    char *name;
    unsigned char *bytes;
    int size;
    struct buffer *next;
};

struct buffer *buffers = NULL;

struct buffer *find_buffer(char *name) {
    struct buffer *b = buffers;
    while (b) {
        if (strcmp(b->name, name) == 0) return b;
        b = b->next;
    }
    return NULL;
}

int hex_to_byte(char *s) {
    int val;
    if (sscanf(s, "%2x", &val) != 1) return -1;
    return val;
}

void new_buffer(char *name, char *hex) {
    if (find_buffer(name)) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    struct buffer *b = malloc(sizeof(struct buffer));
    b->name = malloc(strlen(name) + 1);
    strcpy(b->name, name);
    b->size = len / 2;
    b->bytes = malloc(b->size);
    for (int i = 0; i < b->size; i++) {
        char pair[3];
        pair[0] = hex[2*i];
        pair[1] = hex[2*i+1];
        pair[2] = 0;
        int byte = hex_to_byte(pair);
        if (byte < 0) {
            free(b->name);
            free(b->bytes);
            free(b);
            return;
        }
        b->bytes[i] = (unsigned char)byte;
    }
    b->next = buffers;
    buffers = b;
}

void append_buffer(char *name, char *hex) {
    struct buffer *b = find_buffer(name);
    if (!b) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    int new_size = len / 2;
    unsigned char *new_bytes = malloc(b->size + new_size);
    memcpy(new_bytes, b->bytes, b->size);
    for (int i = 0; i < new_size; i++) {
        char pair[3];
        pair[0] = hex[2*i];
        pair[1] = hex[2*i+1];
        pair[2] = 0;
        int byte = hex_to_byte(pair);
        if (byte < 0) {
            free(new_bytes);
            return;
        }
        new_bytes[b->size + i] = (unsigned char)byte;
    }
    free(b->bytes);
    b->bytes = new_bytes;
    b->size += new_size;
}

void slice_buffer(char *name, int offset, int length) {
    struct buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= b->size || length < 0 || offset + length > b->size) return;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->bytes[offset + i]);
    }
    printf("\n");
}

void delete_buffer(char *name) {
    struct buffer *b = buffers, *prev = NULL;
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

void list_buffers() {
    struct buffer *b = buffers;
    while (b) {
        printf("%s %d\n", b->name, b->size);
        b = b->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], hex[4096];
        int offset, length;
        int n = sscanf(line, "%31s %255s %s %d %d", cmd, name, hex, &offset, &length);
        if (strcmp(cmd, "NEW") == 0 && n >= 3) {
            new_buffer(name, hex);
        } else if (strcmp(cmd, "APPEND") == 0 && n >= 3) {
            append_buffer(name, hex);
        } else if (strcmp(cmd, "SLICE") == 0 && n >= 4) {
            slice_buffer(name, offset, length);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_buffer(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    return 0;
}