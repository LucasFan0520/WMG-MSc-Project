// F016.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct { char *name; char *email; char *phone; } Con;
typedef struct { Con *d; size_t len, cap; } CL;

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
    CL list = {0};
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char *line = NULL; size_t len = 0;
            while (getline(&line, &len, f) != -1) {
                size_t sl = strlen(line);
                if (sl > 0 && line[sl - 1] == '\n') line[sl - 1] = '\0';
                char *p = line;
                char *name = strsep(&p, ",");
                char *email = strsep(&p, ",");
                char *phone = p;
                if (name && email && phone && *name && *email && *phone) {
                    if (list.len == list.cap) {
                        size_t nc = list.cap ? list.cap * 2 : 4;
                        Con *nd = realloc(list.d, nc * sizeof(Con));
                        if (nd) { list.d = nd; list.cap = nc; }
                    }
                    if (list.len < list.cap) {
                        list.d[list.len].name = strdup(name);
                        list.d[list.len].email = strdup(email);
                        list.d[list.len].phone = strdup(phone);
                        list.len++;
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
            char *name = strsep(&p, ",");
            char *email = strsep(&p, ",");
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
                    list.len++;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0 && p) {
            char *name = p;
            for (size_t i = 0; i < list.len; i++) {
                if (strcmp(list.d[i].name, name) == 0) {
                    free(list.d[i].name); free(list.d[i].email); free(list.d[i].phone);
                    for (size_t j = i; j < list.len - 1; j++) list.d[j] = list.d[j + 1];
                    list.len--; break;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0 && p) {
            char *name = p;
            for (size_t i = 0; i < list.len; i++) {
                if (strcmp(list.d[i].name, name) == 0) {
                    printf("%s,%s,%s\n", list.d[i].name, list.d[i].email, list.d[i].phone);
                    break;
                }
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (size_t i = 0; i < list.len; i++) {
                printf("%s,%s,%s\n", list.d[i].name, list.d[i].email, list.d[i].phone);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < list.len; i++) { free(list.d[i].name); free(list.d[i].email); free(list.d[i].phone); }
    free(list.d);
    return 0;
}