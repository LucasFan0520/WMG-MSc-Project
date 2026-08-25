// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct alias {
    char *name;
    char *expansion;
    struct alias *next;
} Alias;

Alias *head = NULL;
Alias *tail = NULL;

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
        return;
    }
    a = malloc(sizeof(Alias));
    a->name = strdup(name);
    a->expansion = strdup(exp);
    a->next = NULL;
    if (tail) tail->next = a;
    else head = a;
    tail = a;
}

void expand_alias(const char *name) {
    Alias *a = find_alias(name);
    if (!a) return;
    for (char *c = a->expansion; *c; c++)
        putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void rename_alias(const char *old, const char *new) {
    Alias *a = find_alias(old);
    if (!a || find_alias(new)) return;
    free(a->name);
    a->name = strdup(new);
}

void erase_alias(const char *name) {
    Alias *a = head, *prev = NULL;
    while (a) {
        if (strcmp(a->name, name) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            if (a == tail) tail = prev;
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
        for (char *c = a->expansion; *c; c++)
            putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
        a = a->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "DEFINE") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", arg1, arg2) == 2)
                define_alias(arg1, arg2);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                expand_alias(arg1);
        } else if (strcmp(cmd, "RENAME") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2)
                rename_alias(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                erase_alias(arg1);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }
    return 0;
}