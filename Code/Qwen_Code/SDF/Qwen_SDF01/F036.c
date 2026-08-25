// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *name; char *data; } Rec;
typedef struct { Rec *d; size_t len, cap; } RL;

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

void print_text(const char *t) {
    for (size_t i = 0; t[i]; i++) putchar(t[i] == ' ' ? '_' : t[i]);
    putchar('\n');
}

int find_rec(RL *l, const char *name) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].name, name) == 0) return i;
    return -1;
}

int main(void) {
    RL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "ADD") == 0) {
            char *name = next_token(&p);
            char *len_data = p;
            if (name && len_data) {
                char *colon = strchr(len_data, ':');
                if (colon) {
                    *colon = '\0';
                    long dlen = strtol(len_data, NULL, 10);
                    char *data = colon + 1;
                    if (dlen >= 0 && (size_t)dlen == strlen(data)) {
                        if (find_rec(&list, name) == -1) {
                            if (list.len == list.cap) {
                                size_t nc = list.cap ? list.cap * 2 : 4;
                                Rec *nd = realloc(list.d, nc * sizeof(Rec));
                                if (nd) { list.d = nd; list.cap = nc; }
                            }
                            if (list.len < list.cap) {
                                list.d[list.len].name = strdup(name);
                                list.d[list.len].data = strdup(data);
                                list.len++;
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = next_token(&p);
            if (name) {
                int idx = find_rec(&list, name);
                if (idx != -1) {
                    free(list.d[idx].name);
                    free(list.d[idx].data);
                    for (size_t i = idx; i < list.len - 1; i++) list.d[i] = list.d[i + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = next_token(&p);
            if (name) {
                int idx = find_rec(&list, name);
                if (idx != -1) {
                    print_text(list.d[idx].data);
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s\n", list.d[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].name);
        free(list.d[i].data);
    }
    free(list.d);
    return 0;
}