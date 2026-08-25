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

Alias *find_alias(const char *name) {
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
        return;
    }
    Alias *new_a = malloc(sizeof(Alias));
    if (!new_a) return;
    new_a->name = strdup(name);
    new_a->expansion = strdup(expansion);
    new_a->next = head;
    head = new_a;
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
    if (!a || find_alias(new)) return;
    free(a->name);
    a->name = strdup(new);
}

void erase_alias(const char *name) {
    Alias *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->expansion);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
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
        Alias *tmp = a;
        a = a->next;
        free(tmp->name);
        free(tmp->expansion);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char arg1[512], arg2[512];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "DEFINE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name_end = p;
            while (*name_end && *name_end != ' ') name_end++;
            if (*name_end) {
                *name_end = '\0';
                char *exp = name_end + 1;
                while (*exp == ' ') exp++;
                define_alias(p, exp);
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            expand_alias(p);
        } else if (strcmp(cmd, "RENAME") == 0) {
            if (sscanf(line + 7, "%511s %511s", arg1, arg2) == 2) {
                rename_alias(arg1, arg2);
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            erase_alias(p);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }
    free_all();
    return 0;
}