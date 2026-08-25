/*
 * F003.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct alias {
    char *name;
    char *expansion;
    struct alias *next;
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
    if (!a) return;
    if (find_alias(new)) return; // name clash
    free(a->name);
    a->name = strdup(new);
}

void erase_alias(const char *name) {
    Alias *prev = NULL, *a = aliases;
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

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[1024];
        if (sscanf(line, "%31s %255s", cmd, arg1) < 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        char *arg1_start = rest;
        while (*rest && *rest != ' ') rest++;
        if (*rest) {
            *rest = 0;
            rest++;
            while (*rest == ' ') rest++;
            strcpy(arg2, rest);
        } else {
            arg2[0] = 0;
        }
        if (strcmp(cmd, "DEFINE") == 0) {
            define_alias(arg1, arg2);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            expand_alias(arg1);
        } else if (strcmp(cmd, "RENAME") == 0) {
            char old[256], new[256];
            if (sscanf(line, "%*s %255s %255s", old, new) == 2)
                rename_alias(old, new);
        } else if (strcmp(cmd, "ERASE") == 0) {
            erase_alias(arg1);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }
    return 0;
}