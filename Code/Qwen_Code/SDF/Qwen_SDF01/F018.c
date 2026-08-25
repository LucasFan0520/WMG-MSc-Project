// F018.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *word; long count; } WF;
typedef struct { WF *d; size_t len, cap; } WL;

char *read_line(void) {
    char *l = NULL; size_t len = 0;
    if (getline(&l, &len, stdin) == -1) { free(l); return NULL; }
    size_t sl = strlen(l);
    if (sl > 0 && l[sl - 1] == '\n') l[sl - 1] = '\0';
    return l;
}

int cmp_wf(const void *a, const void *b) {
    const WF *aa = a, *bb = b;
    if (aa->count != bb->count) return (bb->count > aa->count) - (bb->count < aa->count);
    return strcmp(aa->word, bb->word);
}

int main(int argc, char **argv) {
    WL list = {0};
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *line = NULL; size_t len = 0;
            while (getline(&line, &len, f) != -1) {
                size_t sl = strlen(line);
                if (sl > 0 && line[sl - 1] == '\n') line[sl - 1] = '\0';
                if (*line) {
                    int idx = -1;
                    for (size_t i = 0; i < list.len; i++) if (strcmp(list.d[i].word, line) == 0) { idx = i; break; }
                    if (idx != -1) {
                        list.d[idx].count++;
                    } else {
                        if (list.len == list.cap) {
                            size_t nc = list.cap ? list.cap * 2 : 4;
                            WF *nd = realloc(list.d, nc * sizeof(WF));
                            if (nd) { list.d = nd; list.cap = nc; }
                        }
                        if (list.len < list.cap) {
                            list.d[list.len].word = strdup(line);
                            list.d[list.len].count = 1;
                            list.len++;
                        }
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
        
        if (strcmp(cmd, "WORD") == 0 && p) {
            char *word = p;
            int idx = -1;
            for (size_t i = 0; i < list.len; i++) if (strcmp(list.d[i].word, word) == 0) { idx = i; break; }
            if (idx != -1) {
                list.d[idx].count++;
            } else {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    WF *nd = realloc(list.d, nc * sizeof(WF));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].word = strdup(word);
                    list.d[list.len].count = 1;
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0 && p) {
            char *word = p;
            for (size_t i = 0; i < list.len; i++) {
                if (strcmp(list.d[i].word, word) == 0) {
                    free(list.d[i].word);
                    for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                    list.len--; break;
                }
            }
        } else if (strcmp(cmd, "COUNT") == 0 && p) {
            char *word = p;
            long c = 0;
            for (size_t i = 0; i < list.len; i++) {
                if (strcmp(list.d[i].word, word) == 0) { c = list.d[i].count; break; }
            }
            printf("%ld\n", c);
        } else if (strcmp(cmd, "TOP") == 0) {
            if (list.len > 0) qsort(list.d, list.len, sizeof(WF), cmp_wf);
            for (size_t i = 0; i < list.len; i++) printf("%s %ld\n", list.d[i].word, list.d[i].count);
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) free(list.d[i].word);
    free(list.d);
    return 0;
}