// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *exp;
} Alias;

Alias al[1000];
int ac = 0;

int find_name(char *name) {
    for (int i = 0; i < ac; i++) if (strcmp(al[i].name, name) == 0) return i;
    return -1;
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20];
        sscanf(line, "%s", cmd);
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "DEFINE") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            char *exp = rest + strlen(name);
            while (*exp == ' ') exp++;
            int idx = find_name(name);
            if (idx == -1) {
                al[ac].name = strdup(name);
                al[ac].exp = strdup(exp);
                ac++;
            } else {
                free(al[idx].exp);
                al[idx].exp = strdup(exp);
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                for (char *p = al[idx].exp; *p; p++) {
                    if (*p == ' ') putchar('_');
                    else putchar(*p);
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char old[1000], new_name[1000];
            sscanf(rest, "%s %s", old, new_name);
            int idx = find_name(old);
            if (idx != -1) {
                free(al[idx].name);
                al[idx].name = strdup(new_name);
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char name[1000];
            sscanf(rest, "%s", name);
            int idx = find_name(name);
            if (idx != -1) {
                free(al[idx].name);
                free(al[idx].exp);
                for (int i = idx; i < ac - 1; i++) al[i] = al[i+1];
                ac--;
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < ac; i++) printf("%s %s\n", al[i].name, al[i].exp);
        }
    }
    for (int i = 0; i < ac; i++) {
        free(al[i].name);
        free(al[i].exp);
    }
    return 0;
}