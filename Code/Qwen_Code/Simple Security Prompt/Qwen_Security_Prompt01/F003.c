// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Alias {
    char *name;
    char *exp;
    struct Alias *next;
} Alias;

Alias *head = NULL;

void define_alias(const char *n, const char *e) {
    for (Alias *a = head; a; a = a->next) {
        if (strcmp(a->name, n) == 0) {
            free(a->exp);
            a->exp = strdup(e);
            return;
        }
    }
    Alias *a = malloc(sizeof(Alias));
    a->name = strdup(n);
    a->exp = strdup(e);
    a->next = head;
    head = a;
}

void expand_alias(const char *n) {
    for (Alias *a = head; a; a = a->next) {
        if (strcmp(a->name, n) == 0) {
            for (char *c = a->exp; *c; c++) {
                if (*c == ' ') {
                    putchar('_');
                } else {
                    putchar(*c);
                }
            }
            putchar('\n');
            return;
        }
    }
}

void rename_alias(const char *o, const char *n) {
    for (Alias *a = head; a; a = a->next) {
        if (strcmp(a->name, o) == 0) {
            free(a->name);
            a->name = strdup(n);
            return;
        }
    }
}

void erase_alias(const char *n) {
    Alias *p = head;
    Alias *prev = NULL;
    while (p) {
        if (strcmp(p->name, n) == 0) {
            Alias *next = p->next;
            free(p->name);
            free(p->exp);
            free(p);
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }
            return;
        }
        prev = p;
        p = p->next;
    }
}

void dump_alias() {
    for (Alias *a = head; a; a = a->next) {
        printf("%s %s\n", a->name, a->exp);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char a[1000];
        char b[9000];
        b[0] = 0;
        sscanf(line, "%s %s %[^\n]", cmd, a, b);
        if (strcmp(cmd, "DEFINE") == 0) {
            define_alias(a, b);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            expand_alias(a);
        } else if (strcmp(cmd, "RENAME") == 0) {
            char c[1000];
            sscanf(line, "%*s %*s %s", c);
            rename_alias(a, c);
        } else if (strcmp(cmd, "ERASE") == 0) {
            erase_alias(a);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_alias();
        }
    }
    while (head) {
        Alias *n = head->next;
        free(head->name);
        free(head->exp);
        free(head);
        head = n;
    }
    return 0;
}