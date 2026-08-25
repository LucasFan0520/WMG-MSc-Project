// F009.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *desc; } Ev;
typedef struct { Ev *d; size_t len, cap; } EL;
typedef struct { char *group; EL events; } Grp;
typedef struct { Grp *d; size_t len, cap; } GL;

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

int find_grp(GL *l, const char *g) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].group, g) == 0) return i;
    return -1;
}

int main(void) {
    GL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "EVENT") == 0) {
            char *group = next_token(&p);
            char *desc = p;
            if (group && desc) {
                int idx = find_grp(&list, group);
                if (idx == -1) {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        Grp *nd = realloc(list.d, nc * sizeof(Grp));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        list.d[list.len].group = strdup(group);
                        list.d[list.len].events.d = NULL;
                        list.d[list.len].events.len = 0;
                        list.d[list.len].events.cap = 0;
                        idx = list.len++;
                    } else { free(line); continue; }
                }
                EL *ev = &list.d[idx].events;
                if (ev->len == ev->cap) {
                    size_t nc = ev->cap ? ev->cap * 2 : 4;
                    Ev *nd = realloc(ev->d, nc * sizeof(Ev));
                    if (nd) { ev->d = nd; ev->cap = nc; }
                }
                if (ev->len < ev->cap) {
                    ev->d[ev->len++].desc = strdup(desc);
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *gA = next_token(&p);
            char *gB = next_token(&p);
            char *nG = p;
            if (gA && gB && nG) {
                int iA = find_grp(&list, gA);
                int iB = find_grp(&list, gB);
                if (iA != -1 && iB != -1 && find_grp(&list, nG) == -1) {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        Grp *nd = realloc(list.d, nc * sizeof(Grp));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        Grp *ng = &list.d[list.len++];
                        ng->group = strdup(nG);
                        ng->events.d = NULL; ng->events.len = 0; ng->events.cap = 0;
                        EL *ev = &ng->events;
                        size_t tot = list.d[iA].events.len + list.d[iB].events.len;
                        if (tot > 0) {
                            ev->d = malloc(tot * sizeof(Ev));
                            if (ev->d) {
                                ev->cap = tot;
                                for (size_t i = 0; i < list.d[iA].events.len; i++) ev->d[ev->len++].desc = strdup(list.d[iA].events.d[i].desc);
                                for (size_t i = 0; i < list.d[iB].events.len; i++) ev->d[ev->len++].desc = strdup(list.d[iB].events.d[i].desc);
                            }
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETEGROUP") == 0) {
            char *group = next_token(&p);
            if (group) {
                int idx = find_grp(&list, group);
                if (idx != -1) {
                    free(list.d[idx].group);
                    for (size_t i = 0; i < list.d[idx].events.len; i++) free(list.d[idx].events.d[i].desc);
                    free(list.d[idx].events.d);
                    for (size_t j = idx; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *group = next_token(&p);
            if (group) {
                int idx = find_grp(&list, group);
                printf("%zu\n", idx != -1 ? list.d[idx].events.len : 0);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s %zu\n", list.d[i].group, list.d[i].events.len);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].group);
        for (size_t j = 0; j < list.d[i].events.len; j++) free(list.d[i].events.d[j].desc);
        free(list.d[i].events.d);
    }
    free(list.d);
    return 0;
}