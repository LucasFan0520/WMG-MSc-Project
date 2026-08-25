// F027.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { long pri; char *title; char *note; size_t order; } Rem;
typedef struct { Rem *d; size_t len, cap; } RL;

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

int cmp_rem(const void *a, const void *b) {
    const Rem *aa = a;
    const Rem *bb = b;
    if (aa->pri != bb->pri) return (bb->pri > aa->pri) - (bb->pri < aa->pri);
    return (aa->order > bb->order) - (aa->order < bb->order);
}

int main(void) {
    RL list = {0};
    size_t order_cnt = 0;
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "ADD") == 0) {
            char *pri_s = next_token(&p);
            char *title = next_token(&p);
            char *note = p;
            if (pri_s && title && note) {
                long pri = strtol(pri_s, NULL, 10);
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Rem *nd = realloc(list.d, nc * sizeof(Rem));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].pri = pri;
                    list.d[list.len].title = strdup(title);
                    list.d[list.len].note = strdup(note);
                    list.d[list.len].order = order_cnt++;
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "BUMP") == 0) {
            char *title = next_token(&p);
            char *delta_s = p;
            if (title && delta_s) {
                long delta = strtol(delta_s, NULL, 10);
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].title, title) == 0) {
                        list.d[i].pri += delta;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = next_token(&p);
            if (title) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].title, title) == 0) {
                        free(list.d[i].title);
                        free(list.d[i].note);
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "TOP") == 0) {
            char *n_s = next_token(&p);
            if (n_s) {
                long n = strtol(n_s, NULL, 10);
                if (n > 0 && list.len > 0) {
                    Rem *tmp = malloc(list.len * sizeof(Rem));
                    if (tmp) {
                        memcpy(tmp, list.d, list.len * sizeof(Rem));
                        qsort(tmp, list.len, sizeof(Rem), cmp_rem);
                        size_t lim = (size_t)n < list.len ? (size_t)n : list.len;
                        for (size_t i = 0; i < lim; i++) {
                            printf("%ld %s ", tmp[i].pri, tmp[i].title);
                            print_text(tmp[i].note);
                        }
                        free(tmp);
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%ld %s ", list.d[i].pri, list.d[i].title);
                print_text(list.d[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].title);
        free(list.d[i].note);
    }
    free(list.d);
    return 0;
}