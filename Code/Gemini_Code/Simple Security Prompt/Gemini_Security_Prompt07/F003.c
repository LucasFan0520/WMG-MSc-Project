// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *expansion;
} Alias;

int main() {
    Alias *table = NULL;
    int count = 0;
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "DEFINE") == 0) {
            char *name = strtok(NULL, " ");
            char *exp = strtok(NULL, "");
            if (name && exp) {
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].name, name) == 0) {
                        free(table[i].expansion);
                        table[i].expansion = strdup(exp);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    table = realloc(table, (count + 1) * sizeof(Alias));
                    table[count].name = strdup(name);
                    table[count].expansion = strdup(exp);
                    count++;
                }
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].name, name) == 0) {
                        char *e = table[i].expansion;
                        for (int j = 0; e[j]; j++) {
                            if (e[j] == ' ') {
                                putchar('_');
                            } else {
                                putchar(e[j]);
                            }
                        }
                        putchar('\n');
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *old_name = strtok(NULL, " ");
            char *new_name = strtok(NULL, " ");
            if (old_name && new_name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].name, old_name) == 0) {
                        free(table[i].name);
                        table[i].name = strdup(new_name);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *name = strtok(NULL, " ");
            if (name) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].name, name) == 0) {
                        free(table[i].name);
                        free(table[i].expansion);
                        for (int j = i; j < count - 1; j++) {
                            table[j] = table[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(table);
                    table = NULL;
                } else {
                    table = realloc(table, count * sizeof(Alias));
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", table[i].name, table[i].expansion);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(table[i].name);
        free(table[i].expansion);
    }
    free(table);
    return 0;
}
