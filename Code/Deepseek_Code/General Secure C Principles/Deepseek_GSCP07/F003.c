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

Alias* find_alias(const char *name) {
    Alias *a = head;
    while (a) {
        if (strcmp(a->name, name) == 0) return a;
        a = a->next;
    }
    return NULL;
}

void free_alias(Alias *a) {
    free(a->name);
    free(a->expansion);
    free(a);
}

void handle_define(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    char *exp = strtok(NULL, "");
    if (!exp) exp = "";
    if (find_alias(name)) return;
    Alias *new = malloc(sizeof(Alias));
    new->name = strdup(name);
    new->expansion = strdup(exp);
    new->next = NULL;
    if (!head) head = new;
    else {
        Alias *last = head;
        while (last->next) last = last->next;
        last->next = new;
    }
}

void handle_expand(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Alias *a = find_alias(name);
    if (!a) return;
    for (char *s = a->expansion; *s; s++) {
        putchar(*s == ' ' ? '_' : *s);
    }
    putchar('\n');
}

void handle_rename(char *cmd) {
    char *old = strtok(NULL, " ");
    char *new = strtok(NULL, " ");
    if (!old || !new) return;
    Alias *a = find_alias(old);
    if (!a || find_alias(new)) return;
    free(a->name);
    a->name = strdup(new);
}

void handle_erase(char *cmd) {
    char *name = strtok(NULL, " ");
    if (!name) return;
    Alias *a = head, *prev = NULL;
    while (a) {
        if (strcmp(a->name, name) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            free_alias(a);
            return;
        }
        prev = a;
        a = a->next;
    }
}

void dump() {
    Alias *a = head;
    while (a) {
        printf("%s %s\n", a->name, a->expansion);
        a = a->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "DEFINE") == 0) handle_define(cmd);
        else if (strcmp(cmd, "EXPAND") == 0) handle_expand(cmd);
        else if (strcmp(cmd, "RENAME") == 0) handle_rename(cmd);
        else if (strcmp(cmd, "ERASE") == 0) handle_erase(cmd);
        else if (strcmp(cmd, "DUMP") == 0) dump();
    }
    while (head) {
        Alias *tmp = head;
        head = head->next;
        free_alias(tmp);
    }
    return 0;
}