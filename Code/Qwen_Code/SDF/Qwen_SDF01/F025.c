// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *name; char *note; } Stu;
typedef struct { Stu *d; size_t len, cap; } SL;
typedef struct { char *name; SL students; } Cls;
typedef struct { Cls *d; size_t len, cap; } CL;

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

int find_cls(CL *l, const char *name) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].name, name) == 0) return i;
    return -1;
}

int find_stu(SL *l, const char *name) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].name, name) == 0) return i;
    return -1;
}

int main(void) {
    CL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "CLASS") == 0) {
            char *name = next_token(&p);
            if (name && find_cls(&list, name) == -1) {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Cls *nd = realloc(list.d, nc * sizeof(Cls));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].name = strdup(name);
                    list.d[list.len].students.d = NULL;
                    list.d[list.len].students.len = 0;
                    list.d[list.len].students.cap = 0;
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "STUDENT") == 0) {
            char *cname = next_token(&p);
            char *sname = next_token(&p);
            char *note = p;
            if (cname && sname && note) {
                int ci = find_cls(&list, cname);
                if (ci != -1) {
                    SL *sl = &list.d[ci].students;
                    if (sl->len == sl->cap) {
                        size_t nc = sl->cap ? sl->cap * 2 : 4;
                        Stu *nd = realloc(sl->d, nc * sizeof(Stu));
                        if (nd) { sl->d = nd; sl->cap = nc; }
                    }
                    if (sl->len < sl->cap) {
                        sl->d[sl->len].name = strdup(sname);
                        sl->d[sl->len].note = strdup(note);
                        sl->len++;
                    }
                }
            }
        } else if (strcmp(cmd, "TRANSFER") == 0) {
            char *from = next_token(&p);
            char *to = next_token(&p);
            char *sname = p;
            if (from && to && sname) {
                int fi = find_cls(&list, from);
                int ti = find_cls(&list, to);
                if (fi != -1 && ti != -1) {
                    SL *fsl = &list.d[fi].students;
                    SL *tsl = &list.d[ti].students;
                    int fsi = find_stu(fsl, sname);
                    if (fsi != -1) {
                        if (tsl->len == tsl->cap) {
                            size_t nc = tsl->cap ? tsl->cap * 2 : 4;
                            Stu *nd = realloc(tsl->d, nc * sizeof(Stu));
                            if (nd) { tsl->d = nd; tsl->cap = nc; }
                        }
                        if (tsl->len < tsl->cap) {
                            tsl->d[tsl->len++] = fsl->d[fsi];
                            for (size_t i = fsi; i < fsl->len - 1; i++) fsl->d[i] = fsl->d[i + 1];
                            fsl->len--;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *cname = next_token(&p);
            char *sname = p;
            if (cname && sname) {
                int ci = find_cls(&list, cname);
                if (ci != -1) {
                    SL *sl = &list.d[ci].students;
                    int si = find_stu(sl, sname);
                    if (si != -1) {
                        free(sl->d[si].name);
                        free(sl->d[si].note);
                        for (size_t i = si; i < sl->len - 1; i++) sl->d[i] = sl->d[i + 1];
                        sl->len--;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETECLASS") == 0) {
            char *cname = next_token(&p);
            if (cname) {
                int ci = find_cls(&list, cname);
                if (ci != -1) {
                    free(list.d[ci].name);
                    for (size_t i = 0; i < list.d[ci].students.len; i++) {
                        free(list.d[ci].students.d[i].name);
                        free(list.d[ci].students.d[i].note);
                    }
                    free(list.d[ci].students.d);
                    for (size_t i = ci; i < list.len - 1; i++) list.d[i] = list.d[i + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s\n", list.d[i].name);
                for (size_t j = 0; j < list.d[i].students.len; j++) {
                    printf("  %s ", list.d[i].students.d[j].name);
                    print_text(list.d[i].students.d[j].note);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].name);
        for (size_t j = 0; j < list.d[i].students.len; j++) {
            free(list.d[i].students.d[j].name);
            free(list.d[i].students.d[j].note);
        }
        free(list.d[i].students.d);
    }
    free(list.d);
    return 0;
}