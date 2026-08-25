// F005.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *d; size_t len, cap; } Lines;

char *next_token(char **s) {
    if (!*s) return NULL;
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *st = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return st;
}

char *read_line(void) {
    char *l = NULL; size_t len = 0;
    if (getline(&l, &len, stdin) == -1) { free(l); return NULL; }
    size_t sl = strlen(l);
    if (sl > 0 && l[sl - 1] == '\n') l[sl - 1] = '\0';
    return l;
}

void print_text(const char *t) {
    for (size_t i = 0; t[i]; i++) putchar(t[i] == ' ' ? '_' : t[i]);
    putchar('\n');
}

int main(void) {
    Lines list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "INSERT") == 0) {
            char *idx_s = next_token(&p);
            char *text = p;
            if (idx_s && text) {
                long idx = strtol(idx_s, NULL, 10);
                if (idx >= 0 && (size_t)idx <= list.len) {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        char **nd = realloc(list.d, nc * sizeof(char*));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        for (size_t i = list.len; i > (size_t)idx; i--) list.d[i] = list.d[i - 1];
                        list.d[idx] = strdup(text);
                        list.len++;
                    }
                }
            }
        } else if (strcmp(cmd, "APPEND") == 0) {
            char *text = p;
            if (text) {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    char **nd = realloc(list.d, nc * sizeof(char*));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len++] = strdup(text);
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *idx_s = next_token(&p);
            if (idx_s) {
                long idx = strtol(idx_s, NULL, 10);
                if (idx >= 0 && (size_t)idx < list.len) {
                    free(list.d[idx]);
                    for (size_t i = idx; i < list.len - 1; i++) list.d[i] = list.d[i + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "PATCH") == 0) {
            char *idx_s = next_token(&p);
            char *text = p;
            if (idx_s && text) {
                long idx = strtol(idx_s, NULL, 10);
                if (idx >= 0 && (size_t)idx < list.len) {
                    char *nt = strdup(text);
                    if (nt) { free(list.d[idx]); list.d[idx] = nt; }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < list.len; i++) print_text(list.d[i]);
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) free(list.d[i]);
    free(list.d);
    return 0;
}