// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *id; char *name; char *dept; char *title; } Emp;
typedef struct { Emp *d; size_t len, cap; } EL;

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

int find_emp(EL *l, const char *id) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].id, id) == 0) return i;
    return -1;
}

int main(void) {
    EL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "ADD") == 0) {
            char *id = next_token(&p);
            char *name = next_token(&p);
            char *dept = next_token(&p);
            char *title = p;
            if (id && name && dept && title) {
                if (find_emp(&list, id) == -1) {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        Emp *nd = realloc(list.d, nc * sizeof(Emp));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        list.d[list.len].id = strdup(id);
                        list.d[list.len].name = strdup(name);
                        list.d[list.len].dept = strdup(dept);
                        list.d[list.len].title = strdup(title);
                        list.len++;
                    }
                }
            }
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char *id = next_token(&p);
            char *name = next_token(&p);
            char *dept = next_token(&p);
            char *title = p;
            if (id && name && dept && title) {
                int idx = find_emp(&list, id);
                if (idx != -1) {
                    char *n = strdup(name);
                    char *d = strdup(dept);
                    char *t = strdup(title);
                    if (n && d && t) {
                        free(list.d[idx].name);
                        free(list.d[idx].dept);
                        free(list.d[idx].title);
                        list.d[idx].name = n;
                        list.d[idx].dept = d;
                        list.d[idx].title = t;
                    } else {
                        free(n);
                        free(d);
                        free(t);
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *id = next_token(&p);
            if (id) {
                int idx = find_emp(&list, id);
                if (idx != -1) {
                    free(list.d[idx].id);
                    free(list.d[idx].name);
                    free(list.d[idx].dept);
                    free(list.d[idx].title);
                    for (size_t i = idx; i < list.len - 1; i++) list.d[i] = list.d[i + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *id = next_token(&p);
            if (id) {
                int idx = find_emp(&list, id);
                if (idx != -1) {
                    printf("%s %s %s %s\n", list.d[idx].id, list.d[idx].name, list.d[idx].dept, list.d[idx].title);
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s %s %s %s\n", list.d[i].id, list.d[i].name, list.d[i].dept, list.d[i].title);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].id);
        free(list.d[i].name);
        free(list.d[i].dept);
        free(list.d[i].title);
    }
    free(list.d);
    return 0;
}