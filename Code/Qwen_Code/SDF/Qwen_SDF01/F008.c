// F008.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *name; long score; char *note; } Stu;
typedef struct { Stu *d; size_t len, cap; } SL;

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

void print_text(const char *t) {
    for (size_t i = 0; t[i]; i++) putchar(t[i] == ' ' ? '_' : t[i]);
    putchar('\n');
}

int main(void) {
    SL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "UPDATE") == 0) {
            char *name = next_token(&p);
            char *score_s = next_token(&p);
            char *note = p;
            if (name && score_s && note) {
                long sc = strtol(score_s, NULL, 10);
                int idx = -1;
                for (size_t i = 0; i < list.len; i++) if (strcmp(list.d[i].name, name) == 0) { idx = i; break; }
                if (idx != -1) {
                    list.d[idx].score = sc;
                    char *nn = strdup(note);
                    if (nn) { free(list.d[idx].note); list.d[idx].note = nn; }
                } else if (strcmp(cmd, "ADD") == 0) {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        Stu *nd = realloc(list.d, nc * sizeof(Stu));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        list.d[list.len].name = strdup(name);
                        list.d[list.len].score = sc;
                        list.d[list.len].note = strdup(note);
                        list.len++;
                    }
                }
            }
        } else if (strcmp(cmd, "DROP") == 0) {
            char *name = next_token(&p);
            if (name) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].name, name) == 0) {
                        free(list.d[i].name); free(list.d[i].note);
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--; break;
                    }
                }
            }
        } else if (strcmp(cmd, "AVERAGE") == 0) {
            if (list.len > 0) {
                long sum = 0;
                for (size_t i = 0; i < list.len; i++) sum += list.d[i].score;
                printf("%ld\n", sum / (long)list.len);
            } else printf("0\n");
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s %ld ", list.d[i].name, list.d[i].score);
                print_text(list.d[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) { free(list.d[i].name); free(list.d[i].note); }
    free(list.d);
    return 0;
}