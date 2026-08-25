// F014.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *label; } Lbl;
typedef struct { Lbl *d; size_t len, cap; } LL;
typedef struct { char *name; char *note; LL labels; } Item;
typedef struct { Item *d; size_t len, cap; } IL;

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
    IL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "ITEM") == 0) {
            char *name = next_token(&p);
            char *note = p;
            if (name && note) {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Item *nd = realloc(list.d, nc * sizeof(Item));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].name = strdup(name);
                    list.d[list.len].note = strdup(note);
                    list.d[list.len].labels.d = NULL; list.d[list.len].labels.len = 0; list.d[list.len].labels.cap = 0;
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "LABEL") == 0) {
            char *name = next_token(&p);
            char *label = p;
            if (name && label) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].name, name) == 0) {
                        LL *ll = &list.d[i].labels;
                        if (ll->len == ll->cap) {
                            size_t nc = ll->cap ? ll->cap * 2 : 4;
                            Lbl *nd = realloc(ll->d, nc * sizeof(Lbl));
                            if (nd) { ll->d = nd; ll->cap = nc; }
                        }
                        if (ll->len < ll->cap) ll->d[ll->len++].label = strdup(label);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "UNLABEL") == 0) {
            char *name = next_token(&p);
            char *label = p;
            if (name && label) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].name, name) == 0) {
                        LL *ll = &list.d[i].labels;
                        for (size_t j = 0; j < ll->len; ) {
                            if (strcmp(ll->d[j].label, label) == 0) {
                                free(ll->d[j].label);
                                for (size_t k = j; k < ll->len - 1; k++) ll->d[k] = ll->d[k + 1];
                                ll->len--;
                            } else j++;
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = next_token(&p);
            if (name) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].name, name) == 0) {
                        free(list.d[i].name); free(list.d[i].note);
                        for (size_t j = 0; j < list.d[i].labels.len; j++) free(list.d[i].labels.d[j].label);
                        free(list.d[i].labels.d);
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
                        printf("%s ", list.d[i].name);
                        print_text(list.d[i].note);
                        for (size_t j = 0; j < list.d[i].labels.len; j++) printf("  %s\n", list.d[i].labels.d[j].label);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s ", list.d[i].name);
                print_text(list.d[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].name); free(list.d[i].note);
        for (size_t j = 0; j < list.d[i].labels.len; j++) free(list.d[i].labels.d[j].label);
        free(list.d[i].labels.d);
    }
    free(list.d);
    return 0;
}