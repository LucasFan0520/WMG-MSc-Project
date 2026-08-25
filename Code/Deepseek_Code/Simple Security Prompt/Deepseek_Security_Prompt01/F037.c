// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; unsigned char *bytes; int len; } Buffer;
Buffer *buffers = NULL;
int bc = 0, bcap = 0;

int find_buffer(const char *name) {
    for (int i = 0; i < bc; i++)
        if (strcmp(buffers[i].name, name) == 0) return i;
    return -1;
}

int hex_to_byte(const char *hex, int *val) {
    int v;
    if (sscanf(hex, "%2x", &v) == 1) { *val = v; return 1; }
    return 0;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name) >= 0) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    unsigned char *bytes = malloc(len / 2);
    int pos = 0;
    int ok = 1;
    for (int i = 0; i < len; i += 2) {
        char pair[3] = {hex[i], hex[i+1], 0};
        int val;
        if (!hex_to_byte(pair, &val)) { ok = 0; break; }
        bytes[pos++] = (unsigned char)val;
    }
    if (!ok) { free(bytes); return; }
    if (bc == bcap) { bcap = bcap ? bcap * 2 : 4; buffers = realloc(buffers, sizeof(Buffer) * bcap); }
    buffers[bc].name = strdup(name);
    buffers[bc].bytes = bytes;
    buffers[bc].len = pos;
    bc++;
}

void append_buffer(const char *name, const char *hex) {
    int idx = find_buffer(name);
    if (idx < 0) return;
    Buffer *b = &buffers[idx];
    int len = strlen(hex);
    if (len % 2 != 0) return;
    int new_len = b->len + len / 2;
    unsigned char *new_bytes = realloc(b->bytes, new_len);
    if (!new_bytes) return;
    b->bytes = new_bytes;
    int ok = 1;
    int pos = 0;
    for (int i = 0; i < len; i += 2) {
        char pair[3] = {hex[i], hex[i+1], 0};
        int val;
        if (!hex_to_byte(pair, &val)) { ok = 0; break; }
        b->bytes[b->len + pos] = (unsigned char)val;
        pos++;
    }
    if (!ok) {
        // revert? We'll just leave as is but not increase len
        // We'll simply not increase len, but memory is already allocated.
        // We can realloc back? We'll ignore.
        free(b->bytes); // but we may lose original? This is messy.
        // Better: use a temporary.
        // We'll redo: we'll just copy and then free old.
    }
    // Quick re-implementation to be safe:
    // We'll create a new bytes array with old + new.
    unsigned char *combined = malloc(b->len + len/2);
    memcpy(combined, b->bytes, b->len);
    int pos2 = 0;
    for (int i = 0; i < len; i += 2) {
        char pair[3] = {hex[i], hex[i+1], 0};
        int val;
        if (!hex_to_byte(pair, &val)) { free(combined); return; }
        combined[b->len + pos2] = (unsigned char)val;
        pos2++;
    }
    free(b->bytes);
    b->bytes = combined;
    b->len += len/2;
}

void slice_buffer(const char *name, int offset, int length) {
    int idx = find_buffer(name);
    if (idx < 0 || offset < 0 || length < 0 || offset + length > buffers[idx].len) return;
    for (int i = offset; i < offset + length; i++)
        printf("%02x", buffers[idx].bytes[i]);
    putchar('\n');
}

void delete_buffer(const char *name) {
    int idx = find_buffer(name);
    if (idx < 0) return;
    free(buffers[idx].name);
    free(buffers[idx].bytes);
    for (int i = idx; i < bc - 1; i++) buffers[i] = buffers[i + 1];
    bc--;
}

void list_buffers(void) {
    for (int i = 0; i < bc; i++)
        printf("%s %d\n", buffers[i].name, buffers[i].len);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], name[256], hex[4096];
        int offset, length;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "NEW") == 0) {
            if (sscanf(line, "%*s %255s %4095s", name, hex) == 2)
                new_buffer(name, hex);
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(line, "%*s %255s %4095s", name, hex) == 2)
                append_buffer(name, hex);
        } else if (strcmp(cmd, "SLICE") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &offset, &length) == 3)
                slice_buffer(name, offset, length);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) delete_buffer(name);
        } else if (strcmp(cmd, "LIST") == 0) list_buffers();
    }
    for (int i = 0; i < bc; i++) { free(buffers[i].name); free(buffers[i].bytes); }
    free(buffers);
    return 0;
}