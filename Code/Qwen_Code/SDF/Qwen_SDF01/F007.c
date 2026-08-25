// F007.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { long time; char *label; char *note; } Act;
typedef struct { Act *d; size_t len, cap; } AL;

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

int cmp_act(const void *a, const void *b) {
    const Act *aa = a, *bb = b;
    if (aa->time != bb->time) return (aa->time > bb->time) - (aa->time < bb->time);
    return strcmp(aa->label, bb->label);
}

int main(void) {
    AL list = {0};
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *p = line;
        char *cmd = next_token(&p);
        if (!cmd) { free(line); continue; }
        
        if (strcmp(cmd, "ADD") == 0) {
            char *time_s = next_token(&p);
            char *label = next_token(&p);
            char *note = p;
            if (time_s && label && note) {
                long t = strtol(time_s, NULL, 10);
                if (list.len == list.cap) {
                    size_t nc = list.cap ? list.cap * 2 : 4;
                    Act *nd = realloc(list.d, nc * sizeof(Act));
                    if (nd) { list.d = nd; list.cap = nc; }
                }
                if (list.len < list.cap) {
                    list.d[list.len].time = t;
                    list.d[list.len].label = strdup(label);
                    list.d[list.len].note = strdup(note);
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            char *time_s = next_token(&p);
            char *label = next_token(&p);
            if (time_s && label) {
                long t = strtol(time_s, NULL, 10);
                for (size_t i = 0; i < list.len; ) {
                    if (list.d[i].time == t && strcmp(list.d[i].label, label) == 0) {
                        free(list.d[i].label); free(list.d[i].note);
                        for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                        list.len--;
                    } else i++;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            char *label = next_token(&p);
            if (label) {
                long min_t = -1;
                int idx = -1;
                for (size_t i = 0; i < list.len; i++) {
                    if (strcmp(list.d[i].label, label) == 0) {
                        if (idx == -1 || list.d[i].time < min_t) {
                            min_t = list.d[i].time;
                            idx = i;
                        }
                    }
                }
                if (idx != -1) {
                    printf("%ld %s ", list.d[idx].time, list.d[idx].label);
                    print_text(list.d[idx].note);
                }
            }
        } else if (strcmp(cmd, "SORT") == 0) {
            if (list.len > 0) qsort(list.d, list.len, sizeof(Act), cmp_act);
            for (size_t i = 0; i < list.len; i++) {
                printf("%ld %s ", list.d[i].time, list.d[i].label);
                print_text(list.d[i].note);
            }
        } else if (strcmp(cmd, "CLEAR") == 0) {
            for (size_t i = 0; i < list.len; i++) { free(list.d[i].label); free(list.d[i].note); }
            list.len = 0;
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) { free(list.d[i].label); free(list.d[i].note); }
    free(list.d);
    return 0;
}