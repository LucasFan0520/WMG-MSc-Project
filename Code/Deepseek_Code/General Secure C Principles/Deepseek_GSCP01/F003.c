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
    } else {
        a = malloc(sizeof(Alias));
        a->name = strdup(name);
        a->expansion = strdup(expansion);
        a->next = head;
        head = a;
    }
}

void expand_alias(const char *name) {
    Alias *a = find_alias(name);
    if (!a) { printf("NOT_FOUND\n"); return; }
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
    Alias *a = head, *prev = NULL;
    while (a && strcmp(a->name, name) != 0) {
        prev = a;
        a = a->next;
    }
    if (!a) return;
    if (prev) prev->next = a->next;
    else head = a->next;
    free(a->name);
    free(a->expansion);
    free(a);
}

void dump_aliases(void) {
    Alias *a = head;
    while (a) {
        printf("%s ", a->name);
        for (char *c = a->expansion; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
        a = a->next;
    }
}

void free_all(void) {
    Alias *a = head;
    while (a) {
        Alias *next = a->next;
        free(a->name);
        free(a->expansion);
        free(a);
        a = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "DEFINE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p = '\0';
            p++;
            while (*p == ' ') p++;
            define_alias(name, p);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            expand_alias(p);
        } else if (strcmp(cmd, "RENAME") == 0) {
            char old[256], new[256];
            if (sscanf(line + 7, "%255s %255s", old, new) == 2)
                rename_alias(old, new);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            erase_alias(p);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }
    free_all();
    free(line);
    return 0;
}