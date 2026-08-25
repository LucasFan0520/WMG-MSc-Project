// F012.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *name; char *note; } Per;
typedef struct { Per *d; size_t len, cap; } QL;

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
    QL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "JOIN") == 0 || strcmp(cmd, "REJOIN") == 0) {
            char *name = next_token(&p);
            char *note = p;
            if (name && note) {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Per *nd = realloc(list.d, nc * sizeof(Per));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].name = strdup(name);
                    list.d[list.len].note = strdup(note);
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "SERVE") == 0) {
            if (list.len > 0) {
                printf("%s\n", list.d[0].name);
                free(list.d[0].name); free(list.d[0].note);
                for (size_t i = 0; i < list.len - 1; i++) list.d[i] = list.d[i + 1];
                list.len--;
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = next_token(&p);
            if (name) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].name, name) == 0) {
                        free(list.d[i].name); free(list.d[i].note);
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--; break;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = next_token(&p);
            if (name) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].name, name) == 0) {
                        print_text(list.d[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "QUEUE") == 0) {
            for (size_t i = 0; i < list.len; i++) printf("%s\n", list.d[i].name);
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) { free(list.d[i].name); free(list.d[i].note); }
    free(list.d);
    return 0;
}