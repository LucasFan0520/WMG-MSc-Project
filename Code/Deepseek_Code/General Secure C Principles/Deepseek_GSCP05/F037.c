// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct Buffer {
    char *name;
    unsigned char *data;
    int size;
    struct Buffer *next;
} Buffer;

Buffer *buffers = NULL;

Buffer *find_buffer(const char *name) {
    Buffer *b = buffers;
    while (b) {
        if (strcmp(b->name, name) == 0) return b;
        b = b->next;
    }
    return NULL;
}

int hex_to_byte(const char *s) {
    if (!isxdigit(s[0]) || !isxdigit(s[1])) return -1;
    int val;
    sscanf(s, "%2x", &val);
    return val;
}

void create_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    int size = len / 2;
    unsigned char *data = malloc(size);
    if (!data) return;
    for (int i = 0; i < size; i++) {
        int byte = hex_to_byte(hex + 2*i);
        if (byte < 0) { free(data); return; }
        data[i] = (unsigned char)byte;
    }
    Buffer *b = malloc(sizeof(Buffer));
    if (!b) { free(data); return; }
    b->name = strdup(name);
    b->data = data;
    b->size = size;
    b->next = buffers;
    buffers = b;
}

void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    int add_size = len / 2;
    unsigned char *new_data = realloc(b->data, b->size + add_size);
    if (!new_data) return;
    b->data = new_data;
    for (int i = 0; i < add_size; i++) {
        int byte = hex_to_byte(hex + 2*i);
        if (byte < 0) { b->size += i; return; } // partial append? spec says ignore invalid? We'll rollback? For simplicity, if invalid, we stop and keep partial? Safer: check all first.
        // check all first
    }
    // validate all before appending
    for (int i = 0; i < add_size; i++) {
        if (hex_to_byte(hex + 2*i) < 0) return;
    }
    for (int i = 0; i < add_size; i++) {
        b->data[b->size + i] = (unsigned char)hex_to_byte(hex + 2*i);
    }
    b->size += add_size;
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= b->size || length <= 0) return;
    if (offset + length > b->size) length = b->size - offset;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->data[offset + i]);
    }
    putchar('\n');
}

void delete_buffer(const char *name) {
    Buffer *prev = NULL, *cur = buffers;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else buffers = cur->next;
            free(cur->name);
            free(cur->data);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void list_buffers(void) {
    Buffer *b = buffers;
    while (b) {
        printf("%s %d\n", b->name, b->size);
        b = b->next;
    }
}

void free_all(void) {
    Buffer *b = buffers;
    while (b) {
        Buffer *next = b->next;
        free(b->name);
        free(b->data);
        free(b);
        b = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "NEW") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *hex = space + 1;
            while (*hex == ' ') hex++;
            create_buffer(name, hex);
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *hex = space + 1;
            while (*hex == ' ') hex++;
            append_buffer(name, hex);
        } else if (strcmp(cmd, "SLICE") == 0) {
            int offset, length;
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            if (sscanf(space + 1, "%d %d", &offset, &length) != 2) continue;
            slice_buffer(name, offset, length);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            delete_buffer(name);
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }
    free_all();
    return 0;
}