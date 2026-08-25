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

int find_alias(const char *name) {
    for (int i = 0; i < count; i++)
        if (strcmp(aliases[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], arg1[1000], arg2[1000];
        if (sscanf(line, "%s", cmd) != 1) continue;
        if (strcmp(cmd, "DEFINE") == 0) {
            sscanf(line, "%*s %s", arg1);
            char *exp = line + 7 + strlen(arg1);
            if (*exp == ' ') exp++;
            int i = find_alias(arg1);
            if (i != -1) {
                free(aliases[i].exp);
                aliases[i].exp = strdup(exp);
            } else {
                aliases = realloc(aliases, sizeof(Alias) * (count + 1));
                aliases[count].name = strdup(arg1);
                aliases[count].exp = strdup(exp);
                count++;
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_alias(arg1);
            if (i != -1) {
                for (int j = 0; aliases[i].exp[j]; j++)
                    putchar(aliases[i].exp[j] == ' ' ? '_' : aliases[i].exp[j]);
                putchar('\n');
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            sscanf(line, "%*s %s %s", arg1, arg2);
            int i = find_alias(arg1);
            if (i != -1) {
                free(aliases[i].name);
                aliases[i].name = strdup(arg2);
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            sscanf(line, "%*s %s", arg1);
            int i = find_alias(arg1);
            if (i != -1) {
                free(aliases[i].name);
                free(aliases[i].exp);
                for (int j = i; j < count - 1; j++) aliases[j] = aliases[j + 1];
                count--;
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < count; i++) printf("%s %s\n", aliases[i].name, aliases[i].exp);
        }
    }
    for (int i = 0; i < count; i++) {
        free(aliases[i].name);
        free(aliases[i].exp);
    }
    free(aliases);
    return 0;
}