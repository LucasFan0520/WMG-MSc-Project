// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct buf {
    char *name;
    unsigned char *data;
    int size;
    struct buf *next;
} Buf;

Buf *head = NULL;
Buf *tail = NULL;

Buf *find_buf(const char *name) {
    Buf *b = head;
    while (b) {
        if (strcmp(b->name, name) == 0) return b;
        b = b->next;
    }
    return NULL;
}

int hex_to_byte(const char *s) {
    int val = 0;
    if (s[0] >= '0' && s[0] <= '9') val = s[0] - '0';
    else if (s[0] >= 'A' && s[0] <= 'F') val = s[0] - 'A' + 10;
    else if (s[0] >= 'a' && s[0] <= 'f') val = s[0] - 'a' + 10;
    else return -1;
    val <<= 4;
    if (s[1] >= '0' && s[1] <= '9') val += s[1] - '0';
    else if (s[1] >= 'A' && s[1] <= 'F') val += s[1] - 'A' + 10;
    else if (s[1] >= 'a' && s[1] <= 'f') val += s[1] - 'a' + 10;
    else return -1;
    return val;
}

unsigned char *parse_hex(const char *str, int *out_len) {
    int len = strlen(str);
    if (len % 2 != 0) { *out_len = -1; return NULL; }
    unsigned char *bytes = malloc(len / 2);
    for (int i = 0; i < len; i += 2) {
        int b = hex_to_byte(str + i);
        if (b < 0) { free(bytes); *out_len = -1; return NULL; }
        bytes[i/2] = (unsigned char)b;
    }
    *out_len = len / 2;
    return bytes;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buf(name)) return;
    int len;
    unsigned char *data = parse_hex(hex, &len);
    if (len < 0) return;
    Buf *b = malloc(sizeof(Buf));
    b->name = strdup(name);
    b->data = data;
    b->size = len;
    b->next = NULL;
    if (tail) tail->next = b;
    else head = b;
    tail = b;
}

void append_buffer(const char *name, const char *hex) {
    Buf *b = find_buf(name);
    if (!b) return;
    int len;
    unsigned char *data = parse_hex(hex, &len);
    if (len < 0) return;
    b->data = realloc(b->data, b->size + len);
    memcpy(b->data + b->size, data, len);
    b->size += len;
    free(data);
}

void slice_buffer(const char *name, int offset, int length) {
    Buf *b = find_buf(name);
    if (!b || offset < 0 || length < 0 || offset + length > b->size) return;
    printf("%02x", b->data[offset]);
    for (int i = 1; i < length; i++)
        printf("%02x", b->data[offset + i]);
    putchar('\n');
}

void delete_buffer(const char *name) {
    Buf *b = head, *prev = NULL;
    while (b) {
        if (strcmp(b->name, name) == 0) {
            if (prev) prev->next = b->next;
            else head = b->next;
            if (b == tail) tail = prev;
            free(b->name);
            free(b->data);
            free(b);
            return;
        }
        prev = b;
        b = b->next;
    }
}

void list_buffers(void) {
    Buf *b = head;
    while (b) {
        printf("%s %d\n", b->name, b->size);
        b = b->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char name[256], hex[4096];
        int off, len;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "NEW") == 0) {
            if (sscanf(line, "%*s %255s %s", name, hex) == 2)
                new_buffer(name, hex);
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(line, "%*s %255s %s", name, hex) == 2)
                append_buffer(name, hex);
        } else if (strcmp(cmd, "SLICE") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &off, &len) == 3)
                slice_buffer(name, off, len);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                delete_buffer(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    return 0;
}