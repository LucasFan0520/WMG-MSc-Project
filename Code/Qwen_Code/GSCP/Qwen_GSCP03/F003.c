// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Alias {
    char *name, *exp;
    struct Alias *next;
} Alias;

Alias *head = NULL;

void free_list() {
    while (head) {
        Alias *t = head; head = head->next;
        free(t->name); free(t->exp); free(t);
    }
}

Alias *find_name(const char *name) {
    for (Alias *a = head; a; a = a->next)
        if (strcmp(a->name, name) == 0) return a;
    return NULL;
}

void print_spaces(const char *s) {
    for (; *s; s++) printf("%c", *s == ' ' ? '_' : *s);
    printf("\n");
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], n1[256], n2[256], exp[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "DEFINE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", n1, exp) < 1) continue;
            Alias *a = find_name(n1);
            if (a) { free(a->exp); a->exp = strdup(exp); }
            else {
                a = malloc(sizeof(Alias));
                a->name = strdup(n1); a->exp = strdup(exp); a->next = head; head = a;
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            if (sscanf(line, "%*s %255s", n1) != 1) continue;
            Alias *a = find_name(n1);
            if (a) print_spaces(a->exp);
        } else if (strcmp(cmd, "RENAME") == 0) {
            if (sscanf(line, "%*s %255s %255s", n1, n2) != 2) continue;
            Alias *a = find_name(n1);
            if (a) { free(a->name); a->name = strdup(n2); }
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (sscanf(line, "%*s %255s", n1) != 1) continue;
            Alias *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, n1) == 0) {
                    Alias *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->name); free(tmp->exp); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (Alias *a = head; a; a = a->next) printf("%s %s\n", a->name, a->exp);
        }
    }
    free_list();
    return 0;
}