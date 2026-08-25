// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *name; char *note; } Rec;
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

int main(void) {
    RL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "BATCH") == 0) {
            char *n_s = next_token(&p);
            if (n_s) {
                long n = strtol(n_s, NULL, 10);
                if (n > 0) {
                    RL tmp = {0};
                    tmp.d = malloc(n * sizeof(Rec));
                    if (tmp.d) {
                        tmp.cap = n;
                        int valid = 1;
                        for (long i = 0; i < n; i++) {
                            char *bline = read_line();
                            if (!bline) {
                                valid = 0;
                                break;
                            }
                            char *bp = bline;
                            char *name = next_token(&bp);
                            char *note = bp;
                            if (name && note) {
                                tmp.d[tmp.len].name = strdup(name);
                                tmp.d[tmp.len].note = strdup(note);
                                tmp.len++;
                            } else {
                                valid = 0;
                                free(bline);
                                break;
                            }
                            free(bline);
                        }
                        if (valid) {
                            for (size_t i = 0; i < tmp.len; i++) {
                                if (list.len == list.cap) {
                                    size_t nc = list.cap ? list.cap * 2 : 4;
                                    Rec *nd = realloc(list.d, nc * sizeof(Rec));
                                    if (nd) { list.d = nd; list.cap = nc; }
                                }
                                if (list.len < list.cap) {
                                    list.d[list.len++] = tmp.d[i];
                                } else {
                                    free(tmp.d[i].name);
                                    free(tmp.d[i].note);
                                }
                            }
                        } else {
                            for (size_t i = 0; i < tmp.len; i++) {
                                free(tmp.d[i].name);
                                free(tmp.d[i].note);
                            }
                        }
                        free(tmp.d);
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = next_token(&p);
            if (name) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].name, name) == 0) {
                        free(list.d[i].name);
                        free(list.d[i].note);
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--;
                        break;
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
        free(list.d[i].name);
        free(list.d[i].note);
    }
    free(list.d);
    return 0;
}