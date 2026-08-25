// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *exp; } Alias;
Alias *aliases = NULL;
int alias_count = 0, alias_cap = 0;

int find_alias(const char *name) {
    for (int i = 0; i < alias_count; i++)
        if (strcmp(aliases[i].name, name) == 0) return i;
    return -1;
}

void define_alias(const char *name, const char *exp) {
    int idx = find_alias(name);
    if (idx >= 0) {
        free(aliases[idx].exp);
        aliases[idx].exp = strdup(exp);
    } else {
        if (alias_count == alias_cap) {
            alias_cap = alias_cap ? alias_cap * 2 : 4;
            aliases = realloc(aliases, sizeof(Alias) * alias_cap);
        }
        aliases[alias_count].name = strdup(name);
        aliases[alias_count].exp = strdup(exp);
        alias_count++;
    }
}

void expand_alias(const char *name) {
    int idx = find_alias(name);
    if (idx < 0) return;
    for (char *p = aliases[idx].exp; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

void rename_alias(const char *old, const char *new) {
    int idx = find_alias(old);
    if (idx < 0) return;
    free(aliases[idx].name);
    aliases[idx].name = strdup(new);
}

void erase_alias(const char *name) {
    int idx = find_alias(name);
    if (idx < 0) return;
    free(aliases[idx].name);
    free(aliases[idx].exp);
    for (int i = idx; i < alias_count - 1; i++) aliases[i] = aliases[i + 1];
    alias_count--;
}

void dump_aliases(void) {
    for (int i = 0; i < alias_count; i++)
        printf("%s %s\n", aliases[i].name, aliases[i].exp);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "DEFINE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, arg2) == 2)
                define_alias(arg1, arg2);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) expand_alias(arg1);
        } else if (strcmp(cmd, "RENAME") == 0) {
            if (sscanf(line, "%*s %255s %255s", arg1, arg2) == 2)
                rename_alias(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) erase_alias(arg1);
        } else if (strcmp(cmd, "DUMP") == 0) dump_aliases();
    }
    for (int i = 0; i < alias_count; i++) { free(aliases[i].name); free(aliases[i].exp); }
    free(aliases);
    return 0;
}