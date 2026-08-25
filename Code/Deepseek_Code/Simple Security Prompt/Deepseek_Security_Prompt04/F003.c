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
Alias *tail = NULL;

Alias *find_alias(char *name) {
    for (Alias *a = head; a; a = a->next)
        if (strcmp(a->name, name) == 0) return a;
    return NULL;
}

void define_alias(char *name, char *expansion) {
    Alias *a = find_alias(name);
    if (a) {
        free(a->expansion);
        a->expansion = strdup(expansion);
    } else {
        Alias *new = malloc(sizeof(Alias));
        new->name = strdup(name);
        new->expansion = strdup(expansion);
        new->next = NULL;
        if (!tail) head = tail = new;
        else { tail->next = new; tail = new; }
    }
}

void expand_alias(char *name) {
    Alias *a = find_alias(name);
    if (a) {
        for (char *c = a->expansion; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
    }
}

void rename_alias(char *old, char *new) {
    Alias *a = find_alias(old);
    if (a) {
        free(a->name);
        a->name = strdup(new);
    }
}

void erase_alias(char *name) {
    Alias *prev = NULL;
    for (Alias *a = head; a; a = a->next) {
        if (strcmp(a->name, name) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            if (a == tail) tail = prev;
            free(a->name);
            free(a->expansion);
            free(a);
            return;
        }
        prev = a;
    }
}

void dump_all(void) {
    for (Alias *a = head; a; a = a->next) {
        printf("%s %s\n", a->name, a->expansion);
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

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[256];
        int n = sscanf(line, "%15s %255s %255s", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "DEFINE") == 0 && n == 3) {
            define_alias(arg1, arg2);
        } else if (strcmp(cmd, "EXPAND") == 0 && n == 2) {
            expand_alias(arg1);
        } else if (strcmp(cmd, "RENAME") == 0 && n == 3) {
            rename_alias(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0 && n == 2) {
            erase_alias(arg1);
        } else if (strcmp(cmd, "DUMP") == 0 && n == 1) {
            dump_all();
        }
    }
    free(line);
    free_all();
    return 0;
}