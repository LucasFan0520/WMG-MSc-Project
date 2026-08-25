/* F037.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Buffer {
    char *name;
    unsigned char *bytes;
    int length;
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

int hex_to_byte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

int parse_hex_bytes(const char *hex, unsigned char **out, int *len) {
    int l = strlen(hex);
    if (l % 2 != 0) return 0;
    unsigned char *data = malloc(l / 2);
    for (int i = 0; i < l; i += 2) {
        int hi = hex_to_byte(hex[i]);
        int lo = hex_to_byte(hex[i+1]);
        if (hi < 0 || lo < 0) { free(data); return 0; }
        data[i/2] = (hi << 4) | lo;
    }
    *out = data;
    *len = l / 2;
    return 1;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    unsigned char *data;
    int len;
    if (!parse_hex_bytes(hex, &data, &len)) return;
    Buffer *b = malloc(sizeof(Buffer));
    b->name = strdup(name);
    b->bytes = data;
    b->length = len;
    b->next = buffers;
    buffers = b;
}

void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    unsigned char *data;
    int len;
    if (!parse_hex_bytes(hex, &data, &len)) return;
    b->bytes = realloc(b->bytes, b->length + len);
    memcpy(b->bytes + b->length, data, len);
    b->length += len;
    free(data);
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || length < 0 || offset + length > b->length) return;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->bytes[offset + i]);
    }
    printf("\n");
}

void delete_buffer(const char *name) {
    Buffer *prev = NULL, *cur = buffers;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else buffers = cur->next;
            free(cur->name);
            free(cur->bytes);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void list_buffers(void) {
    Buffer *b = buffers;
    while (b) {
        printf("%s %d\n", b->name, b->length);
        b = b->next;
    }
}

void free_buffers(void) {
    while (buffers) {
        Buffer *tmp = buffers;
        buffers = buffers->next;
        free(tmp->name);
        free(tmp->bytes);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256], hex[2048];
        int offset, length;
        int n = sscanf(line, "%s %s %s %d %d", cmd, name, hex, &offset, &length);
        if (strcmp(cmd, "NEW") == 0) {
            if (n < 3) continue;
            new_buffer(name, hex);
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (n < 3) continue;
            append_buffer(name, hex);
        } else if (strcmp(cmd, "SLICE") == 0) {
            if (n < 4) continue;
            slice_buffer(name, offset, length);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (n < 2) continue;
            delete_buffer(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    free_buffers();
    return 0;
}