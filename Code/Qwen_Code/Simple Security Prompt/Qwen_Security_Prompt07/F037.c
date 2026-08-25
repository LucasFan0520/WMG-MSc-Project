// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; unsigned char *bytes; int size, cap; } Buf;
Buf *bufs = 0; int n = 0, cap = 0;

int find(char *name) {
    for (int i = 0; i < n; i++) if (!strcmp(bufs[i].name, name)) return i;
    return -1;
}

int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

void add_hex(Buf *b, char *hex) {
    int len = strlen(hex);
    if (len % 2 != 0) return;
    for (int i = 0; i < len; i += 2) {
        int h = hex_val(hex[i]), l = hex_val(hex[i+1]);
        if (h < 0 || l < 0) return;
        if (b->size == b->cap) {
            b->cap = b->cap ? b->cap * 2 : 4;
            b->bytes = realloc(b->bytes, b->cap);
        }
        b->bytes[b->size++] = (h << 4) | l;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], b[2048]; int off, len;
        a[0] = b[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "NEW") || !strcmp(cmd, "APPEND")) sscanf(line, "%*s %s %s", a, b);
        else if (!strcmp(cmd, "SLICE")) sscanf(line, "%*s %s %d %d", a, &off, &len);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "NEW")) {
            int idx = find(a);
            if (idx >= 0) { free(bufs[idx].bytes); bufs[idx].bytes = 0; bufs[idx].size = 0; bufs[idx].cap = 0; }
            else {
                if (n == cap) { cap = cap ? cap * 2 : 4; bufs = realloc(bufs, cap * sizeof(Buf)); }
                idx = n++; bufs[idx].name = strdup(a); bufs[idx].bytes = 0; bufs[idx].size = 0; bufs[idx].cap = 0;
            }
            add_hex(&bufs[idx], b);
        } else if (!strcmp(cmd, "APPEND")) {
            int idx = find(a);
            if (idx >= 0) add_hex(&bufs[idx], b);
        } else if (!strcmp(cmd, "SLICE")) {
            int idx = find(a);
            if (idx >= 0 && off >= 0 && len >= 0 && off + len <= bufs[idx].size) {
                for (int i = 0; i < len; i++) printf("%02X", bufs[idx].bytes[off + i]);
                putchar('\n');
            }
        } else if (!strcmp(cmd, "DELETE")) {
            int idx = find(a);
            if (idx >= 0) {
                free(bufs[idx].name); free(bufs[idx].bytes);
                for (int i = idx; i < n - 1; i++) bufs[i] = bufs[i + 1]; n--;
            }
        } else if (!strcmp(cmd, "LIST")) {
            for (int i = 0; i < n; i++) printf("%s %d\n", bufs[i].name, bufs[i].size);
        }
    }
    return 0;
}