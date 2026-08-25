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

void define_alias(const char *name, const char *expansion) {
    Alias *a = find_alias(name);
    if (a) {
        free(a->expansion);
        a->expansion = strdup(expansion);
    } else {
        a = malloc(sizeof(Alias));
        if (!a) return;
        a->name = strdup(name);
        a->expansion = strdup(expansion);
        a->next = head;
        head = a;
    }
}

void expand_alias(const char *name) {
    Alias *a = find_alias(name);
    if (!a) return;
    for (char *p = a->expansion; *p; p++) {
        putchar(*p == ' ' ? '_' : *p);
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
    Alias *prev = NULL;
    Alias *a = head;
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
        printf("%s ", a->name);
        for (char *p = a->expansion; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        a = a->next;
    }
}

void free_all(void) {
    while (head) {
        Alias *next = head->next;
        free(head->name);
        free(head->expansion);
        free(head);
        head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "DEFINE") == 0) {
            char name[256], exp[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", name, exp) == 2) {
                define_alias(name, exp);
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                expand_alias(name);
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char old[256], new[256];
            if (sscanf(line, "%*s %255s %255s", old, new) == 2) {
                rename_alias(old, new);
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char name[256];
            if (sscanf(line, "%*s %255s", name) == 1) {
                erase_alias(name);
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }

    free(line);
    free_all();
    return 0;
}