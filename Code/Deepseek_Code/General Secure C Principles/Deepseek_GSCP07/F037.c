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

Buffer *head = NULL;

Buffer* find_buffer(const char *name) {
    Buffer *b = head;
    while (b) {
        if (strcmp(b->name, name) == 0) return b;
        b = b->next;
    }
    return NULL;
}

void free_buffer(Buffer *b) {
    free(b->name);
    free(b->bytes);
    free(b);
}

int hex_to_byte(const char *hex, unsigned char *byte) {
    if (strlen(hex) != 2) return 0;
    for (int i = 0; i < 2; i++) {
        if (!isxdigit(hex[i])) return 0;
    }
    *byte = (unsigned char) strtol(hex, NULL, 16);
    return 1;
}

int parse_hexbytes(const char *str, unsigned char **out, int *out_len) {
    int len = strlen(str);
    if (len % 2 != 0) return 0;
    int num_bytes = len / 2;
    unsigned char *bytes = malloc(num_bytes);
    for (int i = 0; i < num_bytes; i++) {
        char hex[3] = {str[2*i], str[2*i+1], '\0'};
        unsigned char byte;
        if (!hex_to_byte(hex, &byte)) {
            free(bytes);
            return 0;
        }
        bytes[i] = byte;
    }
    *out = bytes;
    *out_len = num_bytes;
    return 1;
}

void handle_new(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    if (find_buffer(name)) return;
    char *hex = strtok(NULL, " ");
    if (!hex) return;
    unsigned char *bytes;
    int size;
    if (!parse_hexbytes(hex, &bytes, &size)) return;
    Buffer *b = malloc(sizeof(Buffer));
    b->name = strdup(name);
    b->bytes = bytes;
    b->size = size;
    b->next = NULL;
    if (!head) head = b;
    else {
        Buffer *last = head;
        while (last->next) last = last->next;
        last->next = b;
    }
}

void handle_append(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Buffer *b = find_buffer(name);
    if (!b) return;
    char *hex = strtok(NULL, " ");
    if (!hex) return;
    unsigned char *bytes;
    int size;
    if (!parse_hexbytes(hex, &bytes, &size)) return;
    b->bytes = realloc(b->bytes, b->size + size);
    memcpy(b->bytes + b->size, bytes, size);
    b->size += size;
    free(bytes);
}

void handle_slice(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Buffer *b = find_buffer(name);
    if (!b) return;
    char *offstr = strtok(NULL, " ");
    char *lenstr = strtok(NULL, " ");
    if (!offstr || !lenstr) return;
    int offset = atoi(offstr);
    int length = atoi(lenstr);
    if (offset < 0 || length < 0 || offset + length > b->size) return;
    for (int i = 0; i < length; i++) {
        printf("%02X", b->bytes[offset + i]);
    }
    putchar('\n');
}

void handle_delete(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Buffer *b = head, *prev = NULL;
    while (b) {
        if (strcmp(b->name, name) == 0) {
            if (prev) prev->next = b->next;
            else head = b->next;
            free_buffer(b);
            return;
        }
        prev = b;
        b = b->next;
    }
}

void handle_list() {
    Buffer *b = head;
    while (b) {
        printf("%s %d\n", b->name, b->size);
        b = b->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "NEW") == 0) handle_new(cmd);
        else if (strcmp(cmd, "APPEND") == 0) handle_append(cmd);
        else if (strcmp(cmd, "SLICE") == 0) handle_slice(cmd);
        else if (strcmp(cmd, "DELETE") == 0) handle_delete(cmd);
        else if (strcmp(cmd, "LIST") == 0) handle_list();
    }
    while (head) {
        Buffer *tmp = head;
        head = head->next;
        free_buffer(tmp);
    }
    return 0;
}