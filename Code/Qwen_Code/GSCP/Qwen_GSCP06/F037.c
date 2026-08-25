// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 8192

char* get_word(char** str) {
    while (**str && isspace((unsigned char)**str)) (*str)++;
    if (!**str) return NULL;
    char* start = *str;
    while (**str && !isspace((unsigned char)**str)) (*str)++;
    if (**str) { **str = '\0'; (*str)++; }
    return start;
}

typedef struct Buf {
    char* name;
    unsigned char* data;
    size_t len;
    struct Buf* next;
} Buf;

Buf* head = NULL;

Buf* find_buf(const char* name) {
    for (Buf* b = head; b; b = b->next)
        if (strcmp(b->name, name) == 0) return b;
    return NULL;
}

int parse_hex(const char* hex, unsigned char** out) {
    size_t slen = strlen(hex);
    if (slen % 2 != 0) return 0;
    size_t blen = slen / 2;
    unsigned char* buf = malloc(blen);
    for (size_t i = 0; i < blen; i++) {
        unsigned int val;
        if (sscanf(hex + i * 2, "%2x", &val) != 1) {
            free(buf);
            return 0;
        }
        buf[i] = (unsigned char)val;
    }
    *out = buf;
    return blen;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char* p = line;
        char* cmd = get_word(&p);
        if (!cmd) continue;

        if (strcmp(cmd, "NEW") == 0) {
            char* name = get_word(&p);
            char* hex = get_word(&p);
            if (name && hex) {
                unsigned char* data;
                int len = parse_hex(hex, &data);
                if (len > 0) {
                    Buf* n = malloc(sizeof(Buf));
                    n->name = strdup(name);
                    n->data = data;
                    n->len = len;
                    n->next = head;
                    head = n;
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char* name = get_word(&p);
            char* hex = get_word(&p);
            Buf* b = find_buf(name);
            if (b && hex) {
                unsigned char* data;
                int len = parse_hex(hex, &data);
                if (len > 0) {
                    b->data = realloc(b->data, b->len + len);
                    memcpy(b->data + b->len, data, len);
                    b->len += len;
                    free(data);
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char* name = get_word(&p);
            char* off_str = get_word(&p);
            char* len_str = get_word(&p);
            Buf* b = find_buf(name);
            if (b && off_str && len_str) {
                int off = atoi(off_str);
                int len = atoi(len_str);
                if (off >= 0 && len >= 0 && off + len <= (int)b->len) {
                    for (int i = 0; i < len; i++) {
                        printf("%02x", b->data[off + i]);
                    }
                    printf("\n");
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char* name = get_word(&p);
            Buf* prev = NULL;
            for (Buf* b = head; b; prev = b, b = b->next) {
                if (strcmp(b->name, name) == 0) {
                    if (prev) prev->next = b->next;
                    else head = b->next;
                    free(b->name);
                    free(b->data);
                    free(b);
                    break;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (Buf* b = head; b; b = b->next) {
                printf("%s %zu\n", b->name, b->len);
            }
        }
    }
    return 0;
}