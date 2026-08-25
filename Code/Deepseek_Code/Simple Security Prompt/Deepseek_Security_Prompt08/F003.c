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
    } else {
        Alias *new = malloc(sizeof(Alias));
        new->name = malloc(strlen(name) + 1);
        strcpy(new->name, name);
        new->expansion = malloc(strlen(exp) + 1);
        strcpy(new->expansion, exp);
        new->next = head;
        head = new;
    }
}

void expand_alias(const char *name) {
    Alias *a = find_alias(name);
    if (!a) return;
    for (char *c = a->expansion; *c; c++) {
        putchar(*c == ' ' ? '_' : *c);
    }
    putchar('\n');
}

void rename_alias(const char *old, const char *new) {
    Alias *a = find_alias(old);
    if (!a) return;
    free(a->name);
    a->name = malloc(strlen(new) + 1);
    strcpy(a->name, new);
}

void erase_alias(const char *name) {
    Alias *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->name);
            free(curr->expansion);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
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
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "DEFINE") == 0) {
            char *name = strtok(NULL, " ");
            char *exp = strtok(NULL, "");
            if (name && exp) define_alias(name, exp);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) expand_alias(name);
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *old = strtok(NULL, " ");
            char *new = strtok(NULL, " ");
            if (old && new) rename_alias(old, new);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) erase_alias(name);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }
    free(line);
    free_all();
    return 0;
}