// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct alias {
    char *name;
    char *expansion;
    struct alias *next;
} Alias;

Alias *head = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

Alias *find_alias(const char *name) {
    Alias *a = head;
    while (a) {
        if (strcmp(a->name, name) == 0) return a;
        a = a->next;
    }
    return NULL;
}

void define_alias(const char *name, const char *exp) {
    Alias *a = find_alias(name);
    if (a) {
        free(a->expansion);
        a->expansion = strdup(exp);
        if (!a->expansion) { fprintf(stderr, "Memory error\n"); exit(1); }
        return;
    }
    Alias *newa = malloc(sizeof(Alias));
    if (!newa) { fprintf(stderr, "Memory error\n"); exit(1); }
    newa->name = strdup(name);
    newa->expansion = strdup(exp);
    if (!newa->name || !newa->expansion) { fprintf(stderr, "Memory error\n"); exit(1); }
    newa->next = head;
    head = newa;
}

void expand_alias(const char *name) {
    Alias *a = find_alias(name);
    if (!a) return;
    for (char *s = a->expansion; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
    }
    putchar('\n');
}

void rename_alias(const char *old, const char *new) {
    Alias *a = find_alias(old);
    if (!a) return;
    if (find_alias(new)) return;
    free(a->name);
    a->name = strdup(new);
    if (!a->name) { fprintf(stderr, "Memory error\n"); exit(1); }
}

void erase_alias(const char *name) {
    Alias *a = head, *prev = NULL;
    while (a) {
        if (strcmp(a->name, name) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            free(a->name);
            free(a->expansion);
            free(a);
            return;
        }
        prev = a;
        a = a->next;
    }
}

void dump_aliases(void) {
    Alias *a = head;
    while (a) {
        printf("%s ", a->name);
        for (char *s = a->expansion; *s; s++) {
            putchar(*s == ' ' ? '_' : *s);
        }
        putchar('\n');
        a = a->next;
    }
}

void free_all(void) {
    Alias *a = head;
    while (a) {
        Alias *next = a->next;
        free(a->name);
        free(a->expansion);
        free(a);
        a = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "DUMP") == 0) { dump_aliases(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "DEFINE") == 0) {
            char *name = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            define_alias(name, rest);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            expand_alias(rest);
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *old = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            char *new = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rename_alias(old, new);
        } else if (strcmp(cmd, "ERASE") == 0) {
            erase_alias(rest);
        }
    }
    free_all();
    return 0;
}