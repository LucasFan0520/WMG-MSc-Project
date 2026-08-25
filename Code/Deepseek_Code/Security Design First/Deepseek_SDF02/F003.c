/* F003.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct alias {
    char *name;
    char *expansion;
    struct alias *next;
} alias_t;

alias_t *head = NULL;

alias_t *find_alias(const char *name) {
    alias_t *a = head;
    while (a) {
        if (strcmp(a->name, name) == 0) return a;
        a = a->next;
    }
    return NULL;
}

void define_alias(const char *name, const char *exp) {
    alias_t *a = find_alias(name);
    if (a) {
        char *new = strdup(exp);
        if (!new) return;
        free(a->expansion);
        a->expansion = new;
        return;
    }
    a = malloc(sizeof(alias_t));
    if (!a) return;
    a->name = strdup(name);
    a->expansion = strdup(exp);
    a->next = head;
    head = a;
    if (!a->name || !a->expansion) {
        free(a->name); free(a->expansion); free(a);
    }
}

void expand_alias(const char *name) {
    alias_t *a = find_alias(name);
    if (!a) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *s = a->expansion; *s; s++) {
        if (*s == ' ') putchar('_');
        else putchar(*s);
    }
    putchar('\n');
}

void rename_alias(const char *old, const char *new) {
    alias_t *a = find_alias(old);
    if (!a) return;
    if (find_alias(new)) return; // new name exists
    char *newname = strdup(new);
    if (!newname) return;
    free(a->name);
    a->name = newname;
}

void erase_alias(const char *name) {
    alias_t *prev = NULL, *a = head;
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
    alias_t *a = head;
    while (a) {
        printf("%s %s\n", a->name, a->expansion);
        a = a->next;
    }
}

void free_all(void) {
    alias_t *a = head;
    while (a) {
        alias_t *nxt = a->next;
        free(a->name);
        free(a->expansion);
        free(a);
        a = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], a[256], b[256];
        if (sscanf(line, "%9s %255s %255s", cmd, a, b) >= 2) {
            const char *rest = line + strlen(cmd) + 1 + strlen(a) + 1;
            if (strcmp(cmd, "DEFINE") == 0) {
                define_alias(a, rest);
            } else if (strcmp(cmd, "EXPAND") == 0) {
                expand_alias(a);
            } else if (strcmp(cmd, "RENAME") == 0) {
                if (sscanf(line, "%*s %255s %255s", a, b) == 2)
                    rename_alias(a, b);
            } else if (strcmp(cmd, "ERASE") == 0) {
                erase_alias(a);
            } else if (strcmp(cmd, "DUMP") == 0) {
                dump_aliases();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}