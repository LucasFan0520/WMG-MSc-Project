// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *exp;
} Alias;

Alias *aliases = NULL;
int alias_count = 0;

int main() {
    char cmd[20];
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "DEFINE") == 0) {
            char name[100], exp[1000];
            scanf("%s", name);
            scanf(" %[^\n]", exp);
            int idx = -1;
            for (int i = 0; i < alias_count; i++) {
                if (strcmp(aliases[i].name, name) == 0) { idx = i; break; }
            }
            if (idx != -1) {
                free(aliases[idx].exp);
                aliases[idx].exp = strdup(exp);
            } else {
                aliases = realloc(aliases, sizeof(Alias) * (alias_count + 1));
                aliases[alias_count].name = strdup(name);
                aliases[alias_count].exp = strdup(exp);
                alias_count++;
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char name[100];
            scanf("%s", name);
            for (int i = 0; i < alias_count; i++) {
                if (strcmp(aliases[i].name, name) == 0) {
                    for (char *p = aliases[i].exp; *p; p++) {
                        if (*p == ' ') putchar('_');
                        else putchar(*p);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char old[100], new_name[100];
            scanf("%s %s", old, new_name);
            for (int i = 0; i < alias_count; i++) {
                if (strcmp(aliases[i].name, old) == 0) {
                    free(aliases[i].name);
                    aliases[i].name = strdup(new_name);
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char name[100];
            scanf("%s", name);
            for (int i = 0; i < alias_count; i++) {
                if (strcmp(aliases[i].name, name) == 0) {
                    free(aliases[i].name);
                    free(aliases[i].exp);
                    for (int j = i; j < alias_count - 1; j++) aliases[j] = aliases[j+1];
                    alias_count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < alias_count; i++) {
                printf("%s %s\n", aliases[i].name, aliases[i].exp);
            }
        }
    }
    for (int i = 0; i < alias_count; i++) {
        free(aliases[i].name);
        free(aliases[i].exp);
    }
    free(aliases);
    return 0;
}