// F034.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *name; char *note; } Note;
typedef struct { Note *d; size_t len, cap; } NL;

char *read_line(FILE *f) {
    char *l = NULL;
    size_t len = 0;
    if (getline(&l, &len, f) == -1) {
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

void print_text(const char *t) {
    for (size_t i = 0; t[i]; i++) putchar(t[i] == ' ' ? '_' : t[i]);
    putchar('\n');
}

int main(int argc, char **argv) {
    NL list = {0};
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            NL tmp = {0};
            char *line;
            int valid = 1;
            while ((line = read_line(f)) != NULL) {
                char *p = line;
                char *name = next_token(&p);
                char *note = p;
                if (name && note) {
                    if (tmp.len == tmp.cap) {
                        size_t nc = tmp.cap ? tmp.cap * 2 : 4;
                        Note *nd = realloc(tmp.d, nc * sizeof(Note));
                        if (nd) { tmp.d = nd; tmp.cap = nc; }
                    }
                    if (tmp.len < tmp.cap) {
                        tmp.d[tmp.len].name = strdup(name);
                        tmp.d[tmp.len].note = strdup(note);
                        tmp.len++;
                    }
                } else {
                    valid = 0;
                    free(line);
                    break;
                }
                free(line);
            }
            if (valid) {
                for (size_t i = 0; i < list.len; i++) {
                    free(list.d[i].name);
                    free(list.d[i].note);
                }
                free(list.d);
                list = tmp;
            } else {
                for (size_t i = 0; i < tmp.len; i++) {
                    free(tmp.d[i].name);
                    free(tmp.d[i].note);
                }
                free(tmp.d);
            }
            fclose(f);
        }
    }
    char *line;
    while ((line = read_line(stdin)) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "ADD") == 0) {
            char *name = next_token(&p);
            char *note = p;
            if (name && note) {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Note *nd = realloc(list.d, nc * sizeof(Note));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].name = strdup(name);
                    list.d[list.len].note = strdup(note);
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = next_token(&p);
            if (name) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].name, name) == 0) {
                        free(list.d[i].name);
                        free(list.d[i].note);
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--;
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s ", list.d[i].name);
                print_text(list.d[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].name);
        free(list.d[i].note);
    }
    free(list.d);
    return 0;
}