// F022.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *title; char *note; int done; } Task;
typedef struct { Task *d; size_t len, cap; } TL;
typedef struct { char *name; TL tasks; } Proj;
typedef struct { Proj *d; size_t len, cap; } PL;

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

int find_proj(PL *l, const char *name) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].name, name) == 0) return i;
    return -1;
}

int find_task(TL *l, const char *title) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].title, title) == 0) return i;
    return -1;
}

int main(void) {
    PL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "PROJECT") == 0) {
            char *name = next_token(&p);
            if (name && find_proj(&list, name) == -1) {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Proj *nd = realloc(list.d, nc * sizeof(Proj));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].name = strdup(name);
                    list.d[list.len].tasks.d = NULL;
                    list.d[list.len].tasks.len = 0;
                    list.d[list.len].tasks.cap = 0;
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "TASK") == 0) {
            char *pname = next_token(&p);
            char *title = next_token(&p);
            char *note = p;
            if (pname && title && note) {
                int pi = find_proj(&list, pname);
                if (pi != -1) {
                    TL *tl = &list.d[pi].tasks;
                    if (tl->len == tl->cap) {
                        size_t nc = tl->cap ? tl->cap * 2 : 4;
                        Task *nd = realloc(tl->d, nc * sizeof(Task));
                        if (nd) { tl->d = nd; tl->cap = nc; }
                    }
                    if (tl->len < tl->cap) {
                        tl->d[tl->len].title = strdup(title);
                        tl->d[tl->len].note = strdup(note);
                        tl->d[tl->len].done = 0;
                        tl->len++;
                    }
                }
            }
        } else if (strcmp(cmd, "MOVETASK") == 0) {
            char *from = next_token(&p);
            char *to = next_token(&p);
            char *title = p;
            if (from && to && title) {
                int fi = find_proj(&list, from);
                int ti = find_proj(&list, to);
                if (fi != -1 && ti != -1) {
                    TL *ftl = &list.d[fi].tasks;
                    TL *ttl = &list.d[ti].tasks;
                    int fti = find_task(ftl, title);
                    if (fti != -1) {
                        if (ttl->len == ttl->cap) {
                            size_t nc = ttl->cap ? ttl->cap * 2 : 4;
                            Task *nd = realloc(ttl->d, nc * sizeof(Task));
                            if (nd) { ttl->d = nd; ttl->cap = nc; }
                        }
                        if (ttl->len < ttl->cap) {
                            ttl->d[ttl->len++] = ftl->d[fti];
                            for (size_t i = fti; i < ftl->len - 1; i++) ftl->d[i] = ftl->d[i + 1];
                            ftl->len--;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DONETASK") == 0) {
            char *pname = next_token(&p);
            char *title = p;
            if (pname && title) {
                int pi = find_proj(&list, pname);
                if (pi != -1) {
                    int ti = find_task(&list.d[pi].tasks, title);
                    if (ti != -1) list.d[pi].tasks.d[ti].done = 1;
                }
            }
        } else if (strcmp(cmd, "DELETEPROJECT") == 0) {
            char *name = next_token(&p);
            if (name) {
                int pi = find_proj(&list, name);
                if (pi != -1) {
                    free(list.d[pi].name);
                    for (size_t i = 0; i < list.d[pi].tasks.len; i++) {
                        free(list.d[pi].tasks.d[i].title);
                        free(list.d[pi].tasks.d[i].note);
                    }
                    free(list.d[pi].tasks.d);
                    for (size_t i = pi; i < list.len - 1; i++) list.d[i] = list.d[i + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s\n", list.d[i].name);
                for (size_t j = 0; j < list.d[i].tasks.len; j++) {
                    printf("  %s ", list.d[i].tasks.d[j].title);
                    print_text(list.d[i].tasks.d[j].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].name);
        for (size_t j = 0; j < list.d[i].tasks.len; j++) {
            free(list.d[i].tasks.d[j].title);
            free(list.d[i].tasks.d[j].note);
        }
        free(list.d[i].tasks.d);
    }
    free(list.d);
    return 0;
}