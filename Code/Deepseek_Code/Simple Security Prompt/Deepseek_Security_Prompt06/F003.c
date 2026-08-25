// F003.c
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

void define_alias(const char *name, const char *exp) {
    Alias *a = find_alias(name);
    if (a) {
        free(a->expansion);
        a->expansion = malloc(strlen(exp) + 1);
        strcpy(a->expansion, exp);
        return;
    }
    Alias *newa = malloc(sizeof(Alias));
    newa->name = malloc(strlen(name) + 1);
    strcpy(newa->name, name);
    newa->expansion = malloc(strlen(exp) + 1);
    strcpy(newa->expansion, exp);
    newa->next = head;
    head = newa;
}

void expand_alias(const char *name) {
    Alias *a = find_alias(name);
    if (!a) return;
    for (char *s = a->expansion; *s; s++) {
        if (*s == ' ') putchar('_');
        else putchar(*s);
    }
    putchar('\n');
}

void rename_alias(const char *old, const char *new) {
    Alias *a = find_alias(old);
    if (!a) return;
    if (find_alias(new)) return;
    free(a->name);
    a->name = malloc(strlen(new) + 1);
    strcpy(a->name, new);
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
        Alias *tmp = a->next;
        free(a->name);
        free(a->expansion);
        free(a);
        a = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char a[256], b[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "DEFINE") == 0) {
            if (sscanf(rest, "%255s", a) == 1) {
                const char *exp = rest + strlen(a) + 1;
                while (*exp == ' ') exp++;
                if (*exp) define_alias(a, exp);
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            if (sscanf(rest, "%255s", a) == 1) expand_alias(a);
        } else if (strcmp(cmd, "RENAME") == 0) {
            if (sscanf(rest, "%255s %255s", a, b) == 2) rename_alias(a, b);
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (sscanf(rest, "%255s", a) == 1) erase_alias(a);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }
    free_all();
    return 0;
}