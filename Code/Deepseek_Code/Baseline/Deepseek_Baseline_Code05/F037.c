/* F037.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct buffer {
    char *name;
    unsigned char *bytes;
    int len;
    struct buffer *next;
} Buffer;

Buffer *head = NULL;

Buffer *find_buffer(const char *name) {
    Buffer *b = head;
    while (b) {
        if (strcmp(b->name, name) == 0) return b;
        b = b->next;
    }
    return NULL;
}

unsigned char hex_to_byte(const char *s) {
    int val = 0;
    for (int i = 0; i < 2; i++) {
        char c = s[i];
        if (c >= '0' && c <= '9') val = val * 16 + (c - '0');
        else if (c >= 'a' && c <= 'f') val = val * 16 + (c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') val = val * 16 + (c - 'A' + 10);
        else return 0;
    }
    return (unsigned char)val;
}

int parse_hexbytes(const char *str, unsigned char **out) {
    int len = strlen(str);
    if (len % 2 != 0) return -1;
    unsigned char *data = malloc(len / 2);
    for (int i = 0; i < len; i += 2) {
        data[i/2] = hex_to_byte(str + i);
    }
    *out = data;
    return len / 2;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    unsigned char *data = NULL;
    int len = parse_hexbytes(hex, &data);
    if (len < 0) { free(data); return; }
    Buffer *b = malloc(sizeof(Buffer));
    b->name = strdup(name);
    b->bytes = data;
    b->len = len;
    b->next = head;
    head = b;
}

void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    unsigned char *data = NULL;
    int len = parse_hexbytes(hex, &data);
    if (len < 0) { free(data); return; }
    b->bytes = realloc(b->bytes, b->len + len);
    memcpy(b->bytes + b->len, data, len);
    b->len += len;
    free(data);
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= b->len || length < 0) return;
    if (offset + length > b->len) length = b->len - offset;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->bytes[offset + i]);
    }
    putchar('\n');
}

void delete_buffer(const char *name) {
    Buffer *b = head, *prev = NULL;
    while (b) {
        if (strcmp(b->name, name) == 0) {
            if (prev) prev->next = b->next;
            else head = b->next;
            free(b->name);
            free(b->bytes);
            free(b);
            return;
        }
        prev = b;
        b = b->next;
    }
}

void list_buffers() {
    Buffer *b = head;
    while (b) {
        printf("%s %d\n", b->name, b->len);
        b = b->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], name[256], hex[1024];
        int offset, length;
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "LIST") == 0) { list_buffers(); continue; }
        if (strcmp(cmd, "NEW") == 0) {
            if (sscanf(line, "%*s %255s", name) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(name) + 1;
            if (*rest == ' ') rest++;
            new_buffer(name, rest);
        } else if (strcmp(cmd, "APPEND") == 0) {
            if (sscanf(line, "%*s %255s", name) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(name) + 1;
            if (*rest == ' ') rest++;
            append_buffer(name, rest);
        } else if (strcmp(cmd, "SLICE") == 0) {
            if (sscanf(line, "%*s %255s %d %d", name, &offset, &length) == 3)
                slice_buffer(name, offset, length);
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1) delete_buffer(name);
        }
    }
    return 0;
}