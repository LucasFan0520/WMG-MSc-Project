// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

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

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void define_alias(const char *name, const char *expansion) {
    Alias *a = find_alias(name);
    if (a) {
        free(a->expansion);
        a->expansion = safe_strdup(expansion);
    } else {
        a = malloc(sizeof(Alias));
        if (!a) return;
        a->name = safe_strdup(name);
        a->expansion = safe_strdup(expansion);
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
    for (const char *s = a->expansion; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void rename_alias(const char *old, const char *new) {
    Alias *a = find_alias(old);
    if (!a) return;
    free(a->name);
    a->name = safe_strdup(new);
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

void dump_aliases(void) {
    Alias *a = aliases;
    while (a) {
        printf("%s %s\n", a->name, a->expansion);
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
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512];
        int n = sscanf(line, "%31s %511s %511s", cmd, arg1, arg2);
        if (n < 1) continue;

        if (strcmp(cmd, "DEFINE") == 0 && n >= 3) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            define_alias(arg1, p);
        } else if (strcmp(cmd, "EXPAND") == 0 && n >= 2) {
            expand_alias(arg1);
        } else if (strcmp(cmd, "RENAME") == 0 && n >= 3) {
            rename_alias(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0 && n >= 2) {
            erase_alias(arg1);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }
    free_all();
    return 0;
}