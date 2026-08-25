// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALIASES 1000
#define MAX_LEN 1000

typedef struct {
    char name[100];
    char exp[MAX_LEN];
} Alias;

Alias aliases[MAX_ALIASES];
int count = 0;

int find_alias(const char *name) {
    for (int i = 0; i < count; i++) if (strcmp(aliases[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char cmd[20], n1[100], n2[100], exp[MAX_LEN];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "DEFINE") == 0) {
            scanf("%s", n1);
            fgets(exp, MAX_LEN, stdin);
            exp[strcspn(exp, "\n")] = 0;
            if (exp[0] == ' ') memmove(exp, exp + 1, strlen(exp));
            int idx = find_alias(n1);
            if (idx == -1 && count < MAX_ALIASES) {
                strcpy(aliases[count].name, n1);
                strcpy(aliases[count].exp, exp);
                count++;
            } else if (idx != -1) {
                strcpy(aliases[idx].exp, exp);
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            scanf("%s", n1);
            int idx = find_alias(n1);
            if (idx != -1) {
                for (char *p = aliases[idx].exp; *p; p++) putchar(*p == ' ' ? '_' : *p);
                putchar('\n');
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            scanf("%s %s", n1, n2);
            int idx = find_alias(n1);
            if (idx != -1) strcpy(aliases[idx].name, n2);
        } else if (strcmp(cmd, "ERASE") == 0) {
            scanf("%s", n1);
            int idx = find_alias(n1);
            if (idx != -1) {
                for (int i = idx; i < count - 1; i++) aliases[i] = aliases[i + 1];
                count--;
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < count; i++) printf("%s %s\n", aliases[i].name, aliases[i].exp);
        }
    }
    return 0;
}