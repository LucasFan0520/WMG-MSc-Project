/* F003.c */
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
    Alias *a = aliases;
    while (a) {
        if (strcmp(a->name, name) == 0) return a;
        a = a->next;
    }
    return NULL;
}

void add_alias(const char *name, const char *exp) {
    Alias *a = malloc(sizeof(Alias));
    a->name = strdup(name);
    a->expansion = strdup(exp);
    a->next = aliases;
    aliases = a;
}

void remove_alias(const char *name) {
    Alias *prev = NULL, *cur = aliases;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else aliases = cur->next;
            free(cur->name);
            free(cur->expansion);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void free_aliases(void) {
    while (aliases) {
        Alias *tmp = aliases;
        aliases = aliases->next;
        free(tmp->name);
        free(tmp->expansion);
        free(tmp);
    }
}

char *replace_spaces(const char *s) {
    char *r = strdup(s);
    for (char *p = r; *p; p++) if (*p == ' ') *p = '_';
    return r;
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char name[256], old[256], new[256];
        int n = sscanf(line, "%s", cmd);
        if (n < 1) continue;
        if (strcmp(cmd, "DEFINE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *n = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p++ = '\0';
            while (*p == ' ') p++;
            if (*p) add_alias(n, p);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *n = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            Alias *a = find_alias(n);
            if (a) {
                char *s = replace_spaces(a->expansion);
                printf("%s\n", s);
                free(s);
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *oldn = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p++ = '\0';
            while (*p == ' ') p++;
            char *newn = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            Alias *a = find_alias(oldn);
            if (a) {
                free(a->name);
                a->name = strdup(newn);
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *n = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            remove_alias(n);
        } else if (strcmp(cmd, "DUMP") == 0) {
            Alias *a = aliases;
            while (a) {
                printf("%s %s\n", a->name, a->expansion);
                a = a->next;
            }
        }
    }
    free_aliases();
    return 0;
}