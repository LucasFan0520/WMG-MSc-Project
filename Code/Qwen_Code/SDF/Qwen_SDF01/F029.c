// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *key; char *val; } Rec;
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

int main(void) {
    RL list = {0};
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
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Rec *nd = realloc(list.d, nc * sizeof(Rec));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].key = strdup(key);
                    list.d[list.len].val = strdup(val);
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "DEDUP") == 0) {
            for (size_t i = 0; i < list.len; ) {
                int dup = 0;
                for (size_t j = 0; j < i; j++) {
                    if (strcmp(list.d[i].key, list.d[j].key) == 0) {
                        dup = 1;
                        break;
                    }
                }
                if (dup) {
                    free(list.d[i].key);
                    free(list.d[i].val);
                    for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                    list.len--;
                } else i++;
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *key = next_token(&p);
            if (key) {
                for (size_t i = 0; i < list.len; ) {
                    if (strcmp(list.d[i].key, key) == 0) {
                        free(list.d[i].key);
                        free(list.d[i].val);
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--;
                    } else i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *key = next_token(&p);
            if (key) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].key, key) == 0) {
                        printf("%s\n", list.d[i].val);
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s %s\n", list.d[i].key, list.d[i].val);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].key);
        free(list.d[i].val);
    }
    free(list.d);
    return 0;
}