/* F003.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void free_alias(Alias *a) {
    free(a->name);
    free(a->expansion);
    free(a);
}

void define_alias(const char *name, const char *expansion) {
    Alias *a = find_alias(name);
    if (a) {
        free(a->expansion);
        a->expansion = strdup(expansion);
        return;
    }
    a = malloc(sizeof(Alias));
    a->name = strdup(name);
    a->expansion = strdup(expansion);
    a->next = aliases;
    aliases = a;
}

void expand_alias(const char *name) {
    Alias *a = find_alias(name);
    if (!a) {
        printf("NOT_FOUND\n");
        return;
    }
    for (char *c = a->expansion; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
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
    Alias *prev = NULL, *curr = aliases;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else aliases = curr->next;
            free_alias(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void dump_aliases(void) {
    Alias *a = aliases;
    while (a) {
        printf("%s %s\n", a->name, a->expansion);
        a = a->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char name[256], newname[256], expansion[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "DEFINE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) != 1) continue;
            p += strlen(name);
            while (*p == ' ') p++;
            define_alias(name, p);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) expand_alias(name);
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s %255s", name, newname) == 2) rename_alias(name, newname);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", name) == 1) erase_alias(name);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }

    while (aliases) {
        Alias *tmp = aliases;
        aliases = aliases->next;
        free_alias(tmp);
    }
    return 0;
}