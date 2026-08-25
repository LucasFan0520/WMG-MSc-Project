// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Buffer {
    char *name;
    unsigned char *bytes;
    int size;
    struct Buffer *next;
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

int hex_to_byte(const char *s) {
    int val = 0;
    for (int i = 0; i < 2; i++) {
        char c = s[i];
        if (c >= '0' && c <= '9') val = val*16 + (c-'0');
        else if (c >= 'A' && c <= 'F') val = val*16 + (c-'A'+10);
        else if (c >= 'a' && c <= 'f') val = val*16 + (c-'a'+10);
        else return -1;
    }
    return val;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    Buffer *b = malloc(sizeof(Buffer));
    b->name = malloc(strlen(name) + 1);
    strcpy(b->name, name);
    b->size = len / 2;
    b->bytes = malloc(b->size);
    int ok = 1;
    for (int i = 0; i < b->size; i++) {
        int byte = hex_to_byte(hex + 2*i);
        if (byte < 0) { ok = 0; break; }
        b->bytes[i] = (unsigned char)byte;
    }
    if (!ok) {
        free(b->name);
        free(b->bytes);
        free(b);
        return;
    }
    b->next = head;
    head = b;
}

void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    int add_size = len / 2;
    unsigned char *new_bytes = malloc(b->size + add_size);
    memcpy(new_bytes, b->bytes, b->size);
    int ok = 1;
    for (int i = 0; i < add_size; i++) {
        int byte = hex_to_byte(hex + 2*i);
        if (byte < 0) { ok = 0; break; }
        new_bytes[b->size + i] = (unsigned char)byte;
    }
    if (!ok) {
        free(new_bytes);
        return;
    }
    free(b->bytes);
    b->bytes = new_bytes;
    b->size += add_size;
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= b->size || length <= 0 || offset + length > b->size) return;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->bytes[offset + i]);
    }
    putchar('\n');
}

void delete_buffer(const char *name) {
    Buffer *prev = NULL;
    Buffer *b = head;
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

void list_buffers(void) {
    Buffer *b = head;
    while (b) {
        printf("%s %d\n", b->name, b->size);
        b = b->next;
    }
}

void free_all(void) {
    Buffer *b = head;
    while (b) {
        Buffer *next = b->next;
        free(b->name);
        free(b->bytes);
        free(b);
        b = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "NEW") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            new_buffer(name, p);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            append_buffer(name, p);
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            int offset, length;
            if (sscanf(p, "%d %d", &offset, &length) == 2) {
                slice_buffer(name, offset, length);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            delete_buffer(p);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    free_all();
    return 0;
}