// F013.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *title; char *note; int status; } Task;
typedef struct { Task *d; size_t len, cap; } TL;

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
    TL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "TODO") == 0) {
            char *title = next_token(&p);
            char *note = p;
            if (title && note) {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Task *nd = realloc(list.d, nc * sizeof(Task));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].title = strdup(title);
                    list.d[list.len].note = strdup(note);
                    list.d[list.len].status = 0;
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "START") == 0) {
            char *title = next_token(&p);
            if (title) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].title, title) == 0) { list.d[i].status = 1; break; }
                }
            }
        } else if (strcmp(cmd, "DONE") == 0) {
            char *title = next_token(&p);
            if (title) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].title, title) == 0) { list.d[i].status = 2; break; }
                }
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *title = next_token(&p);
            if (title) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].title, title) == 0) {
                        free(list.d[i].title); free(list.d[i].note);
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--; break;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *title = next_token(&p);
            if (title) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].title, title) == 0) {
                        const char *st = list.d[i].status == 0 ? "todo" : (list.d[i].status == 1 ? "doing" : "done");
                        printf("%s ", st);
                        print_text(list.d[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            const char *names[] = {"todo", "doing", "done"};
            for (int s = 0; s < 3; s++) {
                printf("%s:\n", names[s]);
                for (size_t i = 0; i < list.len; i++) {
                    if (list.d[i].status == s) printf("  %s\n", list.d[i].title);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) { free(list.d[i].title); free(list.d[i].note); }
    free(list.d);
    return 0;
}