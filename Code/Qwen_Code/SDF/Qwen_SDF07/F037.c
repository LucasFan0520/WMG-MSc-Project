// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *name; uint8_t *data; size_t size; size_t cap; } Buf;
Buf *bufs = NULL;
size_t bc = 0, bcap = 0;

Buf* find_buf(const char *name) {
    for (size_t i = 0; i < bc; i++) if (strcmp(bufs[i].name, name) == 0) return &bufs[i];
    return NULL;
}

size_t parse_hex(const char *hex, uint8_t **out) {
    size_t len = strlen(hex);
    if (len % 2 != 0) return 0;
    size_t bytes = len / 2;
    uint8_t *data = malloc(bytes);
    if (!data) return 0;
    for (size_t i = 0; i < bytes; i++) {
        unsigned int val;
        if (sscanf(hex + i * 2, "%2x", &val) != 1) { free(data); return 0; }
        data[i] = (uint8_t)val;
    }
    *out = data;
    return bytes;
}

void new_buf(const char *name, const char *hex) {
    if (find_buf(name)) return;
    uint8_t *data;
    size_t bytes = parse_hex(hex, &data);
    if (!data && bytes == 0 && strlen(hex) > 0) return;
    
    if (bc == bcap) {
        size_t nc = bcap ? bcap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Buf)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Buf *tmp = realloc(bufs, nc * sizeof(Buf));
        if (!tmp) { perror("realloc"); exit(1); }
        bufs = tmp;
        bcap = nc;
    }
    bufs[bc].name = strdup(name);
    bufs[bc].data = data;
    bufs[bc].size = bytes;
    bufs[bc].cap = bytes;
    if (!bufs[bc].name) { perror("strdup"); exit(1); }
    bc++;
}

void app_buf(Buf *b, const char *hex) {
    uint8_t *data;
    size_t bytes = parse_hex(hex, &data);
    if (!data && bytes == 0 && strlen(hex) > 0) return;
    size_t nsize = b->size + bytes;
    if (nsize > b->cap) {
        size_t ncap = b->cap ? b->cap * 2 : 4;
        while (ncap < nsize) {
            if (ncap > SIZE_MAX / 2) { fprintf(stderr, "Overflow\n"); exit(1); }
            ncap *= 2;
        }
        uint8_t *tmp = realloc(b->data, ncap);
        if (!tmp) { perror("realloc"); exit(1); }
        b->data = tmp;
        b->cap = ncap;
    }
    memcpy(b->data + b->size, data, bytes);
    b->size = nsize;
    free(data);
}

void del_buf(const char *name) {
    for (size_t i = 0; i < bc; i++) {
        if (strcmp(bufs[i].name, name) == 0) {
            free(bufs[i].name); free(bufs[i].data);
            bufs[i] = bufs[--bc];
            return;
        }
    }
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "NEW") == 0) {
            char name[256], hex[1024];
            if (sscanf(line, "%*s %255s %1023s", name, hex) == 2) new_buf(name, hex);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char name[256], hex[1024];
            if (sscanf(line, "%*s %255s %1023s", name, hex) == 2) {
                Buf *b = find_buf(name);
                if (b) app_buf(b, hex);
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char name[256]; size_t off, len;
            if (sscanf(line, "%*s %255s %zu %zu", name, &off, &len) == 3) {
                Buf *b = find_buf(name);
                if (b && off + len <= b->size) {
                    for (size_t i = 0; i < len; i++) printf("%02x", b->data[off + i]);
                    printf("\n");
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) del_buf(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < bc; i++) printf("%s %zu\n", bufs[i].name, bufs[i].size);
        }
        free(line);
    }
    for (size_t i = 0; i < bc; i++) { free(bufs[i].name); free(bufs[i].data); }
    free(bufs);
    return 0;
}