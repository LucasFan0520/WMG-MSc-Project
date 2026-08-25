// F003.c
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

typedef struct { char *name; char *exp; } Alias;
Alias *a = NULL;
size_t ac = 0, acap = 0;

void def_a(const char *name, const char *exp) {
    for (size_t i = 0; i < ac; i++) {
        if (strcmp(a[i].name, name) == 0) {
            char *tmp = strdup(exp);
            if (!tmp) { perror("strdup"); exit(1); }
            free(a[i].exp);
            a[i].exp = tmp;
            return;
        }
    }
    if (ac == acap) {
        size_t nc = acap ? acap * 2 : 4;
        if (nc > SIZE_MAX / sizeof(Alias)) { fprintf(stderr, "Overflow\n"); exit(1); }
        Alias *tmp = realloc(a, nc * sizeof(Alias));
        if (!tmp) { perror("realloc"); exit(1); }
        a = tmp;
        acap = nc;
    }
    a[ac].name = strdup(name);
    a[ac].exp = strdup(exp);
    if (!a[ac].name || !a[ac].exp) { perror("strdup"); exit(1); }
    ac++;
}

void rename_a(const char *old, const char *new) {
    for (size_t i = 0; i < ac; i++) {
        if (strcmp(a[i].name, old) == 0) {
            char *tmp = strdup(new);
            if (!tmp) { perror("strdup"); exit(1); }
            free(a[i].name);
            a[i].name = tmp;
            return;
        }
    }
}

void erase_a(const char *name) {
    for (size_t i = 0; i < ac; i++) {
        if (strcmp(a[i].name, name) == 0) {
            free(a[i].name); free(a[i].exp);
            a[i] = a[--ac];
            return;
        }
    }
}

void expand_a(const char *name) {
    for (size_t i = 0; i < ac; i++) {
        if (strcmp(a[i].name, name) == 0) {
            for (size_t j = 0; a[i].exp[j]; j++) putchar(a[i].exp[j] == ' ' ? '_' : a[i].exp[j]);
            putchar('\n');
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
        if (strcmp(cmd, "DEFINE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) != 1) { free(line); continue; }
            char *exp = strchr(line, ' ');
            if (exp) { exp = strchr(exp + 1, ' '); if (exp) exp++; }
            if (!exp) exp = "";
            def_a(name, exp);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) expand_a(name);
        } else if (strcmp(cmd, "RENAME") == 0) {
            char old[256], new[256];
            if (sscanf(line, "%*s %255s %255s", old, new) == 2) rename_a(old, new);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) erase_a(name);
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < ac; i++) printf("%s %s\n", a[i].name, a[i].exp);
        }
        free(line);
    }
    for (size_t i = 0; i < ac; i++) { free(a[i].name); free(a[i].exp); }
    free(a);
    return 0;
}