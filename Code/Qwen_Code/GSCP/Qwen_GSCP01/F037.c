// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Buffer {
    char *name;
    unsigned char *data;
    size_t size;
    struct Buffer *next;
} Buffer;

Buffer *buffers = NULL;

Buffer *find_buffer(const char *name) {
    Buffer *curr = buffers;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

int parse_hex(const char *hex, unsigned char **out_data, size_t *out_size) {
    size_t len = strlen(hex);
    if (len % 2 != 0) return 0;
    size_t byte_len = len / 2;
    unsigned char *data = malloc(byte_len);
    if (!data) exit(EXIT_FAILURE);
    
    for (size_t i = 0; i < byte_len; i++) {
        char byte_str[3] = {hex[i*2], hex[i*2+1], '\0'};
        if (!isxdigit((unsigned char)byte_str[0]) || !isxdigit((unsigned char)byte_str[1])) {
            free(data);
            return 0;
        }
        data[i] = (unsigned char)strtol(byte_str, NULL, 16);
    }
    *out_data = data;
    *out_size = byte_len;
    return 1;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    unsigned char *data;
    size_t size;
    if (!parse_hex(hex, &data, &size)) return;
    
    Buffer *b = malloc(sizeof(Buffer));
    if (!b) exit(EXIT_FAILURE);
    b->name = strdup(name);
    b->data = data;
    b->size = size;
    b->next = buffers;
    buffers = b;
}

void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    unsigned char *data;
    size_t size;
    if (!parse_hex(hex, &data, &size)) return;
    
    unsigned char *new_data = malloc(b->size + size);
    if (!new_data) exit(EXIT_FAILURE);
    memcpy(new_data, b->data, b->size);
    memcpy(new_data + b->size, data, size);
    free(b->data);
    free(data);
    b->data = new_data;
    b->size += size;
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || length < 0 || offset + length > (int)b->size) return;
    
    for (int i = 0; i < length; i++) {
        printf("%02X", b->data[offset + i]);
    }
    printf("\n");
}

void delete_buffer(const char *name) {
    Buffer *curr = buffers;
    Buffer *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else buffers = curr->next;
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
    Buffer *curr = buffers;
    while (curr) {
        printf("%s %zu\n", curr->name, curr->size);
        curr = curr->next;
    }
}

void free_all() {
    Buffer *curr = buffers;
    while (curr) {
        Buffer *next = curr->next;
        free(curr->name);
        free(curr->data);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[7000] = {0};
        int offset = 0, length = 0;
        
        int matched = sscanf(line, "%31s %255s %6999s %d %d", cmd, arg1, arg2, &offset, &length);
        
        if (strcmp(cmd, "NEW") == 0 && matched >= 2) {
            new_buffer(arg1, arg2);
        } else if (strcmp(cmd, "APPEND") == 0 && matched >= 2) {
            append_buffer(arg1, arg2);
        } else if (strcmp(cmd, "SLICE") == 0 && matched >= 4) {
            slice_buffer(arg1, offset, length);
        } else if (strcmp(cmd, "DELETE") == 0) {
            delete_buffer(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    free_all();
    return 0;
}