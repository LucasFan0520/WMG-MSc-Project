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
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[16];
        int read_bytes = 0;
        if (sscanf(line, "%15s%n", cmd, &read_bytes) <= 0) {
            continue;
        }
        char *args = line + read_bytes;
        while (*args == ' ') args++;
        if (strcmp(cmd, "DEFINE") == 0) {
            char name[256];
            int name_bytes = 0;
            if (sscanf(args, "%255s%n", name, &name_bytes) > 0) {
                char *exp = args + name_bytes;
                while (*exp == ' ') exp++;
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
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].name, name) == 0) {
                        for (int j = 0; table[i].expansion[j] != '\0'; j++) {
                            if (table[i].expansion[j] == ' ') printf("_");
                            else printf("%c", table[i].expansion[j]);
                        }
                        printf("\n");
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char old_name[256], new_name[256];
            if (sscanf(args, "%255s %255s", old_name, new_name) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].name, old_name) == 0) {
                        free(table[i].name);
                        table[i].name = strdup(new_name);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char name[256];
            if (sscanf(args, "%255s", name) == 1) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].name, name) == 0) {
                        free(table[i].name);
                        free(table[i].expansion);
                        for (int j = i; j < count - 1; j++) {
                            table[j] = table[j + 1];
                        }
                        count--;
                        table = realloc(table, count * sizeof(Alias));
                        break;
                    }
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
