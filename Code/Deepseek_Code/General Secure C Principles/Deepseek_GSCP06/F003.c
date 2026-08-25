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
    if (!a) return;
    for (char *c = a->expansion; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void rename_alias(const char *old, const char *new) {
    Alias *a = find_alias(old);
    if (!a) return;
    if (find_alias(new)) return; // name conflict
    free(a->name);
    a->name = strdup(new);
}

void erase_alias(const char *name) {
    Alias *a = aliases, *prev = NULL;
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

void dump_aliases(void) {
    Alias *a = aliases;
    while (a) {
        printf("%s %s\n", a->name, a->expansion);
        a = a->next;
    }
}

void cleanup_aliases(void) {
    Alias *a = aliases;
    while (a) {
        Alias *tmp = a;
        a = a->next;
        free(tmp->name);
        free(tmp->expansion);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[1024];
        int n = sscanf(line, "%31s %255s %1023s", cmd, arg1, arg2);
        if (n < 2) continue;
        if (strcmp(cmd, "DEFINE") == 0 && n >= 3) {
            char *exp = line + strlen(cmd) + 1 + strlen(arg1) + 1;
            define_alias(arg1, exp);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            expand_alias(arg1);
        } else if (strcmp(cmd, "RENAME") == 0 && n == 3) {
            rename_alias(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0) {
            erase_alias(arg1);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }
    cleanup_aliases();
    return 0;
}