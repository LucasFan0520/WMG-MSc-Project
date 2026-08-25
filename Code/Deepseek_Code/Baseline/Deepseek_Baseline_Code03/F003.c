// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct alias {
    char *name;
    char *expansion;
    struct alias *next;
};

struct alias *aliases = NULL;

struct alias *find_alias(char *name) {
    struct alias *a = aliases;
    while (a) {
        if (strcmp(a->name, name) == 0) return a;
        a = a->next;
    }
    return NULL;
}

void define_alias(char *name, char *exp) {
    struct alias *a = find_alias(name);
    if (a) {
        free(a->expansion);
        a->expansion = malloc(strlen(exp) + 1);
        strcpy(a->expansion, exp);
    } else {
        a = malloc(sizeof(struct alias));
        a->name = malloc(strlen(name) + 1);
        strcpy(a->name, name);
        a->expansion = malloc(strlen(exp) + 1);
        strcpy(a->expansion, exp);
        a->next = aliases;
        aliases = a;
    }
}

void expand_alias(char *name) {
    struct alias *a = find_alias(name);
    if (a) {
        for (char *c = a->expansion; *c; c++) {
            if (*c == ' ') putchar('_');
            else putchar(*c);
        }
        putchar('\n');
    }
}

void rename_alias(char *old, char *new) {
    struct alias *a = find_alias(old);
    if (!a) return;
    if (find_alias(new)) return; // name conflict
    free(a->name);
    a->name = malloc(strlen(new) + 1);
    strcpy(a->name, new);
}

void erase_alias(char *name) {
    struct alias *a = aliases, *prev = NULL;
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

void dump_aliases() {
    struct alias *a = aliases;
    while (a) {
        printf("%s %s\n", a->name, a->expansion);
        a = a->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[1024];
        int n = sscanf(line, "%31s %255s %1023s", cmd, arg1, arg2);
        if (strcmp(cmd, "DEFINE") == 0 && n >= 3) {
            char *rest = line + strlen(cmd) + 1 + strlen(arg1) + 1;
            define_alias(arg1, rest);
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
    return 0;
}