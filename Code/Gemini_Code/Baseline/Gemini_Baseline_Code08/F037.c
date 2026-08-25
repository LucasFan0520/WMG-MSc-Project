// F037.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    unsigned char *bytes;
    int size;
    int capacity;
} ByteBuffer;

ByteBuffer *buffers = NULL;
int count = 0;
int capacity = 0;

int find_buffer(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(buffers[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void create_buffer(const char *name, const char *hex) {
    if (find_buffer(name) != -1) return;
    if (count >= capacity) {
        capacity = capacity == 0 ? 4 : capacity * 2;
        buffers = realloc(buffers, capacity * sizeof(ByteBuffer));
    }
    buffers[count].name = strdup(name);
    size_t hex_len = strlen(hex);
    int b_count = hex_len / 2;
    buffers[count].bytes = malloc(b_count == 0 ? 1 : b_count);
    buffers[count].size = b_count;
    buffers[count].capacity = b_count;
    for (int i = 0; i < b_count; i++) {
        unsigned int val;
        sscanf(hex + 2 * i, "%2x", &val);
        buffers[count].bytes[i] = (unsigned char)val;
    }
    count++;
}

void append_buffer(const char *name, const char *hex) {
    int idx = find_buffer(name);
    if (idx == -1) return;
    ByteBuffer *b = &buffers[idx];
    size_t hex_len = strlen(hex);
    int b_count = hex_len / 2;
    if (b->size + b_count > b->capacity) {
        b->capacity = (b->size + b_count) * 2;
        b->bytes = realloc(b->bytes, b->capacity);
    }
    for (int i = 0; i < b_count; i++) {
        unsigned int val;
        sscanf(hex + 2 * i, "%2x", &val);
        b->bytes[b->size + i] = (unsigned char)val;
    }
    b->size += b_count;
}

void slice_buffer(const char *name, int offset, int length) {
    int idx = find_buffer(name);
    if (idx == -1) return;
    ByteBuffer *b = &buffers[idx];
    if (offset < 0 || offset >= b->size) return;
    int limit = offset + length;
    if (limit > b->size) limit = b->size;
    for (int i = offset; i < limit; i++) {
        printf("%02x", b->bytes[i]);
    }
    printf("\n");
}

void delete_buffer(const char *name) {
    int idx = find_buffer(name);
    if (idx != -1) {
        free(buffers[idx].name);
        free(buffers[idx].bytes);
        for (int i = idx; i < count - 1; i++) {
            buffers[i] = buffers[i + 1];
        }
        count--;
    }
}

void list_buffers(void) {
    for (int i = 0; i < count; i++) {
        printf("%s %d\n", buffers[i].name, buffers[i].size);
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "NEW") == 0) {
            char *name = strtok(NULL, " ");
            char *hex = strtok(NULL, "");
            if (name && hex) create_buffer(name, hex);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *name = strtok(NULL, " ");
            char *hex = strtok(NULL, "");
            if (name && hex) append_buffer(name, hex);
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *name = strtok(NULL, " ");
            char *off_s = strtok(NULL, " ");
            char *len_s = strtok(NULL, "");
            if (name && off_s && len_s) {
                slice_buffer(name, atoi(off_s), atoi(len_s));
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, "");
            if (name) delete_buffer(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    return 0;
}
