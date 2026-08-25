// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name, *exp; } Alias;
Alias *aliases = 0; int n = 0, cap = 0;

int find(char *name) {
    for (int i=0; i<n; i++) if (!strcmp(aliases[i].name, name)) return i;
    return -1;
}

int main() {
    char line[2048];
    while (fgets(line, sizeof line, stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (!strcmp(line, "END")) break;
        char cmd[20], a[256], rest[1700];
        rest[0] = 0;
        sscanf(line, "%s", cmd);
        if (!strcmp(cmd, "DEFINE") || !strcmp(cmd, "RENAME")) sscanf(line, "%*s %s %[^\n]", a, rest);
        else sscanf(line, "%*s %s", a);

        if (!strcmp(cmd, "DEFINE")) {
            int i = find(a);
            if (i >= 0) { free(aliases[i].exp); aliases[i].exp = strdup(rest); }
            else {
                if (n == cap) { cap = cap ? cap*2 : 4; aliases = realloc(aliases, cap*sizeof(Alias)); }
                aliases[n].name = strdup(a); aliases[n].exp = strdup(rest); n++;
            }
        } else if (!strcmp(cmd, "EXPAND")) {
            int i = find(a);
            if (i >= 0) { for (char *x = aliases[i].exp; *x; x++) putchar(*x == ' ' ? '_' : *x); putchar('\n'); }
        } else if (!strcmp(cmd, "RENAME")) {
            int i = find(a);
            if (i >= 0) { free(aliases[i].name); aliases[i].name = strdup(rest); }
        } else if (!strcmp(cmd, "ERASE")) {
            int i = find(a);
            if (i >= 0) {
                free(aliases[i].name); free(aliases[i].exp);
                for (int j=i; j<n-1; j++) aliases[j] = aliases[j+1]; n--;
            }
        } else if (!strcmp(cmd, "DUMP")) {
            for (int i=0; i<n; i++) printf("%s %s\n", aliases[i].name, aliases[i].exp);
        }
    }
    return 0;
}