// F037.c
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

Buffer *head = NULL;

Buffer* find_buffer(const char *name) {
    Buffer *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
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

int parse_hex(const char *hex, unsigned char **out, int *out_len) {
    int len = strlen(hex);
    if (len % 2 != 0) return 0;
    int num_bytes = len / 2;
    unsigned char *data = malloc(num_bytes);
    for (int i = 0; i < num_bytes; i++) {
        int byte = hex_to_byte(hex + i * 2);
        if (byte < 0) { free(data); return 0; }
        data[i] = (unsigned char)byte;
    }
    *out = data;
    *out_len = num_bytes;
    return 1;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    unsigned char *data;
    int size;
    if (!parse_hex(hex, &data, &size)) return;
    Buffer *b = malloc(sizeof(Buffer));
    b->name = strdup(name);
    b->data = data;
    b->size = size;
    b->next = NULL;
    if (!head) head = b;
    else {
        Buffer *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = b;
    }
}

void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    unsigned char *new_data;
    int new_size;
    if (!parse_hex(hex, &new_data, &new_size)) return;
    unsigned char *combined = realloc(b->data, b->size + new_size);
    if (!combined) { free(new_data); return; }
    memcpy(combined + b->size, new_data, new_size);
    b->data = combined;
    b->size += new_size;
    free(new_data);
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
    Buffer *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->name);
            free(cur->data);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void list_buffers() {
    Buffer *cur = head;
    while (cur) {
        printf("%s %d\n", cur->name, cur->size);
        cur = cur->next;
    }
}

void free_all() {
    Buffer *cur = head;
    while (cur) {
        Buffer *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp->data);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "NEW") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *hex = strtok(NULL, " ");
            if (!hex) hex = "";
            new_buffer(name, hex);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *hex = strtok(NULL, " ");
            if (!hex) continue;
            append_buffer(name, hex);
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *off_str = strtok(NULL, " ");
            if (!off_str) continue;
            char *len_str = strtok(NULL, " ");
            if (!len_str) continue;
            int offset = atoi(off_str);
            int length = atoi(len_str);
            slice_buffer(name, offset, length);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_buffer(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    free_all();
    return 0;
}