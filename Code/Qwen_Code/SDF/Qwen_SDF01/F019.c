// F019.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *acc; long amt; char *memo; } Tx;
typedef struct { Tx *d; size_t len, cap; } TL;

char *read_line(void) {
    char *l = NULL; size_t len = 0;
    if (getline(&l, &len, stdin) == -1) { free(l); return NULL; }
    size_t sl = strlen(l);
    if (sl > 0 && l[sl - 1] == '\n') l[sl - 1] = '\0';
    return l;
}

void print_text(const char *t) {
    for (size_t i = 0; t[i]; i++) putchar(t[i] == ' ' ? '_' : t[i]);
    putchar('\n');
}

int main(int argc, char **argv) {
    TL list = {0};
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *line = NULL; size_t len = 0;
            while (getline(&line, &len, f) != -1) {
                size_t sl = strlen(line);
                if (sl > 0 && line[sl - 1] == '\n') line[sl - 1] = '\0';
                char *p = line;
                char *acc = strsep(&p, " ");
                char *amt_s = strsep(&p, " ");
                char *memo = p;
                if (acc && amt_s && memo) {
                    char *end;
                    long amt = strtol(amt_s, &end, 10);
                    if (*end == '\0') {
                        if (list.len == list.cap) {
                            size_t nc = list.cap ? list.cap * 2 : 4;
                            Tx *nd = realloc(list.d, nc * sizeof(Tx));
                            if (nd) { list.d = nd; list.cap = nc; }
                        }
                        if (list.len < list.cap) {
                            list.d[list.len].acc = strdup(acc);
                            list.d[list.len].amt = amt;
                            list.d[list.len].memo = strdup(memo);
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
        
        if (strcmp(cmd, "ADD") == 0 && p) {
            char *acc = strsep(&p, " ");
            char *amt_s = strsep(&p, " ");
            char *memo = p;
            if (acc && amt_s && memo) {
                char *end;
                long amt = strtol(amt_s, &end, 10);
                if (*end == '\0') {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        Tx *nd = realloc(list.d, nc * sizeof(Tx));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        list.d[list.len].acc = strdup(acc);
                        list.d[list.len].amt = amt;
                        list.d[list.len].memo = strdup(memo);
                        list.len++;
                    }
                }
            }
        } else if (strcmp(cmd, "REVERSE") == 0 && p) {
            char *acc = strsep(&p, " ");
            char *amt_s = p;
            if (acc && amt_s) {
                char *end;
                long amt = strtol(amt_s, &end, 10);
                if (*end == '\0') {
                    for (size_t i = 0; i < list.len; ) {
                        if (strcmp(list.d[i].acc, acc) == 0 && list.d[i].amt == amt) {
                            free(list.d[i].acc); free(list.d[i].memo);
                            for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                            list.len--;
                        } else i++;
                    }
                }
            }
        } else if (strcmp(cmd, "BALANCE") == 0 && p) {
            char *acc = p;
            long bal = 0;
            for (size_t i = 0; i < list.len; i++) {
                if (strcmp(list.d[i].acc, acc) == 0) bal += list.d[i].amt;
            }
            printf("%ld\n", bal);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s %ld ", list.d[i].acc, list.d[i].amt);
                print_text(list.d[i].memo);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) { free(list.d[i].acc); free(list.d[i].memo); }
    free(list.d);
    return 0;
}