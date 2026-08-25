// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *key; char *val; } Var;
typedef struct { Var *d; size_t len, cap; } VL;

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

int main(void) {
    VL list = {0};
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
                int idx = -1;
                for (size_t i = 0; i < list.len; i++) if (strcmp(list.d[i].key, key) == 0) { idx = i; break; }
                if (idx != -1) {
                    char *nv = strdup(val);
                    if (nv) { free(list.d[idx].val); list.d[idx].val = nv; }
                } else {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        Var *nd = realloc(list.d, nc * sizeof(Var));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        list.d[list.len].key = strdup(key);
                        list.d[list.len].val = strdup(val);
                        list.len++;
                    }
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = next_token(&p);
            if (key) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].key, key) == 0) {
                        free(list.d[i].key); free(list.d[i].val);
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--; break;
                    }
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text = p;
            if (text) {
                size_t tlen = strlen(text);
                char *out = malloc(tlen * 2 + 1); 
                size_t olen = 0, ocap = tlen * 2 + 1;
                if (!out) { free(line); continue; }
                
                for (size_t i = 0; i < tlen; ) {
                    if (text[i] == '{') {
                        size_t j = i + 1;
                        while (j < tlen && text[j] != '}') j++;
                        if (j < tlen) {
                            char kbuf[256];
                            size_t klen = j - i - 1;
                            if (klen < sizeof(kbuf)) {
                                memcpy(kbuf, text + i + 1, klen);
                                kbuf[klen] = '\0';
                                int found = 0;
                                for (size_t k = 0; k < list.len; k++) {
                                    if (strcmp(list.d[k].key, kbuf) == 0) {
                                        size_t vlen = strlen(list.d[k].val);
                                        while (olen + vlen + 1 > ocap) {
                                            ocap *= 2;
                                            char *no = realloc(out, ocap);
                                            if (!no) { free(out); out = NULL; break; }
                                            out = no;
                                        }
                                        if (!out) break;
                                        memcpy(out + olen, list.d[k].val, vlen);
                                        olen += vlen;
                                        found = 1;
                                        break;
                                    }
                                }
                                if (!found) {
                                    while (olen + klen + 2 + 1 > ocap) {
                                        ocap *= 2;
                                        char *no = realloc(out, ocap);
                                        if (!no) { free(out); out = NULL; break; }
                                        out = no;
                                    }
                                    if (!out) break;
                                    memcpy(out + olen, text + i, klen + 2);
                                    olen += klen + 2;
                                }
                                i = j + 1;
                                continue;
                            }
                        }
                    }
                    while (olen + 1 + 1 > ocap) {
                        ocap *= 2;
                        char *no = realloc(out, ocap);
                        if (!no) { free(out); out = NULL; break; }
                        out = no;
                    }
                    if (!out) break;
                    out[olen++] = text[i++];
                }
                if (out) {
                    out[olen] = '\0';
                    printf("%s\n", out);
                    free(out);
                }
            }
        } else if (strcmp(cmd, "VARS") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s=%s\n", list.d[i].key, list.d[i].val);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) { free(list.d[i].key); free(list.d[i].val); }
    free(list.d);
    return 0;
}