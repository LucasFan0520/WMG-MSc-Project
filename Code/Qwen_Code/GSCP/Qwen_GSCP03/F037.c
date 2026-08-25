// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Buf {
    char *name;
    unsigned char *data;
    int size;
    struct Buf *next;
} Buf;

Buf *head = NULL;

void free_list() {
    while (head) {
        Buf *b = head; head = head->next;
        free(b->name); free(b->data); free(b);
    }
}

int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

int parse_hex(const char *hex, unsigned char **out) {
    int len = strlen(hex);
    if (len % 2 != 0) return -1;
    int bytes = len / 2;
    unsigned char *data = malloc(bytes);
    for (int i = 0; i < bytes; i++) {
        int h = hex_val(hex[2*i]);
        int l = hex_val(hex[2*i+1]);
        if (h < 0 || l < 0) { free(data); return -1; }
        data[i] = (h << 4) | l;
    }
    *out = data;
    return bytes;
}

Buf *find_buf(const char *name) {
    for (Buf *b = head; b; b = b->next)
        if (strcmp(b->name, name) == 0) return b;
    return NULL;
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], name[256], hex[4096] = "";
        int offset, length;
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "NEW") == 0) {
            if (sscanf(line, "%*s %255s %4095s", name, hex) != 2) continue;
            if (find_buf(name)) continue;
            unsigned char *data;
            int size = parse_hex(hex, &data);
            if (size < 0) continue;
            Buf *b = malloc(sizeof(Buf));
            b->name = strdup(name); b->data = data; b->size = size;
            b->next = head; head = b;
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(line, "%*s %255s %4095s", name, hex) != 2) continue;
            Buf *b = find_buf(name);
            if (b) {
                unsigned char *data;
                int size = parse_hex(hex, &data);
                if (size < 0) continue;
                b->data = realloc(b->data, b->size + size);
                memcpy(b->data + b->size, data, size);
                b->size += size;
                free(data);
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &offset, &length) != 3) continue;
            Buf *b = find_buf(name);
            if (b && offset >= 0 && length >= 0 && offset + length <= b->size) {
                for (int i = 0; i < length; i++) {
                    printf("%02x", b->data[offset + i]);
                }
                printf("\n");
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) != 1) continue;
            Buf *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    Buf *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->name); free(tmp->data); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Buf *b = head; b; b = b->next) {
                printf("%s %d\n", b->name, b->size);
            }
        }
    }
    free_list();
    return 0;
}