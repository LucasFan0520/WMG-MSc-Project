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

Buffer *head = NULL;

int parse_hex(const char *hex, unsigned char **out, size_t *out_size) {
    size_t len = strlen(hex);
    if (len % 2 != 0) return 0;
    size_t byte_len = len / 2;
    unsigned char *data = malloc(byte_len);
    if (!data) return 0;
    
    for (size_t i = 0; i < byte_len; i++) {
        char byte_str[3] = {hex[i*2], hex[i*2+1], '\0'};
        if (!isxdigit(byte_str[0]) || !isxdigit(byte_str[1])) {
            free(data);
            return 0;
        }
        data[i] = (unsigned char)strtol(byte_str, NULL, 16);
    }
    *out = data;
    *out_size = byte_len;
    return 1;
}

Buffer *find_buffer(const char *name) {
    for (Buffer *b = head; b; b = b->next) {
        if (strcmp(b->name, name) == 0) return b;
    }
    return NULL;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    unsigned char *data;
    size_t size;
    if (!parse_hex(hex, &data, &size)) return;
    
    Buffer *b = malloc(sizeof(Buffer));
    if (!b) { free(data); return; }
    b->name = strdup(name);
    b->data = data;
    b->size = size;
    if (!b->name) { free(b->data); free(b); return; }
    b->next = head;
    head = b;
}

void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    unsigned char *data;
    size_t size;
    if (!parse_hex(hex, &data, &size)) return;
    
    unsigned char *new_data = realloc(b->data, b->size + size);
    if (!new_data) { free(data); return; }
    memcpy(new_data + b->size, data, size);
    b->data = new_data;
    b->size += size;
    free(data);
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || length < 0 || offset + length > (int)b->size) return;
    
    for (int i = 0; i < length; i++) {
        printf("%02x", b->data[offset + i]);
    }
    printf("\n");
}

void delete_buffer(const char *name) {
    Buffer *curr = head;
    Buffer *prev = NULL;
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
        printf("%s %zu\n", b->name, b->size);
    }
}

void free_all() {
    Buffer *b = head;
    while (b) {
        Buffer *next = b->next;
        free(b->name);
        free(b->data);
        free(b);
        b = next;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        
        char cmd[64] = {0};
        char arg1[1024] = {0};
        char arg2[4096] = {0};
        int off = 0, len = 0;
        
        int n = sscanf(line, "%63s %1023s %4095[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        
        if (strcmp(cmd, "NEW") == 0 && n >= 3) {
            new_buffer(arg1, arg2);
        } else if (strcmp(cmd, "APPEND") == 0 && n >= 3) {
            append_buffer(arg1, arg2);
        } else if (strcmp(cmd, "SLICE") == 0) {
            if (sscanf(line, "%*s %1023s %d %d", arg1, &off, &len) == 3) {
                slice_buffer(arg1, off, len);
            }
        } else if (strcmp(cmd, "DELETE") == 0 && n >= 2) {
            delete_buffer(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    free_all();
    return 0;
}