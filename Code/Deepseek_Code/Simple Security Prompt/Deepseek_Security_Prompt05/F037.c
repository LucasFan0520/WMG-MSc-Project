/* F037.c */
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
    char hex[3] = {s[0], s[1], '\0'};
    return (int)strtol(hex, NULL, 16);
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    Buffer *b = malloc(sizeof(Buffer));
    if (!b) return;
    b->name = strdup(name);
    b->size = len / 2;
    b->data = malloc(b->size);
    if (!b->data) { free(b->name); free(b); return; }
    for (int i = 0; i < b->size; i++) {
        int byte = hex_to_byte(hex + 2*i);
        if (byte < 0) { free(b->data); free(b->name); free(b); return; }
        b->data[i] = (unsigned char)byte;
    }
    b->next = buffers;
    buffers = b;
}

void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    int add = len / 2;
    unsigned char *newdata = realloc(b->data, b->size + add);
    if (!newdata) return;
    b->data = newdata;
    for (int i = 0; i < add; i++) {
        int byte = hex_to_byte(hex + 2*i);
        if (byte < 0) { b->size = b->size; return; }
        b->data[b->size + i] = (unsigned char)byte;
    }
    b->size += add;
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || length < 0 || offset + length > b->size) return;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->data[offset + i]);
    }
    printf("\n");
}

void delete_buffer(const char *name) {
    Buffer *b = buffers, *prev = NULL;
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
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[2048];
        int n = sscanf(line, "%15s %255s %2047s", cmd, arg1, arg2);
        if (strcmp(cmd, "NEW") == 0 && n >= 3) {
            new_buffer(arg1, arg2);
        } else if (strcmp(cmd, "APPEND") == 0 && n >= 3) {
            append_buffer(arg1, arg2);
        } else if (strcmp(cmd, "SLICE") == 0 && n >= 4) {
            char arg3[256];
            sscanf(line, "%*s %*s %s %s", arg1, arg2);
            int off = atoi(arg1), len = atoi(arg2);
            slice_buffer(arg1, off, len);
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_buffer(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    while (buffers) {
        Buffer *tmp = buffers;
        buffers = buffers->next;
        free(tmp->name);
        free(tmp->data);
        free(tmp);
    }
    return 0;
}