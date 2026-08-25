// F003.c
#define _GNU_SOURCE
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
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "DEFINE") == 0 && args) {
            char *name = args;
            char *space2 = strchr(args, ' ');
            char *expansion = "";
            if (space2) {
                *space2 = '\0';
                expansion = space2 + 1;
                while (*expansion == ' ') expansion++;
            }
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].name, name) == 0) {
                    free(table[i].expansion);
                    table[i].expansion = strdup(expansion);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                table = realloc(table, (count + 1) * sizeof(Alias));
                if (!table) return 1;
                table[count].name = strdup(name);
                table[count].expansion = strdup(expansion);
                count++;
            }
        } else if (strcmp(cmd, "EXPAND") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].name, args) == 0) {
                    for (int j = 0; table[i].expansion[j] != '\0'; j++) {
                        if (table[i].expansion[j] == ' ') {
                            putchar('_');
                        } else {
                            putchar(table[i].expansion[j]);
                        }
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0 && args) {
            char *old_name = args;
            char *space2 = strchr(args, ' ');
            if (space2) {
                *space2 = '\0';
                char *new_name = space2 + 1;
                while (*new_name == ' ') new_name++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].name, old_name) == 0) {
                        free(table[i].name);
                        table[i].name = strdup(new_name);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].name, args) == 0) {
                    free(table[i].name);
                    free(table[i].expansion);
                    for (int j = i; j < count - 1; j++) {
                        table[j] = table[j + 1];
                    }
                    count--;
                    table = realloc(table, count * sizeof(Alias));
                    if (count > 0 && !table) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", table[i].name, table[i].expansion);
            }
        }
    }

    free(line);
    for (int i = 0; i < count; i++) {
        free(table[i].name);
        free(table[i].expansion);
    }
    free(table);
    return 0;
}
