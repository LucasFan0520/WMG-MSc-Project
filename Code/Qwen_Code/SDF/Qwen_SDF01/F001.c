// F001.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *id; char *text; } Para;
typedef struct { Para *d; size_t len, cap; } PL;

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
    PL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        char *id = next_token(&p);
        char *text = p;
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "NEW") == 0 && id && text) {
            int found = 0;
            for (size_t i = 0; i < list.len; i++) if (strcmp(list.d[i].id, id) == 0) { found = 1; break; }
            if (!found) {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Para *nd = realloc(list.d, nc * sizeof(Para));
                    if (!nd) { free(line); continue; }
                    list.d = nd; list.cap = nc;
                }
                list.d[list.len].id = strdup(id);
                list.d[list.len].text = strdup(text);
                list.len++;
            }
        } else if (strcmp(cmd, "APPEND") == 0 && id && text) {
            for (size_t i = 0; i < list.len; i++) {
                if (strcmp(list.d[i].id, id) == 0) {
                    size_t ol = strlen(list.d[i].text);
                    size_t al = strlen(text);
                    if (ol + al + 1 > ol) {
                        char *nt = malloc(ol + al + 1);
                        if (nt) {
                            memcpy(nt, list.d[i].text, ol);
                            memcpy(nt + ol, text, al + 1);
                            free(list.d[i].text);
                            list.d[i].text = nt;
                        }
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "REPLACE") == 0 && id && text) {
            for (size_t i = 0; i < list.len; i++) {
                if (strcmp(list.d[i].id, id) == 0) {
                    char *nt = strdup(text);
                    if (nt) { free(list.d[i].text); list.d[i].text = nt; }
                    break;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && id) {
            for (size_t i = 0; i < list.len; i++) {
                if (strcmp(list.d[i].id, id) == 0) {
                    free(list.d[i].id); free(list.d[i].text);
                    for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                    list.len--; break;
                }
            }
        } else if (strcmp(cmd, "SHOW") == 0 && id) {
            int found = 0;
            for (size_t i = 0; i < list.len; i++) {
                if (strcmp(list.d[i].id, id) == 0) { print_text(list.d[i].text); found = 1; break; }
            }
            if (!found) printf("NOT_FOUND\n");
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.len; i++) printf("%s\n", list.d[i].id);
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) { free(list.d[i].id); free(list.d[i].text); }
    free(list.d);
    return 0;
}