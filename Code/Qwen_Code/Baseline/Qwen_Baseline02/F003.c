// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *name; char *exp; } Alias;
Alias a[1000]; int na = 0;

int find(char *name) {
    for (int i = 0; i < na; i++) if (!strcmp(a[i].name, name)) return i;
    return -1;
}

int main() {
    char cmd[20], n1[100], n2[100], exp[10000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "DEFINE")) {
            scanf("%s", n1); fgets(exp, sizeof(exp), stdin);
            exp[strcspn(exp, "\n")] = 0;
            if (exp[0] == ' ') memmove(exp, exp + 1, strlen(exp));
            int idx = find(n1);
            if (idx != -1) { free(a[idx].exp); a[idx].exp = strdup(exp); }
            else { a[na].name = strdup(n1); a[na++].exp = strdup(exp); }
        } else if (!strcmp(cmd, "EXPAND")) {
            scanf("%s", n1); int idx = find(n1);
            if (idx != -1) {
                for (char *c = a[idx].exp; *c; c++) printf("%c", *c == ' ' ? '_' : *c);
                printf("\n");
            }
        } else if (!strcmp(cmd, "RENAME")) {
            scanf("%s %s", n1, n2); int idx = find(n1);
            if (idx != -1) { free(a[idx].name); a[idx].name = strdup(n2); }
        } else if (!strcmp(cmd, "ERASE")) {
            scanf("%s", n1); int idx = find(n1);
            if (idx != -1) {
                free(a[idx].name); free(a[idx].exp);
                for (int i = idx; i < na - 1; i++) a[i] = a[i + 1];
                na--;
            }
        } else if (!strcmp(cmd, "DUMP")) {
            for (int i = 0; i < na; i++) printf("%s %s\n", a[i].name, a[i].exp);
        }
    }
    for (int i = 0; i < na; i++) { free(a[i].name); free(a[i].exp); }
    return 0;
}