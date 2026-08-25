// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Alias {
    char *name;
    char *expansion;
    struct Alias *next;
} Alias;

Alias *head = NULL;

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
        char *new = strdup(exp);
        if (!new) return;
        free(a->expansion);
        a->expansion = new;
        return;
    }
    Alias *new = malloc(sizeof(Alias));
    if (!new) return;
    new->name = strdup(name);
    new->expansion = strdup(exp);
    new->next = NULL;
    if (!new->name || !new->expansion) {
        free(new->name); free(new->expansion); free(new);
        return;
    }
    if (!head) head = new;
    else {
        Alias *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void expand_alias(const char *name) {
    Alias *a = find_alias(name);
    if (!a) return;
    for (char *c = a->expansion; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void rename_alias(const char *old, const char *new) {
    Alias *a = find_alias(old);
    if (!a) return;
    char *newname = strdup(new);
    if (!newname) return;
    free(a->name);
    a->name = newname;
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
        printf("%s %s\n", a->name, a->expansion);
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
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], name[256], exp[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "DEFINE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", name, exp) == 2)
                define_alias(name, exp);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                expand_alias(name);
        } else if (strcmp(cmd, "RENAME") == 0) {
            if (sscanf(line, "%*s %255s %255s", name, exp) == 2) // exp as new name
                rename_alias(name, exp);
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (sscanf(line, "%*s %255s", name) == 1)
                erase_alias(name);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }
    free(line);
    free_all();
    return 0;
}