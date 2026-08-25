// F006.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *item; long count; } Inv;
typedef struct { Inv *d; size_t len, cap; } IL;

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
    IL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "ADD") == 0) {
            char *item = next_token(&p);
            char *cnt_s = next_token(&p);
            if (item && cnt_s) {
                long cnt = strtol(cnt_s, NULL, 10);
                int idx = -1;
                for (size_t i = 0; i < list.len; i++) if (strcmp(list.d[i].item, item) == 0) { idx = i; break; }
                if (idx != -1) {
                    list.d[idx].count += cnt;
                } else {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        Inv *nd = realloc(list.d, nc * sizeof(Inv));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        list.d[list.len].item = strdup(item);
                        list.d[list.len].count = cnt;
                        list.len++;
                    }
                }
            }
        } else if (strcmp(cmd, "TAKE") == 0) {
            char *item = next_token(&p);
            char *cnt_s = next_token(&p);
            if (item && cnt_s) {
                long cnt = strtol(cnt_s, NULL, 10);
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].item, item) == 0) {
                        list.d[i].count -= cnt;
                        if (list.d[i].count < 0) list.d[i].count = 0;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *item = next_token(&p);
            if (item) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].item, item) == 0) {
                        free(list.d[i].item);
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--; break;
                    }
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0) {
            char *item = next_token(&p);
            if (item) {
                long c = 0;
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].item, item) == 0) { c = list.d[i].count; break; }
                }
                printf("%ld\n", c);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.len; i++) printf("%s %ld\n", list.d[i].item, list.d[i].count);
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) free(list.d[i].item);
    free(list.d);
    return 0;
}