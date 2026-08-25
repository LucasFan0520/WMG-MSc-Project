// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

typedef struct Buf {
    char *name;
    unsigned char *bytes;
    int size;
    struct Buf *next;
} Buf;

Buf *bufs = NULL;

int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + c - 'a';
    if (c >= 'A' && c <= 'F') return 10 + c - 'A';
    return -1;
}

int parse_hex(const char *hex, unsigned char **out) {
    int len = strlen(hex);
    if (len % 2 != 0) return 0;
    int byte_len = len / 2;
    unsigned char *bytes = malloc(byte_len);
    for (int i = 0; i < byte_len; i++) {
        int h = hex_val(hex[i*2]);
        int l = hex_val(hex[i*2+1]);
        if (h == -1 || l == -1) { free(bytes); return 0; }
        bytes[i] = (h << 4) | l;
    }
    *out = bytes;
    return byte_len;
}

Buf *find_buf(const char *name) {
    for (Buf *b = bufs; b; b = b->next) {
        if (strcmp(b->name, name) == 0) return b;
    }
    return NULL;
}

void new_buf(const char *name, const char *hex) {
    if (find_buf(name)) return;
    unsigned char *bytes;
    int len = parse_hex(hex, &bytes);
    if (len == 0) return;
    
    Buf *b = malloc(sizeof(Buf));
    b->name = strdup(name);
    b->bytes = bytes;
    b->size = len;
    b->next = bufs;
    bufs = b;
}

void append_buf(const char *name, const char *hex) {
    Buf *b = find_buf(name);
    if (!b) return;
    unsigned char *bytes;
    int len = parse_hex(hex, &bytes);
    if (len == 0) return;
    
    b->bytes = realloc(b->bytes, b->size + len);
    memcpy(b->bytes + b->size, bytes, len);
    b->size += len;
    free(bytes);
}

void slice_buf(const char *name, int offset, int length) {
    Buf *b = find_buf(name);
    if (!b || offset < 0 || length < 0 || offset + length > b->size) return;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->bytes[offset + i]);
    }
    printf("\n");
}

void delete_buf(const char *name) {
    Buf *prev = NULL, *curr = bufs;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else bufs = curr->next;
            free(curr->name);
            free(curr->bytes);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void list_bufs() {
    for (Buf *b = bufs; b; b = b->next) {
        printf("%s %d\n", b->name, b->size);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[512] = {0};
        int off = 0, len = 0;
        sscanf(line, "%15s %255s %511s", cmd, arg1, arg2);
        
        if (strcmp(cmd, "NEW") == 0) new_buf(arg1, arg2);
        else if (strcmp(cmd, "APPEND") == 0) append_buf(arg1, arg2);
        else if (strcmp(cmd, "SLICE") == 0) {
            sscanf(line, "%*s %*s %d %d", &off, &len);
            slice_buf(arg1, off, len);
        }
        else if (strcmp(cmd, "DELETE") == 0) delete_buf(arg1);
        else if (strcmp(cmd, "LIST") == 0) list_bufs();
        
        free(line);
    }
    return 0;
}