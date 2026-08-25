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

Buffer *find_buffer(char *name) {
    for (Buffer *b = head; b; b = b->next)
        if (strcmp(b->name, name) == 0) return b;
    return NULL;
}

int hex_to_byte(char *hex, unsigned char *out) {
    int len = strlen(hex);
    if (len % 2 != 0) return 0;
    for (int i = 0; i < len; i += 2) {
        unsigned int byte;
        if (sscanf(hex + i, "%2x", &byte) != 1) return 0;
        out[i/2] = (unsigned char)byte;
    }
    return len / 2;
}

void new_buffer(char *name, char *hex) {
    if (find_buffer(name)) return;
    unsigned char *bytes = malloc(strlen(hex)/2);
    int size = hex_to_byte(hex, bytes);
    if (size == 0) {
        free(bytes);
        return;
    }
    Buffer *b = malloc(sizeof(Buffer));
    b->name = strdup(name);
    b->bytes = bytes;
    b->size = size;
    b->next = head;
    head = b;
}

void append_buffer(char *name, char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    unsigned char *new_bytes = malloc(b->size + strlen(hex)/2);
    int added = hex_to_byte(hex, new_bytes + b->size);
    if (added == 0) {
        free(new_bytes);
        return;
    }
    memcpy(new_bytes, b->bytes, b->size);
    free(b->bytes);
    b->bytes = new_bytes;
    b->size += added;
}

void slice_buffer(char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= b->size || length <= 0 || offset + length > b->size) return;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->bytes[offset + i]);
    }
    putchar('\n');
}

void delete_buffer(char *name) {
    Buffer *prev = NULL;
    for (Buffer *b = head; b; b = b->next) {
        if (strcmp(b->name, name) == 0) {
            if (prev) prev->next = b->next;
            else head = b->next;
            free(b->name);
            free(b->bytes);
            free(b);
            return;
        }
        prev = b;
    }
}

void list_buffers(void) {
    for (Buffer *b = head; b; b = b->next)
        printf("%s %d\n", b->name, b->size);
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "NEW") == 0 && n == 3) {
            new_buffer(arg1, arg2);
        } else if (strcmp(cmd, "APPEND") == 0 && n == 3) {
            append_buffer(arg1, arg2);
        } else if (strcmp(cmd, "SLICE") == 0 && n == 4) {
            int offset, length;
            sscanf(arg2, "%d", &offset);
            sscanf(line + strlen(cmd) + strlen(arg1) + 2, "%d", &length);
            slice_buffer(arg1, offset, length);
        } else if (strcmp(cmd, "DELETE") == 0 && n == 2) {
            delete_buffer(arg1);
        } else if (strcmp(cmd, "LIST") == 0 && n == 1) {
            list_buffers();
        }
    }
    free(line);
    free_all();
    return 0;
}