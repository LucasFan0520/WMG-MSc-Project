// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *key; char *val; } Set;
typedef struct { Set *d; size_t len, cap; } SL;

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

int find_key(SL *l, const char *key) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].key, key) == 0) return i;
    return -1;
}

void set_val(SL *l, const char *key, const char *val) {
    int idx = find_key(l, key);
    if (idx != -1) {
        char *nv = strdup(val);
        if (nv) {
            free(l->d[idx].val);
            l->d[idx].val = nv;
        }
    } else {
        if (l->len == l->cap) {
            size_t nc = l->cap ? l->cap * 2 : 4;
            Set *nd = realloc(l->d, nc * sizeof(Set));
            if (nd) { l->d = nd; l->cap = nc; }
        }
        if (l->len < l->cap) {
            l->d[l->len].key = strdup(key);
            l->d[l->len].val = strdup(val);
            l->len++;
        }
    }
}

int main(void) {
    SL committed = {0};
    SL staged = {0};
    int in_trans = 0;
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "SET") == 0) {
            char *key = next_token(&p);
            char *val = p;
            if (key && val) {
                if (in_trans) {
                    set_val(&staged, key, val);
                } else {
                    set_val(&committed, key, val);
                }
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            for (size_t i = 0; i < staged.len; i++) {
                free(staged.d[i].key);
                free(staged.d[i].val);
            }
            staged.len = 0;
            in_trans = 1;
        } else if (strcmp(cmd, "TSET") == 0) {
            char *key = next_token(&p);
            char *val = p;
            if (key && val && in_trans) {
                set_val(&staged, key, val);
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_trans) {
                for (size_t i = 0; i < staged.len; i++) {
                    set_val(&committed, staged.d[i].key, staged.d[i].val);
                }
                for (size_t i = 0; i < staged.len; i++) {
                    free(staged.d[i].key);
                    free(staged.d[i].val);
                }
                staged.len = 0;
                in_trans = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_trans) {
                for (size_t i = 0; i < staged.len; i++) {
                    free(staged.d[i].key);
                    free(staged.d[i].val);
                }
                staged.len = 0;
                in_trans = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = next_token(&p);
            if (key) {
                int idx = find_key(&committed, key);
                if (idx != -1) printf("%s\n", committed.d[idx].val);
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < committed.len; i++) {
                printf("%s=%s\n", committed.d[i].key, committed.d[i].val);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < committed.len; i++) {
        free(committed.d[i].key);
        free(committed.d[i].val);
    }
    for (size_t i = 0; i < staged.len; i++) {
        free(staged.d[i].key);
        free(staged.d[i].val);
    }
    free(committed.d);
    free(staged.d);
    return 0;
}