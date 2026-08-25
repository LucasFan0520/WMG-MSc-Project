// F015.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *name; char *email; char *phone; char *note; } Con;
typedef struct { Con *d; size_t len, cap; } CL;

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
    CL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "ADD") == 0) {
            char *name = next_token(&p);
            char *email = next_token(&p);
            char *phone = p;
            if (name && email && phone) {
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Con *nd = realloc(list.d, nc * sizeof(Con));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].name = strdup(name);
                    list.d[list.len].email = strdup(email);
                    list.d[list.len].phone = strdup(phone);
                    list.d[list.len].note = strdup("");
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "NOTE") == 0) {
            char *name = next_token(&p);
            char *text = p;
            if (name && text) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].name, name) == 0) {
                        char *nn = strdup(text);
                        if (nn) { free(list.d[i].note); list.d[i].note = nn; }
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "MERGE") == 0) {
            char *keep = next_token(&p);
            char *rem = p;
            if (keep && rem) {
                int ik = -1, ir = -1;
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].name, keep) == 0) ik = i;
                    if (strcmp(list.d[i].name, rem) == 0) ir = i;
                }
                if (ik != -1 && ir != -1) {
                    size_t ol = strlen(list.d[ik].note);
                    size_t al = strlen(list.d[ir].note);
                    if (ol + al + 2 > ol) {
                        char *nn = malloc(ol + al + 2);
                        if (nn) {
                            memcpy(nn, list.d[ik].note, ol);
                            if (ol > 0) nn[ol++] = ' ';
                            memcpy(nn + ol, list.d[ir].note, al + 1);
                            free(list.d[ik].note);
                            list.d[ik].note = nn;
                        }
                    }
                    free(list.d[ir].name); free(list.d[ir].email); free(list.d[ir].phone); free(list.d[ir].note);
                    for (size_t j = ir; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = next_token(&p);
            if (name) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].name, name) == 0) {
                        free(list.d[i].name); free(list.d[i].email); free(list.d[i].phone); free(list.d[i].note);
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--; break;
                    }
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *name = next_token(&p);
            if (name) {
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].name, name) == 0) {
                        printf("%s %s %s ", list.d[i].name, list.d[i].email, list.d[i].phone);
                        print_text(list.d[i].note);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s %s %s ", list.d[i].name, list.d[i].email, list.d[i].phone);
                print_text(list.d[i].note);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].name); free(list.d[i].email); free(list.d[i].phone); free(list.d[i].note);
    }
    free(list.d);
    return 0;
}