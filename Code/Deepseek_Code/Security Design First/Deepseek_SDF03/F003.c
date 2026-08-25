// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_NAME 64
#define MAX_EXP 4096

typedef struct Alias {
    char name[MAX_NAME];
    char exp[MAX_EXP];
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

void free_all() {
    Alias *a = head;
    while (a) {
        Alias *tmp = a;
        a = a->next;
        free(tmp);
    }
    head = NULL;
}

void handle_define(char *line) {
    char name[MAX_NAME], exp[MAX_EXP];
    int n = sscanf(line, "DEFINE %63s %4095[^\n]", name, exp);
    if (n < 2) return;
    Alias *a = find_alias(name);
    if (a) {
        strcpy(a->exp, exp);
    } else {
        Alias *newa = malloc(sizeof(Alias));
        if (!newa) return;
        strcpy(newa->name, name);
        strcpy(newa->exp, exp);
        newa->next = head;
        head = newa;
    }
}

void handle_expand(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "EXPAND %63s", name) != 1) return;
    Alias *a = find_alias(name);
    if (!a) return;
    for (char *c = a->exp; *c; c++) {
        if (*c == ' ') putchar('_');
        else putchar(*c);
    }
    putchar('\n');
}

void handle_rename(char *line) {
    char old[MAX_NAME], new[MAX_NAME];
    if (sscanf(line, "RENAME %63s %63s", old, new) != 2) return;
    Alias *a = find_alias(old);
    if (!a) return;
    if (find_alias(new)) return;
    strcpy(a->name, new);
}

void handle_erase(char *line) {
    char name[MAX_NAME];
    if (sscanf(line, "ERASE %63s", name) != 1) return;
    Alias *a = head, *prev = NULL;
    while (a) {
        if (strcmp(a->name, name) == 0) {
            if (prev) prev->next = a->next;
            else head = a->next;
            free(a);
            return;
        }
        prev = a;
        a = a->next;
    }
}

void handle_dump() {
    Alias *a = head;
    while (a) {
        printf("%s %s\n", a->name, a->exp);
        a = a->next;
    }
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "DEFINE ", 7) == 0) handle_define(line);
        else if (strncmp(line, "EXPAND ", 7) == 0) handle_expand(line);
        else if (strncmp(line, "RENAME ", 7) == 0) handle_rename(line);
        else if (strncmp(line, "ERASE ", 6) == 0) handle_erase(line);
        else if (strcmp(line, "DUMP") == 0) handle_dump();
    }
    free_all();
    return 0;
}