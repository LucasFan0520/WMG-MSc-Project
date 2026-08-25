// F003.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *expansion;
} Alias;

static Alias *aliases = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_alias(const char *name) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(aliases[i].name, name) == 0)
            return (ssize_t)i;
    return -1;
}

static void add_alias(const char *name, const char *exp) {
    ssize_t idx = find_alias(name);
    if (idx != -1) {
        free(aliases[idx].expansion);
        aliases[idx].expansion = strdup(exp);
        if (!aliases[idx].expansion) { fprintf(stderr, "Memory error\n"); exit(1); }
        return;
    }
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Alias *new = realloc(aliases, capacity * sizeof(Alias));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        aliases = new;
    }
    aliases[count].name = strdup(name);
    aliases[count].expansion = strdup(exp);
    if (!aliases[count].name || !aliases[count].expansion) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void rename_alias(const char *old, const char *new) {
    ssize_t idx = find_alias(old);
    if (idx == -1) return;
    if (find_alias(new) != -1) return; // name conflict, ignore
    free(aliases[idx].name);
    aliases[idx].name = strdup(new);
    if (!aliases[idx].name) { fprintf(stderr, "Memory error\n"); exit(1); }
}

static void erase_alias(const char *name) {
    ssize_t idx = find_alias(name);
    if (idx == -1) return;
    free(aliases[idx].name);
    free(aliases[idx].expansion);
    for (size_t i = idx; i < count - 1; i++)
        aliases[i] = aliases[i + 1];
    count--;
}

static void expand_alias(const char *name) {
    ssize_t idx = find_alias(name);
    if (idx == -1) return;
    for (char *p = aliases[idx].expansion; *p; p++)
        putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

static void dump(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %s\n", aliases[i].name, aliases[i].expansion);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(aliases[i].name);
        free(aliases[i].expansion);
    }
    free(aliases);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], old[256], new[256], exp[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "DEFINE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", old, exp) == 2)
                add_alias(old, exp);
        } else if (strcmp(cmd, "RENAME") == 0) {
            if (sscanf(line, "%*s %255s %255s", old, new) == 2)
                rename_alias(old, new);
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (sscanf(line, "%*s %255s", old) == 1)
                erase_alias(old);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            if (sscanf(line, "%*s %255s", old) == 1)
                expand_alias(old);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump();
        }
    }

    free(line);
    free_all();
    return 0;
}