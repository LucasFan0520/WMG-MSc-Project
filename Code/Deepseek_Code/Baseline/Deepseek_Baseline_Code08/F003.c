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
    Alias *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_alias(const char *name, const char *exp) {
    Alias *new = malloc(sizeof(Alias));
    new->name = strdup(name);
    new->expansion = strdup(exp);
    new->next = NULL;
    if (!head) head = new;
    else {
        Alias *cur = head;
        while (cur->next) cur = cur->next;
        cur->next = new;
    }
}

void rename_alias(const char *old, const char *new) {
    Alias *a = find_alias(old);
    if (!a) return;
    if (find_alias(new)) return;
    free(a->name);
    a->name = strdup(new);
}

void erase_alias(const char *name) {
    Alias *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->name, name) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->name);
            free(cur->expansion);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void free_all() {
    Alias *cur = head;
    while (cur) {
        Alias *tmp = cur;
        cur = cur->next;
        free(tmp->name);
        free(tmp->expansion);
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "DEFINE") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            char *exp = strtok(NULL, "");
            if (!exp) exp = "";
            if (find_alias(name)) continue;
            add_alias(name, exp);
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *name = strtok(NULL, " ");
            if (!name) continue;
            Alias *a = find_alias(name);
            if (!a) continue;
            char *p = a->expansion;
            while (*p) {
                if (*p == ' ') putchar('_');
                else putchar(*p);
                p++;
            }
            putchar('\n');
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *old = strtok(NULL, " ");
            if (!old) continue;
            char *new = strtok(NULL, " ");
            if (!new) continue;
            rename_alias(old, new);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) erase_alias(name);
        } else if (strcmp(cmd, "DUMP") == 0) {
            Alias *cur = head;
            while (cur) {
                printf("%s %s\n", cur->name, cur->expansion);
                cur = cur->next;
            }
        }
    }
    free_all();
    return 0;
}