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
        Alias *new = malloc(sizeof(Alias));
        if (!new) return;
        new->name = strdup(name);
        new->expansion = strdup(exp);
        new->next = aliases;
        aliases = new;
    }
}

void rename_alias(const char *old, const char *new) {
    Alias *a = find_alias(old);
    if (!a) return;
    if (find_alias(new)) return; // prevent overwrite
    free(a->name);
    a->name = strdup(new);
}

void erase_alias(const char *name) {
    Alias *prev = NULL, *cur = aliases;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else aliases = cur->next;
            free(cur->name);
            free(cur->expansion);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void expand_alias(const char *name) {
    Alias *a = find_alias(name);
    if (!a) return;
    char *s = a->expansion;
    for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void dump_aliases(void) {
    Alias *a = aliases;
    while (a) {
        printf("%s: ", a->name);
        char *s = a->expansion;
        for (; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        a = a->next;
    }
}

void free_all(void) {
    Alias *a = aliases;
    while (a) {
        Alias *next = a->next;
        free(a->name);
        free(a->expansion);
        free(a);
        a = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "DEFINE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (!space) continue;
            *space = '\0';
            char *exp = space + 1;
            while (*exp == ' ') exp++;
            define_alias(name, exp);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            expand_alias(name);
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *old = p;
            char *space = strchr(old, ' ');
            if (!space) continue;
            *space = '\0';
            char *new = space + 1;
            while (*new == ' ') new++;
            rename_alias(old, new);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *name = p;
            char *space = strchr(name, ' ');
            if (space) *space = '\0';
            erase_alias(name);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }
    free_all();
    return 0;
}