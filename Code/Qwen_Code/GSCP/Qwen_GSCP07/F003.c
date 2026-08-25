// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Alias { char *name, *exp; struct Alias *next; } Alias;
Alias *head = NULL;

void define_alias(const char *name, const char *exp) {
    for (Alias *a = head; a; a = a->next) {
        if (strcmp(a->name, name) == 0) {
            free(a->exp);
            a->exp = strdup(exp);
            return;
        }
    }
    Alias *a = malloc(sizeof(Alias));
    a->name = strdup(name);
    a->exp = strdup(exp);
    a->next = head;
    head = a;
}

void expand_alias(const char *name) {
    for (Alias *a = head; a; a = a->next) {
        if (strcmp(a->name, name) == 0) {
            for (char *c = a->exp; *c; c++) putchar(*c == ' ' ? '_' : *c);
            putchar('\n');
            return;
        }
    }
}

void rename_alias(const char *old, const char *new_name) {
    for (Alias *a = head; a; a = a->next) {
        if (strcmp(a->name, old) == 0) {
            free(a->name);
            a->name = strdup(new_name);
            return;
        }
    }
}

void erase_alias(const char *name) {
    Alias **p = &head;
    while (*p) {
        if (strcmp((*p)->name, name) == 0) {
            Alias *tmp = *p;
            *p = tmp->next;
            free(tmp->name);
            free(tmp->exp);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void dump() {
    for (Alias *a = head; a; a = a->next) printf("%s %s\n", a->name, a->exp);
}

void cleanup() {
    while (head) {
        Alias *tmp = head->next;
        free(head->name);
        free(head->exp);
        free(head);
        head = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "DEFINE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) define_alias(a1, t + 1); }
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) expand_alias(a1);
        } else if (strcmp(cmd, "RENAME") == 0) {
            if (sscanf(line, "%*s %255s %255s", a1, a2) == 2) rename_alias(a1, a2);
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) erase_alias(a1);
        } else if (strcmp(cmd, "DUMP") == 0) dump();
    }
    cleanup();
    return 0;
}