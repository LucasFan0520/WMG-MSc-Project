// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Alias {
    char *name;
    char *expansion;
    struct Alias *next;
} Alias;

Alias *aliases = NULL;

Alias* find_alias(const char *name) {
    for (Alias *a = aliases; a; a = a->next)
        if (strcmp(a->name, name) == 0) return a;
    return NULL;
}

void print_text(const char *text) {
    for (const char *p = text; *p; p++) putchar(*p == ' ' ? '_' : *p);
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "DEFINE") == 0) {
            char name[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", name, &n) != 1) continue;
            char *exp = rest + n;
            while (*exp == ' ') exp++;
            Alias *a = find_alias(name);
            if (a) {
                free(a->expansion);
                a->expansion = strdup(exp);
            } else {
                a = malloc(sizeof(Alias));
                if (!a) continue;
                a->name = strdup(name);
                a->expansion = strdup(exp);
                a->next = aliases;
                aliases = a;
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Alias *a = find_alias(name);
            if (a) print_text(a->expansion);
        } else if (strcmp(cmd, "RENAME") == 0) {
            char old_name[256], new_name[256];
            if (sscanf(rest, "%255s %255s", old_name, new_name) != 2) continue;
            Alias *a = find_alias(old_name);
            if (a && !find_alias(new_name)) {
                free(a->name);
                a->name = strdup(new_name);
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char name[256];
            if (sscanf(rest, "%255s", name) != 1) continue;
            Alias *prev = NULL;
            for (Alias *a = aliases; a; prev = a, a = a->next) {
                if (strcmp(a->name, name) == 0) {
                    if (prev) prev->next = a->next;
                    else aliases = a->next;
                    free(a->name);
                    free(a->expansion);
                    free(a);
                    break;
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (Alias *a = aliases; a; a = a->next) {
                printf("%s ", a->name);
                print_text(a->expansion);
            }
        }
    }
    while (aliases) {
        Alias *a = aliases;
        aliases = aliases->next;
        free(a->name);
        free(a->expansion);
        free(a);
    }
    return 0;
}