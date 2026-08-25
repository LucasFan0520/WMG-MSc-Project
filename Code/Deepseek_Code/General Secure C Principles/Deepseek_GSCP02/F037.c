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

int hex_to_byte(const char *hex) {
    if (strlen(hex) != 2) return -1;
    for (int i = 0; i < 2; i++) {
        if (!isxdigit(hex[i])) return -1;
    }
    unsigned int val;
    sscanf(hex, "%02x", &val);
    return (int)val;
}

void create_buffer(const char *name, const char *hexstr) {
    if (find_buffer(name)) return;
    Buffer *b = malloc(sizeof(Buffer));
    if (!b) return;
    b->name = strdup(name);
    b->bytes = NULL;
    b->size = 0;
    int len = strlen(hexstr);
    if (len % 2 != 0) {
        free(b->name);
        free(b);
        return;
    }
    unsigned char *data = malloc(len / 2);
    if (!data) {
        free(b->name);
        free(b);
        return;
    }
    int ok = 1;
    int count = 0;
    for (int i = 0; i < len; i += 2) {
        char pair[3] = {hexstr[i], hexstr[i+1], 0};
        int val = hex_to_byte(pair);
        if (val < 0) { ok = 0; break; }
        data[count++] = (unsigned char)val;
    }
    if (!ok) {
        free(b->name);
        free(data);
        free(b);
        return;
    }
    b->bytes = data;
    b->size = count;
    b->next = head;
    head = b;
}

void append_bytes(const char *name, const char *hexstr) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    int len = strlen(hexstr);
    if (len % 2 != 0) return;
    unsigned char *new_data = malloc(b->size + len / 2);
    if (!new_data) return;
    memcpy(new_data, b->bytes, b->size);
    int count = b->size;
    int ok = 1;
    for (int i = 0; i < len; i += 2) {
        char pair[3] = {hexstr[i], hexstr[i+1], 0};
        int val = hex_to_byte(pair);
        if (val < 0) { ok = 0; break; }
        new_data[count++] = (unsigned char)val;
    }
    if (!ok) {
        free(new_data);
        return;
    }
    free(b->bytes);
    b->bytes = new_data;
    b->size = count;
}

void slice_buffer(const char *name, int offset, int length) {
    Buffer *b = find_buffer(name);
    if (!b) return;
    if (offset < 0 || offset >= b->size) return;
    if (length <= 0 || offset + length > b->size) return;
    for (int i = 0; i < length; i++) {
        printf("%02x", b->bytes[offset + i]);
    }
    printf("\n");
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
    while (head) {
        Buffer *next = head->next;
        free(head->name);
        free(head->bytes);
        free(head);
        head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "NEW") == 0) {
            char name[256], hexstr[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", name, hexstr) == 2) {
                create_buffer(name, hexstr);
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char name[256], hexstr[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", name, hexstr) == 2) {
                append_bytes(name, hexstr);
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char name[256];
            int offset, length;
            if (sscanf(line, "%*s %255s %d %d", name, &offset, &length) == 3) {
                slice_buffer(name, offset, length);
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                delete_buffer(name);
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            list_buffers();
        }
    }

    free(line);
    free_all();
    return 0;
}