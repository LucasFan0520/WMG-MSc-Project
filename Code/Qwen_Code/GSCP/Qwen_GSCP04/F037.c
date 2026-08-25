// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Buf { char *name; unsigned char *data; int size; struct Buf *next; } Buf;
Buf *bufs = NULL;

int hex2bin(const char *hex, unsigned char **out) {
    int len = strlen(hex); if (len % 2 != 0) return -1;
    *out = malloc(len / 2);
    for (int i = 0; i < len / 2; i++) {
        if (!isxdigit(hex[i*2]) || !isxdigit(hex[i*2+1])) { free(*out); return -1; }
        unsigned int val; sscanf(hex + i*2, "%2x", &val); (*out)[i] = val;
    }
    return len / 2;
}

Buf *find(char *n) { for (Buf *b = bufs; b; b = b->next) if (!strcmp(b->name, n)) return b; return NULL; }
void cmd_new(char *n, char *hex) {
    if (find(n)) return;
    unsigned char *data; int len = hex2bin(hex, &data); if (len < 0) return;
    Buf *b = malloc(sizeof(Buf)); b->name = strdup(n); b->data = data; b->size = len; b->next = bufs; bufs = b;
}
void cmd_append(char *n, char *hex) {
    Buf *b = find(n); if (!b) return;
    unsigned char *data; int len = hex2bin(hex, &data); if (len < 0) return;
    b->data = realloc(b->data, b->size + len); memcpy(b->data + b->size, data, len); b->size += len; free(data);
}
void cmd_slice(char *n, int off, int len) {
    Buf *b = find(n); if (!b || off < 0 || len < 0 || off + len > b->size) return;
    for (int i = 0; i < len; i++) printf("%02x", b->data[off + i]);
    printf("\n");
}
void cmd_delete(char *n) {
    Buf *curr = bufs, *prev = NULL;
    while (curr) { if (!strcmp(curr->name, n)) { if (prev) prev->next = curr->next; else bufs = curr->next; free(curr->name); free(curr->data); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_list() { for (Buf *b = bufs; b; b = b->next) printf("%s %d\n", b->name, b->size); }
void free_all() { Buf *b = bufs; while (b) { Buf *nb = b->next; free(b->name); free(b->data); free(b); b = nb; } }
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[4096] = {0}; int off, len;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "NEW") && sscanf(line, "%*s %255s %4095s", a1, a2) == 2) cmd_new(a1, a2);
        else if (!strcmp(cmd, "APPEND") && sscanf(line, "%*s %255s %4095s", a1, a2) == 2) cmd_append(a1, a2);
        else if (!strcmp(cmd, "SLICE") && sscanf(line, "%*s %255s %d %d", a1, &off, &len) == 3) cmd_slice(a1, off, len);
        else if (!strcmp(cmd, "DELETE") && sscanf(line, "%*s %255s", a1) == 1) cmd_delete(a1);
        else if (!strcmp(cmd, "LIST")) cmd_list();
    }
    free_all(); return 0;
}