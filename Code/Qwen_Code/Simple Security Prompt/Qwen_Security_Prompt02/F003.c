// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *exp;
} Alias;

Alias *aliases = NULL;
int count = 0;

int find_name(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(aliases[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void show_exp(const char *e) {
    for (int i = 0; e[i]; i++) {
        if (e[i] == ' ') {
            putchar('_');
        } else {
            putchar(e[i]);
        }
    }
    putchar('\n');
}

int main() {
    char cmd[20];
    char n1[1000];
    char n2[1000];
    char exp[10000];

    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "DEFINE") == 0) {
            scanf("%s", n1);
            fgets(exp, sizeof(exp), stdin);
            if (exp[0] == ' ') {
                memmove(exp, exp + 1, strlen(exp));
            }
            exp[strcspn(exp, "\n")] = 0;
            int idx = find_name(n1);
            if (idx != -1) {
                free(aliases[idx].exp);
                aliases[idx].exp = strdup(exp);
            } else {
                aliases = realloc(aliases, sizeof(Alias) * (count + 1));
                aliases[count].name = strdup(n1);
                aliases[count].exp = strdup(exp);
                count++;
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            scanf("%s", n1);
            int idx = find_name(n1);
            if (idx != -1) {
                show_exp(aliases[idx].exp);
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            scanf("%s %s", n1, n2);
            int idx = find_name(n1);
            if (idx != -1) {
                free(aliases[idx].name);
                aliases[idx].name = strdup(n2);
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            scanf("%s", n1);
            int idx = find_name(n1);
            if (idx != -1) {
                free(aliases[idx].name);
                free(aliases[idx].exp);
                for (int i = idx; i < count - 1; i++) {
                    aliases[i] = aliases[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", aliases[i].name, aliases[i].exp);
            }
        }
    }

    for (int i = 0; i < count; i++) {
        free(aliases[i].name);
        free(aliases[i].exp);
    }
    free(aliases);

    return 0;
}