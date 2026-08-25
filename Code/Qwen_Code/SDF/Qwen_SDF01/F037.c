// F037.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *name; uint8_t *bytes; size_t len, cap; } Buf;
typedef struct { Buf *d; size_t len, cap; } BL;

char *read_line(void) {
    char *l = NULL;
    size_t len = 0;
    if (getline(&l, &len, stdin) == -1) {
        free(l);
        return NULL;
    }
    size_t sl = strlen(l);
    if (sl > 0 && l[sl - 1] == '\n') l[sl - 1] = '\0';
    return l;
}

char *next_token(char **s) {
    if (!*s) return NULL;
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *st = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return st;
}

int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

int parse_hex(const char *hex, uint8_t **out, size_t *out_len) {
    size_t hlen = strlen(hex);
    if (hlen % 2 != 0) return 0;
    size_t blen = hlen / 2;
    uint8_t *b = malloc(blen);
    if (!b) return 0;
    for (size_t i = 0; i < blen; i++) {
        int h = hex_val(hex[2 * i]);
        int l = hex_val(hex[2 * i + 1]);
        if (h == -1 || l == -1) {
            free(b);
            return 0;
        }
        b[i] = (h << 4) | l;
    }
    *out = b;
    *out_len = blen;
    return 1;
}

int find_buf(BL *l, const char *name) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].name, name) == 0) return i;
    return -1;
}

int main(void) {
    BL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "NEW") == 0) {
            char *name = next_token(&p);
            char *hex = p;
            if (name && hex) {
                uint8_t *b;
                size_t blen;
                if (parse_hex(hex, &b, &blen)) {
                    if (find_buf(&list, name) == -1) {
                        if (list.len == list.cap) {
                            size_t nc = list.cap ? list.cap * 2 : 4;
                            Buf *nd = realloc(list.d, nc * sizeof(Buf));
                            if (nd) { list.d = nd; list.cap = nc; }
                        }
                        if (list.len < list.cap) {
                            list.d[list.len].name = strdup(name);
                            list.d[list.len].bytes = b;
                            list.d[list.len].len = blen;
                            list.d[list.len].cap = blen;
                            list.len++;
                        } else {
                            free(b);
                        }
                    } else {
                        free(b);
                    }
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *name = next_token(&p);
            char *hex = p;
            if (name && hex) {
                int idx = find_buf(&list, name);
                if (idx != -1) {
                    uint8_t *b;
                    size_t blen;
                    if (parse_hex(hex, &b, &blen)) {
                        size_t nlen = list.d[idx].len + blen;
                        if (nlen > list.d[idx].cap) {
                            size_t nc = nlen;
                            uint8_t *nb = realloc(list.d[idx].bytes, nc);
                            if (nb) {
                                list.d[idx].bytes = nb;
                                list.d[idx].cap = nc;
                            }
                        }
                        if (nlen <= list.d[idx].cap) {
                            memcpy(list.d[idx].bytes + list.d[idx].len, b, blen);
                            list.d[idx].len = nlen;
                        }
                        free(b);
                    }
                }
            }
        } else if (strcmp(cmd, "SLICE") == 0) {
            char *name = next_token(&p);
            char *off_s = next_token(&p);
            char *len_s = p;
            if (name && off_s && len_s) {
                int idx = find_buf(&list, name);
                if (idx != -1) {
                    long off = strtol(off_s, NULL, 10);
                    long slen = strtol(len_s, NULL, 10);
                    if (off >= 0 && slen >= 0 && (size_t)off + (size_t)slen <= list.d[idx].len) {
                        for (long i = 0; i < slen; i++) {
                            printf("%02x", list.d[idx].bytes[off + i]);
                        }
                        printf("\n");
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = next_token(&p);
            if (name) {
                int idx = find_buf(&list, name);
                if (idx != -1) {
                    free(list.d[idx].name);
                    free(list.d[idx].bytes);
                    for (size_t i = idx; i < list.len - 1; i++) list.d[i] = list.d[i + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s %zu\n", list.d[i].name, list.d[i].len);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].name);
        free(list.d[i].bytes);
    }
    free(list.d);
    return 0;
}