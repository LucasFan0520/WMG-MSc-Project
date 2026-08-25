// F033.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

char* read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) { free(buf); return NULL; }
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
        buf[len++] = (char)c;
    }
    if (len == 0 && c == EOF) { free(buf); return NULL; }
    buf[len] = '\0';
    return buf;
}

typedef struct { char *id; char *name; char *dept; char *title; } Emp;
Emp *emps = NULL;
size_t ec = 0, ecap = 0;

void add_emp(const char *id, const char *name, const char *dept, const char *title) {
    for (size_t i = 0; i < ec; i++) {
        if (strcmp(emps[i].id, id) == 0) return;
    }
    if (ec == ecap) {
        size_t nc = ecap ? ecap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Emp)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Emp *tmp = realloc(emps, nc * sizeof(Emp));
        if (!tmp) { perror("realloc"); exit(1); }
        emps = tmp;
        ecap = nc;
    }
    emps[ec].id = strdup(id);
    emps[ec].name = strdup(name);
    emps[ec].dept = strdup(dept);
    emps[ec].title = strdup(title);
    if (!emps[ec].id || !emps[ec].name || !emps[ec].dept || !emps[ec].title) { perror("strdup"); exit(1); }
    ec++;
}

void upd_emp(const char *id, const char *name, const char *dept, const char *title) {
    for (size_t i = 0; i < ec; i++) {
        if (strcmp(emps[i].id, id) == 0) {
            char *n = strdup(name), *d = strdup(dept), *t = strdup(title);
            if (!n || !d || !t) { perror("strdup"); exit(1); }
            free(emps[i].name); free(emps[i].dept); free(emps[i].title);
            emps[i].name = n; emps[i].dept = d; emps[i].title = t;
            return;
        }
    }
}

void del_emp(const char *id) {
    for (size_t i = 0; i < ec; i++) {
        if (strcmp(emps[i].id, id) == 0) {
            free(emps[i].id); free(emps[i].name); free(emps[i].dept); free(emps[i].title);
            emps[i] = emps[--ec];
            return;
        }
    }
}

int main() {
    char *line;
    while ((line = read_line()) != NULL) {
        if (strcmp(line, "END") == 0) { free(line); break; }
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) { free(line); continue; }
        if (strcmp(cmd, "ADD") == 0) {
            char id[256], name[256], dept[256], title[256];
            if (sscanf(line, "%*s %255s %255s %255s %255s", id, name, dept, title) == 4) add_emp(id, name, dept, title);
        } else if (strcmp(cmd, "UPDATE") == 0) {
            char id[256], name[256], dept[256], title[256];
            if (sscanf(line, "%*s %255s %255s %255s %255s", id, name, dept, title) == 4) upd_emp(id, name, dept, title);
        } else if (strcmp(cmd, "DELETE") == 0) {
            char id[256];
            if (sscanf(line, "%*s %255s", id) == 1) del_emp(id);
        } else if (strcmp(cmd, "FIND") == 0) {
            char id[256];
            if (sscanf(line, "%*s %255s", id) == 1) {
                for (size_t i = 0; i < ec; i++) {
                    if (strcmp(emps[i].id, id) == 0) {
                        printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].dept, emps[i].title);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "LIST") == 0) {
            for (size_t i = 0; i < ec; i++) printf("%s %s %s %s\n", emps[i].id, emps[i].name, emps[i].dept, emps[i].title);
        }
        free(line);
    }
    for (size_t i = 0; i < ec; i++) { free(emps[i].id); free(emps[i].name); free(emps[i].dept); free(emps[i].title); }
    free(emps);
    return 0;
}