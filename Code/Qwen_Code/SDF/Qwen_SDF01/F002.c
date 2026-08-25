// F002.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *tag; char *msg; } Rec;
typedef struct { Rec *d; size_t len, cap; } RL;

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

int find_tag(RL *l, const char *tag) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].tag, tag) == 0) return i;
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
            char *tag = next_token(&p);
            char *msg = p;
            if (tag && msg) {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Rec *nd = realloc(list.d, nc * sizeof(Rec));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].tag = strdup(tag);
                    list.d[list.len].msg = strdup(msg);
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "CONCAT") == 0) {
            char *t1 = next_token(&p);
            char *t2 = next_token(&p);
            char *nt = p;
            if (t1 && t2 && nt) {
                int i1 = find_tag(&list, t1);
                int i2 = find_tag(&list, t2);
                if (i1 != -1 && i2 != -1 && find_tag(&list, nt) == -1) {
                    size_t l1 = strlen(list.d[i1].msg);
                    size_t l2 = strlen(list.d[i2].msg);
                    if (l1 + l2 + 2 > l1) {
                        char *nm = malloc(l1 + l2 + 2);
                        if (nm) {
                            memcpy(nm, list.d[i1].msg, l1);
                            nm[l1] = ' ';
                            memcpy(nm + l1 + 1, list.d[i2].msg, l2 + 1);
                            if (list.len == list.cap) {
                                size_t nc = list.cap ? list.cap * 2 : 4;
                                Rec *nd = realloc(list.d, nc * sizeof(Rec));
                                if (nd) { list.d = nd; list.cap = nc; }
                            }
                            if (list.len < list.cap) {
                                list.d[list.len].tag = strdup(nt);
                                list.d[list.len].msg = nm;
                                list.len++;
                            } else {
                                free(nm);
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *tag = next_token(&p);
            if (tag) {
                for (size_t i = 0; i < list.len; ) {
                    if (strcmp(list.d[i].tag, tag) == 0) {
                        free(list.d[i].tag); free(list.d[i].msg);
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--;
                    } else {
                        i++;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *tag = next_token(&p);
            if (tag) {
                int idx = find_tag(&list, tag);
                if (idx != -1) print_text(list.d[idx].msg);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s ", list.d[i].tag);
                print_text(list.d[i].msg);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) { free(list.d[i].tag); free(list.d[i].msg); }
    free(list.d);
    return 0;
}