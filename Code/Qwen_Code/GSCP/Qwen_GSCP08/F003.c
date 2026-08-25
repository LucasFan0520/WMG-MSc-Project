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

Alias *find_alias(char *name) {
    for (Alias *a = head; a; a = a->next) {
        if (strcmp(a->name, name) == 0) return a;
    }
    return NULL;
}

void define_alias(char *name, char *exp) {
    Alias *a = find_alias(name);
    if (!a) {
        a = malloc(sizeof(Alias));
        a->name = strdup(name);
        a->next = head;
        head = a;
    } else {
        free(a->expansion);
    }
    a->expansion = strdup(exp);
}

void expand_alias(char *name) {
    Alias *a = find_alias(name);
    if (a) {
        for (char *c = a->expansion; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
    }
}

void rename_alias(char *old_name, char *new_name) {
    Alias *a = find_alias(old_name);
    if (a) {
        free(a->name);
        a->name = strdup(new_name);
    }
}

void erase_alias(char *name) {
    Alias *prev = NULL;
    Alias *curr = head;
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

void dump_aliases() {
    for (Alias *a = head; a; a = a->next) {
        for (char *c = a->expansion; *c; c++) {
            putchar(*c == ' ' ? '_' : *c);
        }
        putchar('\n');
    }
}

int main() {
    char cmd[20], name[100], text[1000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "DEFINE") == 0) {
            scanf("%99s", name);
            fgets(text, sizeof(text), stdin);
            while (text[0] == ' ') memmove(text, text + 1, strlen(text));
            text[strcspn(text, "\n")] = 0;
            define_alias(name, text);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            scanf("%99s", name);
            expand_alias(name);
        } else if (strcmp(cmd, "RENAME") == 0) {
            char new_name[100];
            scanf("%99s%99s", name, new_name);
            rename_alias(name, new_name);
        } else if (strcmp(cmd, "ERASE") == 0) {
            scanf("%99s", name);
            erase_alias(name);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }
    return 0;
}