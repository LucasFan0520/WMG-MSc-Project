// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Buf {
    char *name;
    unsigned char *data;
    int size;
    struct Buf *next;
} Buf;

Buf *head = NULL;

int hex_to_byte(char c1, char c2) {
    if (!isxdigit(c1) || !isxdigit(c2)) return -1;
    int v1 = isdigit(c1) ? c1 - '0' : tolower(c1) - 'a' + 10;
    int v2 = isdigit(c2) ? c2 - '0' : tolower(c2) - 'a' + 10;
    return (v1 << 4) | v2;
}

int parse_hex(const char *hex, unsigned char **out) {
    int len = strlen(hex);
    if (len % 2 != 0) return -1;
    int byte_len = len / 2;
    *out = malloc(byte_len);
    for (int i = 0; i < byte_len; i++) {
        int b = hex_to_byte(hex[2*i], hex[2*i+1]);
        if (b < 0) { free(*out); return -1; }
        (*out)[i] = b;
    }
    return byte_len;
}

int main() {
    char cmd[20], name[256], hex[2048];
    int offset, length;
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "NEW") == 0) {
            scanf("%s %s", name, hex);
            unsigned char *data;
            int size = parse_hex(hex, &data);
            if (size >= 0) {
                Buf *b = malloc(sizeof(Buf));
                b->name = strdup(name); b->data = data; b->size = size; b->next = head; head = b;
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            scanf("%s %s", name, hex);
            unsigned char *data;
            int size = parse_hex(hex, &data);
            if (size >= 0) {
                for (Buf *b = head; b; b = b->next) {
                    if (strcmp(b->name, name) == 0) {
                        b->data = realloc(b->data, b->size + size);
                        memcpy(b->data + b->size, data, size);
                        b->size += size;
                        break;
                    }
                }
                free(data);
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            scanf("%s %d %d", name, &offset, &length);
            for (Buf *b = head; b; b = b->next) {
                if (strcmp(b->name, name) == 0) {
                    if (offset < 0 || length < 0 || offset + length > b->size) break;
                    for (int i = 0; i < length; i++) printf("%02x", b->data[offset + i]);
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", name);
            Buf *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, name) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->name); free(curr->data); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Buf *b = head; b; b = b->next) printf("%s %d\n", b->name, b->size);
        }
    }
    return 0;
}