// filename: F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Buffer {
    char *name;
    unsigned char *bytes;
    size_t size;
    struct Buffer *next;
} Buffer;

static Buffer *buffers = NULL;

static void cleanup(void) {
    while (buffers) {
        Buffer *tmp = buffers;
        buffers = buffers->next;
        free(tmp->name);
        free(tmp->bytes);
        free(tmp);
    }
}

static Buffer *find_buffer(const char *name) {
    for (Buffer *b = buffers; b; b = b->next)
        if (strcmp(b->name, name) == 0)
            return b;
    return NULL;
}

static int hex_to_byte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

static unsigned char *parse_hex(const char *hex, size_t *out_len) {
    size_t len = strlen(hex);
    if (len % 2 != 0) return NULL;
    unsigned char *data = malloc(len / 2);
    if (!data) return NULL;
    size_t j = 0;
    for (size_t i = 0; i < len; i += 2) {
        int hi = hex_to_byte(hex[i]);
        int lo = hex_to_byte(hex[i+1]);
        if (hi < 0 || lo < 0) {
            free(data);
            return NULL;
        }
        data[j++] = (hi << 4) | lo;
    }
    *out_len = j;
    return data;
}

static void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    size_t sz;
    unsigned char *data = parse_hex(hex, &sz);
    if (!data) return;
    Buffer *b = malloc(sizeof(Buffer));
    if (!b) { free(data); return; }
    b->name = strdup(name);
    if (!b->name) { free(data); free(b); return; }
    b->bytes = data;
    b->size = sz;
    b->next = buffers;
    buffers = b;
}

static void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    size_t add_sz;
    unsigned char *add = parse_hex(hex, &add_sz);
    if (!add) return;
    unsigned char *new_bytes = realloc(b->bytes, b->size + add_sz);
    if (!new_bytes) { free(add); return; }
    memcpy(new_bytes + b->size, add, add_sz);
    b->bytes = new_bytes;
    b->size += add_sz;
    free(add);
}

static void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= (int)b->size || length <= 0) return;
    if (offset + length > (int)b->size) length = b->size - offset;
    for (int i = 0; i < length; ++i) {
        printf("%02X", b->bytes[offset + i]);
    }
    putchar('\n');
}

static void delete_buffer(const char *name) {
    Buffer *prev = NULL;
    for (Buffer *b = buffers; b; b = b->next) {
        if (strcmp(b->name, name) == 0) {
            if (prev) prev->next = b->next;
            else buffers = b->next;
            free(b->name);
            free(b->bytes);
            free(b);
            return;
        }
        prev = b;
    }
}

static void list_buffers(void) {
    for (Buffer *b = buffers; b; b = b->next)
        printf("%s %zu\n", b->name, b->size);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        if (strcmp(cmd, "NEW") == 0) {
            if (*arg1 && *arg2) new_buffer(arg1, arg2);
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (*arg1 && *arg2) append_buffer(arg1, arg2);
        } else if (strcmp(cmd, "SLICE") == 0) {
            if (*arg1 && *arg2) {
                char *arg3 = arg2;
                while (*arg3 && *arg3 != ' ') ++arg3;
                if (*arg3) *arg3++ = '\0';
                int offset = atoi(arg2);
                int length = atoi(arg3);
                slice_buffer(arg1, offset, length);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (*arg1) delete_buffer(arg1);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    free(line);
    return 0;
}