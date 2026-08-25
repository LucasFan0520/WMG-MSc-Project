// F035.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *name; char *payload; } Job;
typedef struct { Job *d; size_t len, cap; } JL;

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

int find_job(JL *l, const char *name) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].name, name) == 0) return i;
    return -1;
}

int main(void) {
    JL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "ENQUEUE") == 0) {
            char *name = next_token(&p);
            char *payload = p;
            if (name && payload) {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Job *nd = realloc(list.d, nc * sizeof(Job));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].name = strdup(name);
                    list.d[list.len].payload = strdup(payload);
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "CLONE") == 0) {
            char *old = next_token(&p);
            char *new = p;
            if (old && new) {
                int idx = find_job(&list, old);
                if (idx != -1 && find_job(&list, new) == -1) {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        Job *nd = realloc(list.d, nc * sizeof(Job));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        list.d[list.len].name = strdup(new);
                        list.d[list.len].payload = strdup(list.d[idx].payload);
                        list.len++;
                    }
                }
            }
        } else if (strcmp(cmd, "CANCEL") == 0) {
            char *name = next_token(&p);
            if (name) {
                int idx = find_job(&list, name);
                if (idx != -1) {
                    free(list.d[idx].name);
                    free(list.d[idx].payload);
                    for (size_t i = idx; i < list.len - 1; i++) list.d[i] = list.d[i + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "RUN") == 0) {
            char *name = next_token(&p);
            if (name) {
                int idx = find_job(&list, name);
                if (idx != -1) {
                    printf("%s ", list.d[idx].name);
                    print_text(list.d[idx].payload);
                    free(list.d[idx].name);
                    free(list.d[idx].payload);
                    for (size_t i = idx; i < list.len - 1; i++) list.d[i] = list.d[i + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s ", list.d[i].name);
                print_text(list.d[i].payload);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].name);
        free(list.d[i].payload);
    }
    free(list.d);
    return 0;
}