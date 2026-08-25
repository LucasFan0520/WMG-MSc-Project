// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *name; char *exp; } Alias;
typedef struct { Alias *d; size_t len, cap; } AL;

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
    AL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "DEFINE") == 0) {
            char *name = next_token(&p);
            char *exp = p;
            if (name && exp) {
                int idx = -1;
                for (size_t i = 0; i < list.len; i++) if (strcmp(list.d[i].name, name) == 0) { idx = i; break; }
                if (idx != -1) {
                    char *ne = strdup(exp);
                    if (ne) { free(list.d[idx].exp); list.d[idx].exp = ne; }
                } else {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        Alias *nd = realloc(list.d, nc * sizeof(Alias));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        list.d[list.len].name = strdup(name);
                        list.d[list.len].exp = strdup(exp);
                        list.len++;
                    }
                }
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *name = next_token(&p);
            if (name) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].name, name) == 0) { print_text(list.d[i].exp); break; }
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *old = next_token(&p);
            char *new_n = next_token(&p);
            if (old && new_n) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].name, old) == 0) {
                        char *nn = strdup(new_n);
                        if (nn) { free(list.d[i].name); list.d[i].name = nn; }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *name = next_token(&p);
            if (name) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].name, name) == 0) {
                        free(list.d[i].name); free(list.d[i].exp);
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--; break;
                    }
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s ", list.d[i].name);
                print_text(list.d[i].exp);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) { free(list.d[i].name); free(list.d[i].exp); }
    free(list.d);
    return 0;
}