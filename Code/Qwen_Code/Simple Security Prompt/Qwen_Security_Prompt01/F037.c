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
    *out = malloc(bytes);
    for (int i = 0; i < bytes; i++) {
        int h = hex_val(hex[2*i]);
        int l = hex_val(hex[2*i+1]);
        if (h < 0 || l < 0) { free(*out); return -1; }
        (*out)[i] = (h << 4) | l;
    }
    return bytes;
}

void new_buf(const char *name, const char *hex) {
    unsigned char *data;
    int size = parse_hex(hex, &data);
    if (size < 0) return;
    Buf *b = malloc(sizeof(Buf));
    b->name = strdup(name);
    b->data = data;
    b->size = size;
    b->next = head;
    head = b;
}

void append_buf(const char *name, const char *hex) {
    for (Buf *b = head; b; b = b->next) {
        if (strcmp(b->name, name) == 0) {
            unsigned char *data;
            int size = parse_hex(hex, &data);
            if (size < 0) return;
            b->data = realloc(b->data, b->size + size);
            memcpy(b->data + b->size, data, size);
            b->size += size;
            free(data);
            return;
        }
    }
}

void slice_buf(const char *name, int offset, int length) {
    for (Buf *b = head; b; b = b->next) {
        if (strcmp(b->name, name) == 0) {
            if (offset < 0 || length < 0 || offset + length > b->size) return;
            for (int i = 0; i < length; i++) {
                printf("%02x", b->data[offset + i]);
            }
            printf("\n");
            return;
        }
    }
}

void delete_buf(const char *name) {
    Buf *p = head, *prev = NULL;
    while (p) {
        if (strcmp(p->name, name) == 0) {
            if (prev) prev->next = p->next;
            else head = p->next;
            free(p->name); free(p->data); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void list_buf() {
    for (Buf *b = head; b; b = b->next) {
        printf("%s %d\n", b->name, b->size);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[8000];
        int i, j;
        b[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "NEW") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            new_buf(a, b);
        } else if (strcmp(cmd, "APPEND") == 0) {
            sscanf(line, "%*s %s %s", a, b);
            append_buf(a, b);
        } else if (strcmp(cmd, "SLICE") == 0) {
            sscanf(line, "%*s %s %d %d", a, &i, &j);
            slice_buf(a, i, j);
        } else if (strcmp(cmd, "DELETE") == 0) {
            sscanf(line, "%*s %s", a);
            delete_buf(a);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buf();
        }
    }
    while (head) {
        Buf *n = head->next;
        free(head->name); free(head->data); free(head);
        head = n;
    }
    return 0;
}