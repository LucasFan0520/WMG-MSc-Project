// F021.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *name; char *note; } Ing;
typedef struct { Ing *d; size_t len, cap; } IL;
typedef struct { char *name; IL ings; } Rec;
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

int find_rec(RL *l, const char *name) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].name, name) == 0) return i;
    return -1;
}

int find_ing(IL *l, const char *name) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].name, name) == 0) return i;
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
        
        if (strcmp(cmd, "RECIPE") == 0) {
            char *name = next_token(&p);
            if (name && find_rec(&list, name) == -1) {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Rec *nd = realloc(list.d, nc * sizeof(Rec));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].name = strdup(name);
                    list.d[list.len].ings.d = NULL;
                    list.d[list.len].ings.len = 0;
                    list.d[list.len].ings.cap = 0;
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "ING") == 0) {
            char *rname = next_token(&p);
            char *iname = next_token(&p);
            char *note = p;
            if (rname && iname && note) {
                int ri = find_rec(&list, rname);
                if (ri != -1) {
                    IL *il = &list.d[ri].ings;
                    if (il->len == il->cap) {
                        size_t nc = il->cap ? il->cap * 2 : 4;
                        Ing *nd = realloc(il->d, nc * sizeof(Ing));
                        if (nd) { il->d = nd; il->cap = nc; }
                    }
                    if (il->len < il->cap) {
                        il->d[il->len].name = strdup(iname);
                        il->d[il->len].note = strdup(note);
                        il->len++;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVEING") == 0) {
            char *rname = next_token(&p);
            char *iname = p;
            if (rname && iname) {
                int ri = find_rec(&list, rname);
                if (ri != -1) {
                    IL *il = &list.d[ri].ings;
                    int ii = find_ing(il, iname);
                    if (ii != -1) {
                        free(il->d[ii].name);
                        free(il->d[ii].note);
                        for (size_t i = ii; i < il->len - 1; i++) il->d[i] = il->d[i + 1];
                        il->len--;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETERECIPE") == 0) {
            char *name = next_token(&p);
            if (name) {
                int ri = find_rec(&list, name);
                if (ri != -1) {
                    free(list.d[ri].name);
                    for (size_t i = 0; i < list.d[ri].ings.len; i++) {
                        free(list.d[ri].ings.d[i].name);
                        free(list.d[ri].ings.d[i].note);
                    }
                    free(list.d[ri].ings.d);
                    for (size_t i = ri; i < list.len - 1; i++) list.d[i] = list.d[i + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0) {
            char *name = next_token(&p);
            if (name) {
                int ri = find_rec(&list, name);
                if (ri != -1) {
                    for (size_t i = 0; i < list.d[ri].ings.len; i++) {
                        printf("%s ", list.d[ri].ings.d[i].name);
                        print_text(list.d[ri].ings.d[i].note);
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.len; i++) printf("%s\n", list.d[i].name);
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].name);
        for (size_t j = 0; j < list.d[i].ings.len; j++) {
            free(list.d[i].ings.d[j].name);
            free(list.d[i].ings.d[j].note);
        }
        free(list.d[i].ings.d);
    }
    free(list.d);
    return 0;
}