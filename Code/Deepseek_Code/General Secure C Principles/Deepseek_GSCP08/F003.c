// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Alias { char *name; char *exp; struct Alias *next; } Alias;
Alias *aliases = NULL;

Alias *find_alias(const char *name) {
    for (Alias *a = aliases; a; a = a->next)
        if (strcmp(a->name, name) == 0) return a;
    return NULL;
}

void define_alias(const char *name, const char *exp) {
    Alias *a = find_alias(name);
    if (a) { free(a->exp); a->exp = strdup(exp); return; }
    a = malloc(sizeof(Alias));
    if (!a) return;
    a->name = strdup(name); a->exp = strdup(exp); a->next = aliases; aliases = a;
}

void expand_alias(const char *name) {
    Alias *a = find_alias(name);
    if (!a) return;
    for (char *c = a->exp; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void rename_alias(const char *old, const char *new) {
    Alias *a = find_alias(old);
    if (!a || find_alias(new)) return;
    free(a->name); a->name = strdup(new);
}

void erase_alias(const char *name) {
    Alias *prev = NULL;
    for (Alias *a = aliases; a; prev = a, a = a->next) {
        if (strcmp(a->name, name) == 0) {
            if (prev) prev->next = a->next; else aliases = a->next;
            free(a->name); free(a->exp); free(a);
            return;
        }
    }
}

void dump_aliases(void) {
    for (Alias *a = aliases; a; a = a->next) {
        printf("%s ", a->name);
        for (char *c = a->exp; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "DEFINE") == 0) {
            char name[256], exp[4096];
            if (sscanf(rest, "%255s", name) == 1) {
                char *e = rest + strlen(name);
                while (*e == ' ') e++;
                strcpy(exp, e);
                define_alias(name, exp);
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) expand_alias(name);
        } else if (strcmp(cmd, "RENAME") == 0) {
            char old[256], new[256];
            if (sscanf(rest, "%255s %255s", old, new) == 2) rename_alias(old, new);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) erase_alias(name);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }
    while (aliases) { Alias *a = aliases; aliases = a->next; free(a->name); free(a->exp); free(a); }
    return 0;
}