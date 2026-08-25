// F038.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

typedef struct { char *name; char *enc; } Enc;
typedef struct { Enc *d; size_t len, cap; } EL;

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

int find_enc(EL *l, const char *name) {
    for (size_t i = 0; i < l->len; i++) if (strcmp(l->d[i].name, name) == 0) return i;
    return -1;
}

int main(void) {
    EL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "STORE") == 0) {
            char *name = next_token(&p);
            char *enc = p;
            if (name && enc) {
                if (find_enc(&list, name) == -1) {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        Enc *nd = realloc(list.d, nc * sizeof(Enc));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        list.d[list.len].name = strdup(name);
                        list.d[list.len].enc = strdup(enc);
                        list.len++;
                    }
                }
            }
        } else if (strcmp(cmd, "DECODE") == 0) {
            char *name = next_token(&p);
            if (name) {
                int idx = find_enc(&list, name);
                if (idx != -1) {
                    char *e = list.d[idx].enc;
                    size_t elen = strlen(e);
                    size_t i = 0;
                    int valid = 1;
                    while (i < elen) {
                        if (!isdigit((unsigned char)e[i])) {
                            valid = 0;
                            break;
                        }
                        long count = 0;
                        while (i < elen && isdigit((unsigned char)e[i])) {
                            count = count * 10 + (e[i] - '0');
                            if (count > 1000000) {
                                valid = 0;
                                break;
                            }
                            i++;
                        }
                        if (!valid || i >= elen) break;
                        i++;
                    }
                    if (valid) {
                        i = 0;
                        while (i < elen) {
                            long count = 0;
                            while (i < elen && isdigit((unsigned char)e[i])) {
                                count = count * 10 + (e[i] - '0');
                                i++;
                            }
                            char c = e[i++];
                            for (long j = 0; j < count; j++) {
                                putchar(c == ' ' ? '_' : c);
                            }
                        }
                        putchar('\n');
                    }
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *name = next_token(&p);
            if (name) {
                int idx = find_enc(&list, name);
                if (idx != -1) {
                    free(list.d[idx].name);
                    free(list.d[idx].enc);
                    for (size_t i = idx; i < list.len - 1; i++) list.d[i] = list.d[i + 1];
                    list.len--;
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s\n", list.d[i].name);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) {
        free(list.d[i].name);
        free(list.d[i].enc);
    }
    free(list.d);
    return 0;
}