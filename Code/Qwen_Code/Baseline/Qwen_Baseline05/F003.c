// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_EXP 1000

typedef struct {
    char name[MAX_NAME];
    char expansion[MAX_EXP];
} Alias;

Alias aliases[1000];
int count = 0;

int find_alias(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(aliases[i].name, name) == 0) return i;
    }
    return -1;
}

void print_with_underscores(const char *text) {
    for (int i = 0; text[i]; i++) {
        if (text[i] == ' ') putchar('_');
        else putchar(text[i]);
    }
    putchar('\n');
}

int main() {
    char cmd[20], name[MAX_NAME], old_name[MAX_NAME], new_name[MAX_NAME], exp[MAX_EXP];
    
    while (scanf("%s", cmd) == 1) {
        if (strcmp(cmd, "END") == 0) break;
        
        if (strcmp(cmd, "DEFINE") == 0) {
            scanf("%s", name);
            fgets(exp, MAX_EXP, stdin);
            if (exp[0] == ' ') memmove(exp, exp + 1, strlen(exp));
            if (exp[strlen(exp) - 1] == '\n') exp[strlen(exp) - 1] = '\0';
            
            int idx = find_alias(name);
            if (idx == -1) {
                strcpy(aliases[count].name, name);
                strcpy(aliases[count].expansion, exp);
                count++;
            } else {
                strcpy(aliases[idx].expansion, exp);
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            scanf("%s", name);
            int idx = find_alias(name);
            if (idx != -1) {
                print_with_underscores(aliases[idx].expansion);
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            scanf("%s %s", old_name, new_name);
            int idx = find_alias(old_name);
            if (idx != -1) {
                strcpy(aliases[idx].name, new_name);
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            scanf("%s", name);
            int idx = find_alias(name);
            if (idx != -1) {
                for (int i = idx; i < count - 1; i++) {
                    aliases[i] = aliases[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", aliases[i].name, aliases[i].expansion);
            }
        }
    }
    
    return 0;
}