// F024.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *name; } Auth;
typedef struct { Auth *d; size_t len, cap; } AL;
typedef struct { char *title; long year; AL authors; } Book;
typedef struct { Book *d; size_t len, cap; } BL;

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

int find_book(BL *l, const char *title) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].title, title) == 0) return i;
    return -1;
}

int find_auth(AL *l, const char *name) {
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
        
        if (strcmp(cmd, "BOOK") == 0) {
            char *title = next_token(&p);
            char *year_s = next_token(&p);
            if (title && year_s) {
                long year = strtol(year_s, NULL, 10);
                if (find_book(&list, title) == -1) {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        Book *nd = realloc(list.d, nc * sizeof(Book));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        list.d[list.len].title = strdup(title);
                        list.d[list.len].year = year;
                        list.d[list.len].authors.d = NULL;
                        list.d[list.len].authors.len = 0;
                        list.d[list.len].authors.cap = 0;
                        list.len++;
                    }
                }
            }
        } else if (strcmp(cmd, "AUTHOR") == 0) {
            char *title = next_token(&p);
            char *name = p;
            if (title && name) {
                int bi = find_book(&list, title);
                if (bi != -1) {
                    AL *al = &list.d[bi].authors;
                    if (find_auth(al, name) == -1) {
                        if (al->len == al->cap) {
                            size_t nc = al->cap ? al->cap * 2 : 4;
                            Auth *nd = realloc(al->d, nc * sizeof(Auth));
                            if (nd) { al->d = nd; al->cap = nc; }
                        }
                        if (al->len < al->cap) {
                            al->d[al->len++].name = strdup(name);
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEAUTHOR") == 0) {
            char *title = next_token(&p);
            char *name = p;
            if (title && name) {
                int bi = find_book(&list, title);
                if (bi != -1) {
                    AL *al = &list.d[bi].authors;
                    int ai = find_auth(al, name);
                    if (ai != -1) {
                        free(al->d[ai].name);
                        for (size_t i = ai; i < al->len - 1; i++) al->d[i] = al->d[i + 1];
                        al->len--;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *title = next_token(&p);
            if (title) {
                int bi = find_book(&list, title);
                if (bi != -1) {
                    free(list.d[bi].title);
                    for (size_t i = 0; i < list.d[bi].authors.len; i++) free(list.d[bi].authors.d[i].name);
                    free(list.d[bi].authors.d);
                    for (size_t i = bi; i < list.len - 1; i++) list.d[i] = list.d[i + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = next_token(&p);
            if (title) {
                int bi = find_book(&list, title);
                if (bi != -1) {
                    for (size_t i = 0; i < list.d[bi].authors.len; i++) {
                        printf("%s\n", list.d[bi].authors.d[i].name);
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s %ld\n", list.d[i].title, list.d[i].year);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].title);
        for (size_t j = 0; j < list.d[i].authors.len; j++) free(list.d[i].authors.d[j].name);
        free(list.d[i].authors.d);
    }
    free(list.d);
    return 0;
}