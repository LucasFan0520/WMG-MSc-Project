/* F037.c */
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
    Buffer *b = buffers;
    while (b) {
        if (strcmp(b->name, name) == 0) return b;
        b = b->next;
    }
    return NULL;
}

void free_buffer(Buffer *b) {
    free(b->name);
    free(b->data);
    free(b);
}

int hex_to_byte(const char *s) {
    unsigned int val;
    if (sscanf(s, "%2x", &val) != 1) return -1;
    return (int)val;
}

void buffer_new(const char *name, const char *hexstr) {
    if (find_buffer(name)) return;
    size_t len = strlen(hexstr);
    if (len % 2 != 0) return;
    size_t bytes = len / 2;
    unsigned char *data = malloc(bytes);
    int ok = 1;
    for (size_t i = 0; i < bytes; i++) {
        char pair[3] = {hexstr[2*i], hexstr[2*i+1], '\0'};
        int val = hex_to_byte(pair);
        if (val < 0) { ok = 0; break; }
        data[i] = (unsigned char)val;
    }
    if (!ok) { free(data); return; }
    Buffer *b = malloc(sizeof(Buffer));
    b->name = strdup(name);
    b->data = data;
    b->size = bytes;
    b->next = buffers;
    buffers = b;
}

void buffer_append(const char *name, const char *hexstr) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    size_t len = strlen(hexstr);
    if (len % 2 != 0) return;
    size_t bytes = len / 2;
    unsigned char *newdata = realloc(b->data, b->size + bytes);
    if (!newdata) return;
    b->data = newdata;
    int ok = 1;
    for (size_t i = 0; i < bytes; i++) {
        char pair[3] = {hexstr[2*i], hexstr[2*i+1], '\0'};
        int val = hex_to_byte(pair);
        if (val < 0) { ok = 0; break; }
        b->data[b->size + i] = (unsigned char)val;
    }
    if (!ok) {
        b->data = realloc(b->data, b->size);
        return;
    }
    b->size += bytes;
}

void buffer_slice(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= (int)b->size || length < 0) return;
    if (offset + length > (int)b->size) length = b->size - offset;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->data[offset + i]);
    }
    putchar('\n');
}

void buffer_delete(const char *name) {
    Buffer *prev = NULL, *curr = buffers;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else buffers = curr->next;
            free_buffer(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void buffer_list(void) {
    Buffer *b = buffers;
    while (b) {
        printf("%s %zu\n", b->name, b->size);
        b = b->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char name[256];
    int offset, length;

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "NEW") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) != 1) continue;
            p += strlen(name);
            while (*p == ' ') p++;
            buffer_new(name, p);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) != 1) continue;
            p += strlen(name);
            while (*p == ' ') p++;
            buffer_append(name, p);
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %d %d", name, &offset, &length) == 3) buffer_slice(name, offset, length);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) buffer_delete(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            buffer_list();
        }
    }

    while (buffers) {
        Buffer *tmp = buffers;
        buffers = buffers->next;
        free_buffer(tmp);
    }
    return 0;
}