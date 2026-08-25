// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 65536

typedef struct Buf {
    char *name;
    unsigned char *data;
    size_t size;
    size_t cap;
    struct Buf *next;
} Buf;

Buf *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

Buf *find_buf(const char *name) {
    for (Buf *b = head; b; b = b->next)
        if (strcmp(b->name, name) == 0) return b;
    return NULL;
}

void append_hex(Buf *b, const char *hex) {
    size_t len = strlen(hex);
    if (len % 2 != 0) return;
    size_t bytes = len / 2;
    if (b->size + bytes > b->cap) {
        size_t new_cap = b->cap ? b->cap * 2 : 16;
        while (new_cap < b->size + bytes) new_cap *= 2;
        unsigned char *tmp = realloc(b->data, new_cap);
        if (!tmp) return;
        b->data = tmp;
        b->cap = new_cap;
    }
    for (size_t i = 0; i < bytes; i++) {
        unsigned int val;
        if (sscanf(hex + i * 2, "%2x", &val) != 1) return;
        b->data[b->size++] = (unsigned char)val;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "NEW") == 0) {
            char *name = get_token(&ptr);
            if (name && !find_buf(name)) {
                Buf *b = malloc(sizeof(Buf));
                if (b) {
                    b->name = strdup(name);
                    b->data = NULL;
                    b->size = 0;
                    b->cap = 0;
                    b->next = head;
                    head = b;
                    append_hex(b, ptr);
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Buf *b = find_buf(name);
                if (b) append_hex(b, ptr);
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *name = get_token(&ptr);
            char *ostr = get_token(&ptr);
            char *lstr = get_token(&ptr);
            if (name && ostr && lstr) {
                size_t offset = strtoul(ostr, NULL, 10);
                size_t length = strtoul(lstr, NULL, 10);
                Buf *b = find_buf(name);
                if (b && offset + length <= b->size) {
                    for (size_t i = 0; i < length; i++) {
                        printf("%02x", b->data[offset + i]);
                    }
                    printf("\n");
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = get_token(&ptr);
            if (name) {
                Buf *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->name, name) == 0) {
                        if (prev) prev->next = curr->next;
                        else head = curr->next;
                        free(curr->name);
                        free(curr->data);
                        free(curr);
                        break;
                    }
                    prev = curr;
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Buf *b = head; b; b = b->next) {
                printf("%s %zu\n", b->name, b->size);
            }
        }
    }
    while (head) {
        Buf *next = head->next;
        free(head->name);
        free(head->data);
        free(head);
        head = next;
    }
    return 0;
}