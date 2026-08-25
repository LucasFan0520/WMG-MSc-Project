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

void define_alias(const char *name, const char *exp) {
    for (Alias *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) {
            free(curr->expansion);
            curr->expansion = strdup(exp);
            return;
        }
    }
    Alias *a = malloc(sizeof(Alias));
    if (!a) return;
    a->name = strdup(name);
    a->expansion = strdup(exp);
    a->next = head;
    head = a;
}

void expand_alias(const char *name) {
    for (Alias *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->name, name) == 0) {
            for (size_t i = 0; i < strlen(curr->expansion); i++) {
                if (curr->expansion[i] == ' ') putchar('_');
                else putchar(curr->expansion[i]);
            }
            putchar('\n');
            return;
        }
    }
}

void rename_alias(const char *old_name, const char *new_name) {
    for (Alias *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->name, old_name) == 0) {
            free(curr->name);
            curr->name = strdup(new_name);
            return;
        }
    }
}

void erase_alias(const char *name) {
    Alias *curr = head;
    Alias *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            Alias *next = curr->next;
            free(curr->name);
            free(curr->expansion);
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void dump_aliases() {
    for (Alias *curr = head; curr; curr = curr->next) {
        printf("%s %s\n", curr->name, curr->expansion);
    }
}

void free_all() {
    Alias *curr = head;
    while (curr) {
        Alias *next = curr->next;
        free(curr->name);
        free(curr->expansion);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[16];
        char arg1[1024], arg2[4096];
        
        int n = sscanf(line, "%15s %1023s %4095[^\n]", cmd, arg1, arg2);
        if (strcmp(cmd, "DEFINE") == 0 && n == 3) {
            define_alias(arg1, arg2);
        } else if (strcmp(cmd, "EXPAND") == 0 && n >= 2) {
            expand_alias(arg1);
        } else if (strcmp(cmd, "RENAME") == 0) {
            sscanf(line, "%*s %1023s %1023s", arg1, arg2);
            rename_alias(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0 && n >= 2) {
            erase_alias(arg1);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_aliases();
        }
    }
    free_all();
    return 0;
}