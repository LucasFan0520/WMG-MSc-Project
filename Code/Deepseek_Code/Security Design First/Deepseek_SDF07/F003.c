// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *name;
    char *expansion;
} Alias;

typedef struct {
    Alias *items;
    size_t count;
    size_t capacity;
} AliasTable;

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len && s[len-1] == '\n') s[len-1] = '\0';
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

static void at_init(AliasTable *t) {
    t->items = NULL;
    t->count = 0;
    t->capacity = 0;
}

static void at_free(AliasTable *t) {
    for (size_t i = 0; i < t->count; i++) {
        free(t->items[i].name);
        free(t->items[i].expansion);
    }
    free(t->items);
    t->items = NULL;
    t->count = 0;
    t->capacity = 0;
}

static int at_find(AliasTable *t, const char *name) {
    for (size_t i = 0; i < t->count; i++) {
        if (strcmp(t->items[i].name, name) == 0)
            return (int)i;
    }
    return -1;
}

static void at_define(AliasTable *t, const char *name, const char *exp) {
    int idx = at_find(t, name);
    if (idx >= 0) {
        char *new = safe_strdup(exp);
        if (!new) return;
        free(t->items[idx].expansion);
        t->items[idx].expansion = new;
        return;
    }
    if (t->count == t->capacity) {
        size_t newcap = t->capacity ? t->capacity * 2 : 4;
        Alias *tmp = realloc(t->items, newcap * sizeof(Alias));
        if (!tmp) return;
        t->items = tmp;
        t->capacity = newcap;
    }
    t->items[t->count].name = safe_strdup(name);
    t->items[t->count].expansion = safe_strdup(exp);
    if (t->items[t->count].name && t->items[t->count].expansion)
        t->count++;
    else {
        free(t->items[t->count].name);
        free(t->items[t->count].expansion);
    }
}

static void at_expand(AliasTable *t, const char *name) {
    int idx = at_find(t, name);
    if (idx < 0) return;
    const char *p = t->items[idx].expansion;
    while (*p) {
        putchar(*p == ' ' ? '_' : *p);
        p++;
    }
    putchar('\n');
}

static void at_rename(AliasTable *t, const char *old, const char *new) {
    int idx = at_find(t, old);
    if (idx < 0) return;
    if (at_find(t, new) >= 0) return; // new name exists
    char *dup = safe_strdup(new);
    if (!dup) return;
    free(t->items[idx].name);
    t->items[idx].name = dup;
}

static void at_erase(AliasTable *t, const char *name) {
    int idx = at_find(t, name);
    if (idx < 0) return;
    free(t->items[idx].name);
    free(t->items[idx].expansion);
    for (size_t i = idx; i < t->count - 1; i++)
        t->items[i] = t->items[i+1];
    t->count--;
}

static void at_dump(AliasTable *t) {
    for (size_t i = 0; i < t->count; i++) {
        printf("%s: ", t->items[i].name);
        const char *p = t->items[i].expansion;
        while (*p) {
            putchar(*p == ' ' ? '_' : *p);
            p++;
        }
        putchar('\n');
    }
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) cmd[i++] = *p++;
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a1sz - 1) a1[i++] = *p++;
    a1[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
}

int main(void) {
    AliasTable t;
    at_init(&t);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "DEFINE") == 0) {
            at_define(&t, a1, a2);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            at_expand(&t, a1);
        } else if (strcmp(cmd, "RENAME") == 0) {
            at_rename(&t, a1, a2);
        } else if (strcmp(cmd, "ERASE") == 0) {
            at_erase(&t, a1);
        } else if (strcmp(cmd, "DUMP") == 0) {
            at_dump(&t);
        }
    }
    at_free(&t);
    return 0;
}