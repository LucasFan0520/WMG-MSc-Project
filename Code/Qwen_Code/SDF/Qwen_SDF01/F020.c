// F020.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *title; char **body; size_t blen, bcap; } Rec;
typedef struct { Rec *d; size_t len, cap; } RL;

char *read_line(void) {
    char *l = NULL; size_t len = 0;
    if (getline(&l, &len, stdin) == -1) { free(l); return NULL; }
    size_t sl = strlen(l);
    if (sl > 0 && l[sl - 1] == '\n') l[sl - 1] = '\0';
    return l;
}

int main(int argc, char **argv) {
    RL list = {0};
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *line = NULL; size_t len = 0;
            Rec *cur = NULL;
            while (getline(&line, &len, f) != -1) {
                size_t sl = strlen(line);
                if (sl > 0 && line[sl - 1] == '\n') line[sl - 1] = '\0';
                if (strcmp(line, "--") == 0) {
                    cur = NULL;
                } else {
                    if (!cur) {
                        if (list.len == list.cap) {
                            size_t nc = list.cap ? list.cap * 2 : 4;
                            Rec *nd = realloc(list.d, nc * sizeof(Rec));
                            if (nd) { list.d = nd; list.cap = nc; }
                        }
                        if (list.len < list.cap) {
                            cur = &list.d[list.len++];
                            cur->title = strdup(line);
                            cur->body = NULL; cur->blen = 0; cur->bcap = 0;
                        }
                    } else {
                        if (cur->blen == cur->bcap) {
                            size_t nc = cur->bcap ? cur->bcap * 2 : 4;
                            char **nd = realloc(cur->body, nc * sizeof(char*));
                            if (nd) { cur->body = nd; cur->bcap = nc; }
                        }
                        if (cur->blen < cur->bcap) cur->body[cur->blen++] = strdup(line);
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
        
        if (strcmp(cmd, "FIND") == 0 && p) {
            char *title = p;
            for (size_t i = 0; i < list.len; i++) {
                if (strcmp(list.d[i].title, title) == 0) {
                    printf("%s\n", list.d[i].title);
                    for (size_t j = 0; j < list.d[i].blen; j++) printf("%s\n", list.d[i].body[j]);
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && p) {
            char *title = p;
            for (size_t i = 0; i < list.len; i++) {
                if (strcmp(list.d[i].title, title) == 0) {
                    free(list.d[i].title);
                    for (size_t j = 0; j < list.d[i].blen; j++) free(list.d[i].body[j]);
                    free(list.d[i].body);
                    for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                    list.len--; break;
                }
            }
        } else if (strcmp(cmd, "ADD") == 0 && p) {
            char *title = p;
            if (list.len == list.cap) {
                size_t nc = list.cap ? list.cap * 2 : 4;
                Rec *nd = realloc(list.d, nc * sizeof(Rec));
                if (nd) { list.d = nd; list.cap = nc; }
            }
            if (list.len < list.cap) {
                Rec *cur = &list.d[list.len++];
                cur->title = strdup(title);
                cur->body = NULL; cur->blen = 0; cur->bcap = 0;
                char *bline;
                while ((bline = read_line()) != NULL) {
                    if (strcmp(bline, "--") == 0) { free(bline); break; }
                    if (cur->blen == cur->bcap) {
                        size_t nc = cur->bcap ? cur->bcap * 2 : 4;
                        char **nd = realloc(cur->body, nc * sizeof(char*));
                        if (nd) { cur->body = nd; cur->bcap = nc; }
                    }
                    if (cur->blen < cur->bcap) cur->body[cur->blen++] = strdup(bline);
                    free(bline);
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s\n", list.d[i].title);
                for (size_t j = 0; j < list.d[i].blen; j++) printf("%s\n", list.d[i].body[j]);
                printf("--\n");
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].title);
        for (size_t j = 0; j < list.d[i].blen; j++) free(list.d[i].body[j]);
        free(list.d[i].body);
    }
    free(list.d);
    return 0;
}