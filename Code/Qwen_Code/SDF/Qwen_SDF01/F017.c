// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *key; char *val; } KV;
typedef struct { KV *d; size_t len, cap; } KL;

char *read_line(void) {
    char *l = NULL; size_t len = 0;
    if (getline(&l, &len, stdin) == -1) { free(l); return NULL; }
    size_t sl = strlen(l);
    if (sl > 0 && l[sl - 1] == '\n') l[sl - 1] = '\0';
    return l;
}

int main(int argc, char **argv) {
    KL list = {0};
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *line = NULL; size_t len = 0;
            while (getline(&line, &len, f) != -1) {
                size_t sl = strlen(line);
                if (sl > 0 && line[sl - 1] == '\n') line[sl - 1] = '\0';
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = '\0';
                    char *key = line;
                    char *val = eq + 1;
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        KV *nd = realloc(list.d, nc * sizeof(KV));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        list.d[list.len].key = strdup(key);
                        list.d[list.len].val = strdup(val);
                        list.len++;
                    }
                }
            }
            free(line);
            fclose(f);
        }
    }
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = strsep(&p, " ");
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "SET") == 0 && p) {
            char *key = strsep(&p, " ");
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
                        KV *nd = realloc(list.d, nc * sizeof(KV));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        list.d[list.len].key = strdup(key);
                        list.d[list.len].val = strdup(val);
                        list.len++;
                    }
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0 && p) {
            char *key = p;
            for (size_t i = 0; i < list.len; i++) {
                if (strcmp(list.d[i].key, key) == 0) {
                    free(list.d[i].key); free(list.d[i].val);
                    for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                    list.len--; break;
                }
            }
        } else if (strcmp(cmd, "GET") == 0 && p) {
            char *key = p;
            for (size_t i = 0; i < list.len; i++) {
                if (strcmp(list.d[i].key, key) == 0) {
                    printf("%s\n", list.d[i].val);
                    break;
                }
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
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