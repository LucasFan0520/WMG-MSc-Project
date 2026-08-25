// F026.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *cat; char *text; } Note;
typedef struct { Note *d; size_t len, cap; } NL;

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

void print_text(const char *t) {
    for (size_t i = 0; t[i]; i++) putchar(t[i] == ' ' ? '_' : t[i]);
    putchar('\n');
}

int main(void) {
    NL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "ADD") == 0) {
            char *cat = next_token(&p);
            char *text = p;
            if (cat && text) {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Note *nd = realloc(list.d, nc * sizeof(Note));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].cat = strdup(cat);
                    list.d[list.len].text = strdup(text);
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *cat = next_token(&p);
            if (cat) {
                for (size_t i = 0; i < list.len; ) {
                    if (strcmp(list.d[i].cat, cat) == 0) {
                        free(list.d[i].cat);
                        free(list.d[i].text);
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--;
                    } else i++;
                }
            }
        } else if (strcmp(cmd, "KEEP") == 0) {
            char *cat = next_token(&p);
            if (cat) {
                for (size_t i = 0; i < list.len; ) {
                    if (strcmp(list.d[i].cat, cat) != 0) {
                        free(list.d[i].cat);
                        free(list.d[i].text);
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--;
                    } else i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *word = next_token(&p);
            if (word) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strstr(list.d[i].text, word) != NULL) {
                        print_text(list.d[i].text);
                    }
                }
            }
        } else if (strcmp(cmd, "PRINT") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s ", list.d[i].cat);
                print_text(list.d[i].text);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].cat);
        free(list.d[i].text);
    }
    free(list.d);
    return 0;
}