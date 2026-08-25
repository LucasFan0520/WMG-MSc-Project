// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *val; } Val;
typedef struct { Val *d; size_t len, cap; } VL;
typedef struct { char *key; VL vals; } KV;
typedef struct { KV *d; size_t len, cap; } KL;

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

int find_key(KL *l, const char *k) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].key, k) == 0) return i;
    return -1;
}

int main(void) {
    KL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "ADD") == 0) {
            char *key = next_token(&p);
            char *val = p;
            if (key && val) {
                int idx = find_key(&list, key);
                if (idx == -1) {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        KV *nd = realloc(list.d, nc * sizeof(KV));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        list.d[list.len].key = strdup(key);
                        list.d[list.len].vals.d = NULL; list.d[list.len].vals.len = 0; list.d[list.len].vals.cap = 0;
                        idx = list.len++;
                    } else { free(line); continue; }
                }
                VL *vl = &list.d[idx].vals;
                if (vl->len == vl->cap) {
                    size_t nc = vl->cap ? vl->cap * 2 : 4;
                    Val *nd = realloc(vl->d, nc * sizeof(Val));
                    if (nd) { vl->d = nd; vl->cap = nc; }
                }
                if (vl->len < vl->cap) vl->d[vl->len++].val = strdup(val);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *key = next_token(&p);
            char *val = p;
            if (key && val) {
                int idx = find_key(&list, key);
                if (idx != -1) {
                    VL *vl = &list.d[idx].vals;
                    for (size_t i = 0; i < vl->len; ) {
                        if (strcmp(vl->d[i].val, val) == 0) {
                            free(vl->d[i].val);
                            for (size_t j = i; j < vl->len - 1; j++) vl->d[j] = vl->d[j + 1];
                            vl->len--;
                        } else i++;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *key = next_token(&p);
            if (key) {
                int idx = find_key(&list, key);
                if (idx != -1) {
                    free(list.d[idx].key);
                    for (size_t i = 0; i < list.d[idx].vals.len; i++) free(list.d[idx].vals.d[i].val);
                    free(list.d[idx].vals.d);
                    for (size_t j = idx; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key = next_token(&p);
            if (key) {
                int idx = find_key(&list, key);
                if (idx != -1) {
                    for (size_t i = 0; i < list.d[idx].vals.len; i++) printf("%s\n", list.d[idx].vals.d[i].val);
                }
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (size_t i = 0; i < list.len; i++) printf("%s\n", list.d[i].key);
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].key);
        for (size_t j = 0; j < list.d[i].vals.len; j++) free(list.d[i].vals.d[j].val);
        free(list.d[i].vals.d);
    }
    free(list.d);
    return 0;
}