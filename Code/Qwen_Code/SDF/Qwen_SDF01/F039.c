// F039.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *name; long rows, cols; char ***cells; } Tab;
typedef struct { Tab *d; size_t len, cap; } TL;

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

int find_tab(TL *l, const char *name) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].name, name) == 0) return i;
    return -1;
}

int main(void) {
    TL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "TABLE") == 0) {
            char *name = next_token(&p);
            char *r_s = next_token(&p);
            char *c_s = p;
            if (name && r_s && c_s) {
                long r = strtol(r_s, NULL, 10);
                long c = strtol(c_s, NULL, 10);
                if (r > 0 && c > 0 && find_tab(&list, name) == -1) {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        Tab *nd = realloc(list.d, nc * sizeof(Tab));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        list.d[list.len].name = strdup(name);
                        list.d[list.len].rows = r;
                        list.d[list.len].cols = c;
                        list.d[list.len].cells = malloc(r * sizeof(char**));
                        if (list.d[list.len].cells) {
                            for (long i = 0; i < r; i++) {
                                list.d[list.len].cells[i] = calloc(c, sizeof(char*));
                            }
                        }
                        list.len++;
                    }
                }
            }
        } else if (strcmp(cmd, "SET") == 0) {
            char *name = next_token(&p);
            char *r_s = next_token(&p);
            char *c_s = next_token(&p);
            char *val = p;
            if (name && r_s && c_s && val) {
                int idx = find_tab(&list, name);
                if (idx != -1) {
                    long r = strtol(r_s, NULL, 10);
                    long c = strtol(c_s, NULL, 10);
                    if (r >= 0 && r < list.d[idx].rows && c >= 0 && c < list.d[idx].cols) {
                        char *nv = strdup(val);
                        if (nv) {
                            free(list.d[idx].cells[r][c]);
                            list.d[idx].cells[r][c] = nv;
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *name = next_token(&p);
            char *r_s = next_token(&p);
            char *c_s = p;
            if (name && r_s && c_s) {
                int idx = find_tab(&list, name);
                if (idx != -1) {
                    long r = strtol(r_s, NULL, 10);
                    long c = strtol(c_s, NULL, 10);
                    if (r >= 0 && r < list.d[idx].rows && c >= 0 && c < list.d[idx].cols) {
                        if (list.d[idx].cells[r][c]) {
                            printf("%s\n", list.d[idx].cells[r][c]);
                        } else {
                            printf("EMPTY\n");
                        }
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = next_token(&p);
            if (name) {
                int idx = find_tab(&list, name);
                if (idx != -1) {
                    free(list.d[idx].name);
                    for (long i = 0; i < list.d[idx].rows; i++) {
                        for (long j = 0; j < list.d[idx].cols; j++) {
                            free(list.d[idx].cells[i][j]);
                        }
                        free(list.d[idx].cells[i]);
                    }
                    free(list.d[idx].cells);
                    for (size_t i = idx; i < list.len - 1; i++) list.d[i] = list.d[i + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "TABLES") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s\n", list.d[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].name);
        for (long j = 0; j < list.d[i].rows; j++) {
            for (long k = 0; k < list.d[i].cols; k++) {
                free(list.d[i].cells[j][k]);
            }
            free(list.d[i].cells[j]);
        }
        free(list.d[i].cells);
    }
    free(list.d);
    return 0;
}