/* F003.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *expansion;
} Alias;

static Alias *aliases = NULL;
static size_t acount = 0, acap = 0;

static Alias *find_alias(const char *name) {
    for (size_t i = 0; i < acount; i++) {
        if (strcmp(aliases[i].name, name) == 0)
            return &aliases[i];
    }
    return NULL;
}

static void add_alias(const char *name, const char *exp) {
    if (acount == acap) {
        acap = acap ? acap * 2 : 4;
        aliases = realloc(aliases, acap * sizeof(Alias));
        if (!aliases) { perror("realloc"); exit(1); }
    }
    aliases[acount].name = strdup(name);
    aliases[acount].expansion = strdup(exp);
    acount++;
}

static void remove_alias(const char *name) {
    for (size_t i = 0; i < acount; i++) {
        if (strcmp(aliases[i].name, name) == 0) {
            free(aliases[i].name);
            free(aliases[i].expansion);
            for (size_t j = i; j < acount - 1; j++) {
                aliases[j] = aliases[j + 1];
            }
            acount--;
            return;
        }
    }
}

static void rename_alias(const char *old, const char *new) {
    Alias *a = find_alias(old);
    if (!a) return;
    if (find_alias(new)) return; // new name already exists
    free(a->name);
    a->name = strdup(new);
}

static void expand_alias(const char *name) {
    Alias *a = find_alias(name);
    if (!a) return;
    for (char *c = a->expansion; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

static void dump_aliases(void) {
    for (size_t i = 0; i < acount; i++) {
        printf("%s %s\n", aliases[i].name, aliases[i].expansion);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "DEFINE") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *exp = strtok(NULL, "");
            if (!exp) exp = "";
            if (find_alias(name)) {
                // replace existing
                Alias *a = find_alias(name);
                free(a->expansion);
                a->expansion = strdup(exp);
            } else {
                add_alias(name, exp);
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) expand_alias(name);
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *old = strtok(NULL, " ");
            char *new = strtok(NULL, " ");
            if (old && new) rename_alias(old, new);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) remove_alias(name);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }
    for (size_t i = 0; i < acount; i++) {
        free(aliases[i].name);
        free(aliases[i].expansion);
    }
    free(aliases);
    return 0;
}