// F028.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *stop; char *note; } Stop;
typedef struct { Stop *d; size_t len, cap; } SL;

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

int main(void) {
    SL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "ADD") == 0) {
            char *stop = next_token(&p);
            char *note = p;
            if (stop && note) {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Stop *nd = realloc(list.d, nc * sizeof(Stop));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].stop = strdup(stop);
                    list.d[list.len].note = strdup(note);
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "INSERT") == 0) {
            char *idx_s = next_token(&p);
            char *stop = next_token(&p);
            char *note = p;
            if (idx_s && stop && note) {
                long idx = strtol(idx_s, NULL, 10);
                if (idx >= 0 && (size_t)idx <= list.len) {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        Stop *nd = realloc(list.d, nc * sizeof(Stop));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        for (size_t i = list.len; i > (size_t)idx; i--) list.d[i] = list.d[i - 1];
                        list.d[idx].stop = strdup(stop);
                        list.d[idx].note = strdup(note);
                        list.len++;
                    }
                }
            }
        } else if (strcmp(cmd, "MOVE") == 0) {
            char *from_s = next_token(&p);
            char *to_s = p;
            if (from_s && to_s) {
                long from = strtol(from_s, NULL, 10);
                long to = strtol(to_s, NULL, 10);
                if (from >= 0 && (size_t)from < list.len && to >= 0 && (size_t)to < list.len && from != to) {
                    Stop tmp = list.d[from];
                    if (from < to) {
                        for (size_t i = from; i < to; i++) list.d[i] = list.d[i + 1];
                    } else {
                        for (size_t i = from; i > to; i--) list.d[i] = list.d[i - 1];
                    }
                    list.d[to] = tmp;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *idx_s = next_token(&p);
            if (idx_s) {
                long idx = strtol(idx_s, NULL, 10);
                if (idx >= 0 && (size_t)idx < list.len) {
                    free(list.d[idx].stop);
                    free(list.d[idx].note);
                    for (size_t i = idx; i < list.len - 1; i++) list.d[i] = list.d[i + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *stop = next_token(&p);
            if (stop) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].stop, stop) == 0) {
                        print_text(list.d[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s ", list.d[i].stop);
                print_text(list.d[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].stop);
        free(list.d[i].note);
    }
    free(list.d);
    return 0;
}