// F030.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *old; char *new; } Rule;
typedef struct { Rule *d; size_t len, cap; } RL;

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

int main(void) {
    RL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "ADD") == 0) {
            char *old = next_token(&p);
            char *new = p;
            if (old && new) {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Rule *nd = realloc(list.d, nc * sizeof(Rule));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].old = strdup(old);
                    list.d[list.len].new = strdup(new);
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "APPLY") == 0) {
            char *text = p;
            if (text) {
                for (size_t i = 0; i < list.len; i++) {
                    char *pos = strstr(text, list.d[i].old);
                    if (pos) {
                        size_t pre = pos - text;
                        size_t olen = strlen(list.d[i].old);
                        size_t nlen = strlen(list.d[i].new);
                        size_t tlen = strlen(text);
                        size_t ntot = pre + nlen + (tlen - pre - olen);
                        char *res = malloc(ntot + 1);
                        if (res) {
                            memcpy(res, text, pre);
                            memcpy(res + pre, list.d[i].new, nlen);
                            memcpy(res + pre + nlen, pos + olen, tlen - pre - olen + 1);
                            printf("%s\n", res);
                            free(res);
                        }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *old = next_token(&p);
            if (old) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].old, old) == 0) {
                        free(list.d[i].old);
                        free(list.d[i].new);
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "SWAP") == 0) {
            char *i_s = next_token(&p);
            char *j_s = p;
            if (i_s && j_s) {
                long i = strtol(i_s, NULL, 10);
                long j = strtol(j_s, NULL, 10);
                if (i >= 0 && (size_t)i < list.len && j >= 0 && (size_t)j < list.len) {
                    Rule tmp = list.d[i];
                    list.d[i] = list.d[j];
                    list.d[j] = tmp;
                }
            }
        } else if (strcmp(cmd, "RULES") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s %s\n", list.d[i].old, list.d[i].new);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].old);
        free(list.d[i].new);
    }
    free(list.d);
    return 0;
}