/* F037.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Buffer {
    char *name;
    unsigned char *bytes;
    size_t size;
    size_t cap;
} Buffer;

static Buffer *buffers = NULL;
static size_t bcount = 0, bcap = 0;

static Buffer *find_buffer(const char *name) {
    for (size_t i = 0; i < bcount; i++) {
        if (strcmp(buffers[i].name, name) == 0)
            return &buffers[i];
    }
    return NULL;
}

static int hex_to_byte(const char *s) {
    int val = 0;
    for (int i = 0; i < 2; i++) {
        char c = s[i];
        if (c >= '0' && c <= '9') val = val * 16 + (c - '0');
        else if (c >= 'a' && c <= 'f') val = val * 16 + (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') val = val * 16 + (c - 'A' + 10);
        else return -1;
    }
    return val;
}

static void append_bytes(Buffer *b, const char *hex) {
    size_t len = strlen(hex);
    if (len % 2 != 0) return;
    for (size_t i = 0; i < len; i += 2) {
        int byte = hex_to_byte(hex + i);
        if (byte < 0) return;
        if (b->size == b->cap) {
            b->cap = b->cap ? b->cap * 2 : 16;
            b->bytes = realloc(b->bytes, b->cap);
            if (!b->bytes) { perror("realloc"); exit(1); }
        }
        b->bytes[b->size++] = (unsigned char)byte;
    }
}

static void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    if (bcount == bcap) {
        bcap = bcap ? bcap * 2 : 4;
        buffers = realloc(buffers, bcap * sizeof(Buffer));
        if (!buffers) { perror("realloc"); exit(1); }
    }
    buffers[bcount].name = strdup(name);
    buffers[bcount].bytes = NULL;
    buffers[bcount].size = buffers[bcount].cap = 0;
    append_bytes(&buffers[bcount], hex);
    bcount++;
}

static void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (b) append_bytes(b, hex);
}

static void slice_buffer(const char *name, size_t offset, size_t length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset >= b->size) return;
    if (offset + length > b->size) length = b->size - offset;
    for (size_t i = 0; i < length; i++) {
        printf("%02x", b->bytes[offset + i]);
    }
    printf("\n");
}

static void delete_buffer(const char *name) {
    for (size_t i = 0; i < bcount; i++) {
        if (strcmp(buffers[i].name, name) == 0) {
            free(buffers[i].name);
            free(buffers[i].bytes);
            for (size_t j = i; j < bcount - 1; j++)
                buffers[j] = buffers[j + 1];
            bcount--;
            return;
        }
    }
}

static void list_buffers(void) {
    for (size_t i = 0; i < bcount; i++) {
        printf("%s %zu\n", buffers[i].name, buffers[i].size);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
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
            if (!hex) hex = "";
            append_buffer(name, hex);
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *off = strtok(NULL, " ");
            char *len = strtok(NULL, " ");
            if (!off || !len) continue;
            int o = atoi(off), l = atoi(len);
            if (o < 0 || l < 0) continue;
            slice_buffer(name, (size_t)o, (size_t)l);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) delete_buffer(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    for (size_t i = 0; i < bcount; i++) {
        free(buffers[i].name);
        free(buffers[i].bytes);
    }
    free(buffers);
    return 0;
}