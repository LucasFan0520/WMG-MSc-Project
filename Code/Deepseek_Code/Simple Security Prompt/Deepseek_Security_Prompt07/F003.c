// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Alias {
    char *name;
    char *expansion;
    struct Alias *next;
} Alias;

Alias *aliases = NULL;

Alias *find_alias(const char *name) {
    Alias *a = aliases;
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
    } else {
        a = malloc(sizeof(Alias));
        a->name = strdup(name);
        a->expansion = strdup(exp);
        a->next = aliases;
        aliases = a;
    }
}

void expand_alias(const char *name) {
    Alias *a = find_alias(name);
    if (!a) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *s = a->expansion; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void rename_alias(const char *old, const char *new) {
    Alias *a = find_alias(old);
    if (!a || find_alias(new)) return;
    free(a->name);
    a->name = strdup(new);
}

void erase_alias(const char *name) {
    Alias *prev = NULL;
    Alias *a = aliases;
    while (a) {
        if (strcmp(a->name, name) == 0) {
            if (prev) prev->next = a->next;
            else aliases = a->next;
            free(a->name);
            free(a->expansion);
            free(a);
            return;
        }
        prev = a;
        a = a->next;
    }
}

void dump_aliases() {
    Alias *a = aliases;
    while (a) {
        printf("%s:", a->name);
        for (char *s = a->expansion; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        a = a->next;
    }
}

void free_all() {
    Alias *a = aliases;
    while (a) {
        Alias *next = a->next;
        free(a->name);
        free(a->expansion);
        free(a);
        a = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "DEFINE") == 0) {
            char name[256], exp[4096];
            if (sscanf(rest, "%255s %[^\n]", name, exp) == 2) {
                define_alias(name, exp);
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) {
                expand_alias(name);
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char old[256], new[256];
            if (sscanf(rest, "%255s %255s", old, new) == 2) {
                rename_alias(old, new);
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) == 1) {
                erase_alias(name);
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }
    free_all();
    return 0;
}