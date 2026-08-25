/* F037.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int hex_to_byte(const char *hex) {
    int val = 0;
    for (int i = 0; i < 2; i++) {
        char c = hex[i];
        if (c >= '0' && c <= '9') val = val * 16 + (c - '0');
        else if (c >= 'A' && c <= 'F') val = val * 16 + (c - 'A' + 10);
        else if (c >= 'a' && c <= 'f') val = val * 16 + (c - 'a' + 10);
        else return -1;
    }
    return val;
}

void new_buffer(const char *name, const char *hex) {
    if (find_buffer(name)) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    int byte_count = len / 2;
    unsigned char *bytes = malloc(byte_count);
    if (!bytes) return;
    for (int i = 0; i < byte_count; i++) {
        int b = hex_to_byte(hex + 2 * i);
        if (b < 0) {
            free(bytes);
            return;
        }
        bytes[i] = (unsigned char)b;
    }
    Buffer *buf = malloc(sizeof(Buffer));
    if (!buf) { free(bytes); return; }
    buf->name = strdup(name);
    buf->bytes = bytes;
    buf->size = byte_count;
    buf->next = head;
    head = buf;
}

void append_buffer(const char *name, const char *hex) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    int len = strlen(hex);
    if (len % 2 != 0) return;
    int byte_count = len / 2;
    unsigned char *new_bytes = realloc(b->bytes, b->size + byte_count);
    if (!new_bytes) return;
    b->bytes = new_bytes;
    for (int i = 0; i < byte_count; i++) {
        int val = hex_to_byte(hex + 2 * i);
        if (val < 0) {
            // should we revert? Ignore.
            return;
        }
        b->bytes[b->size + i] = (unsigned char)val;
    }
    b->size += byte_count;
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= b->size || length <= 0 || offset + length > b->size) {
        printf("INVALID\n");
        return;
    }
    for (int i = 0; i < length; i++) {
        printf("%02X", b->bytes[offset + i]);
    }
    printf("\n");
}

void delete_buffer(const char *name) {
    Buffer *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->bytes);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
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
        Buffer *tmp = b;
        b = b->next;
        free(tmp->name);
        free(tmp->bytes);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "NEW") == 0) {
            char name[256], hex[512];
            if (sscanf(line + 4, "%255s %511s", name, hex) == 2) {
                new_buffer(name, hex);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char name[256], hex[512];
            if (sscanf(line + 7, "%255s %511s", name, hex) == 2) {
                append_buffer(name, hex);
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char name[256];
            int offset, length;
            if (sscanf(line + 6, "%255s %d %d", name, &offset, &length) == 3) {
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