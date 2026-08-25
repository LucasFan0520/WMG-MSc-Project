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
    Alias *curr = aliases;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
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

void rename_alias(const char *old_name, const char *new_name) {
    Alias *a = find_alias(old_name);
    if (a && !find_alias(new_name)) {
        free(a->name);
        a->name = strdup(new_name);
    }
}

void erase_alias(const char *name) {
    Alias *curr = aliases, *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else aliases = curr->next;
            free(curr->name);
            free(curr->expansion);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void dump_aliases() {
    Alias *curr = aliases;
    while (curr) {
        printf("%s %s\n", curr->name, curr->expansion);
        curr = curr->next;
    }
}

void free_all() {
    Alias *curr = aliases;
    while (curr) {
        Alias *next = curr->next;
        free(curr->name);
        free(curr->expansion);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32], name[256], text[4096] = {0};
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, name, text);
        
        if (strcmp(cmd, "DEFINE") == 0 && n >= 2) define_alias(name, text);
        else if (strcmp(cmd, "EXPAND") == 0 && n >= 1) expand_alias(name);
        else if (strcmp(cmd, "RENAME") == 0 && n >= 2) {
            char new_name[256];
            sscanf(line, "%*s %*s %255s", new_name);
            rename_alias(name, new_name);
        }
        else if (strcmp(cmd, "ERASE") == 0 && n >= 1) erase_alias(name);
        else if (strcmp(cmd, "DUMP") == 0) dump_aliases();
    }
    free_all();
    return 0;
}