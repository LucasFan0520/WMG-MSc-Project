/* F003.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Alias {
    char *name;
    char *expansion;
    struct Alias *next;
} Alias;

Alias *head = NULL;

Alias* find_alias(const char *name) {
    Alias *a = head;
    while (a) {
        if (strcmp(a->name, name) == 0) return a;
        a = a->next;
    }
    return NULL;
}

void define_alias(const char *name, const char *expansion) {
    Alias *a = find_alias(name);
    if (a) {
        free(a->expansion);
        a->expansion = strdup(expansion);
    } else {
        a = malloc(sizeof(Alias));
        a->name = strdup(name);
        a->expansion = strdup(expansion);
        a->next = head;
        head = a;
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
    if (find_alias(new)) return;
    free(a->name);
    a->name = strdup(new);
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

void dump_aliases() {
    Alias *a = head;
    while (a) {
        printf("%s %s\n", a->name, a->expansion);
        a = a->next;
    }
}

void free_all() {
    while (head) {
        Alias *tmp = head;
        head = head->next;
        free(tmp->name);
        free(tmp->expansion);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char name[256]; char exp[4096];
        int n = sscanf(line, "%31s %255s %[^\n]", cmd, name, exp);
        if (strcmp(cmd, "DEFINE") == 0 && n == 3) {
            define_alias(name, exp);
        } else if (strcmp(cmd, "EXPAND") == 0 && n == 2) {
            expand_alias(name);
        } else if (strcmp(cmd, "RENAME") == 0 && n == 3) {
            rename_alias(name, exp);
        } else if (strcmp(cmd, "ERASE") == 0 && n == 2) {
            erase_alias(name);
        } else if (strcmp(cmd, "DUMP") == 0 && n == 1) {
            dump_aliases();
        }
    }
    free_all();
    return 0;
}