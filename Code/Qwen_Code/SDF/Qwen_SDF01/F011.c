// F011.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *title; char *body; } Draft;
typedef struct { Draft *d; size_t len, cap; } DL;
typedef struct { Draft *d; size_t len, cap; } Trash;

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
    DL list = {0};
    Trash trash = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "CREATE") == 0) {
            char *title = next_token(&p);
            char *body = p;
            if (title && body) {
                int found = 0;
                for (size_t i = 0; i < list.len; i++) if (strcmp(list.d[i].title, title) == 0) { found = 1; break; }
                if (!found) {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        Draft *nd = realloc(list.d, nc * sizeof(Draft));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        list.d[list.len].title = strdup(title);
                        list.d[list.len].body = strdup(body);
                        list.len++;
                    }
                }
            }
        } else if (strcmp(cmd, "REVISE") == 0) {
            char *title = next_token(&p);
            char *body = p;
            if (title && body) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].title, title) == 0) {
                        char *nb = strdup(body);
                        if (nb) { free(list.d[i].body); list.d[i].body = nb; }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DISCARD") == 0) {
            char *title = next_token(&p);
            if (title) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].title, title) == 0) {
                        if (trash.len == trash.cap) {
                            size_t nc = trash.cap ? trash.cap * 2 : 4;
                            Draft *nd = realloc(trash.d, nc * sizeof(Draft));
                            if (nd) { trash.d = nd; trash.cap = nc; }
                        }
                        if (trash.len < trash.cap) {
                            trash.d[trash.len++] = list.d[i];
                        } else {
                            free(list.d[i].title); free(list.d[i].body);
                        }
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--; break;
                    }
                }
            }
        } else if (strcmp(cmd, "RESTORE") == 0) {
            if (trash.len > 0) {
                trash.len--;
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Draft *nd = realloc(list.d, nc * sizeof(Draft));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len++] = trash.d[trash.len];
                } else {
                    free(trash.d[trash.len].title); free(trash.d[trash.len].body);
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = next_token(&p);
            if (title) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].title, title) == 0) {
                        printf("%s ", list.d[i].title);
                        print_text(list.d[i].body);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s ", list.d[i].title);
                print_text(list.d[i].body);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) { free(list.d[i].title); free(list.d[i].body); }
    for (size_t i = 0; i < trash.len; i++) { free(trash.d[i].title); free(trash.d[i].body); }
    free(list.d); free(trash.d);
    return 0;
}