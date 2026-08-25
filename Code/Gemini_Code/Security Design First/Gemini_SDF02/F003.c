/* F003.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *expansion;
} Alias;

int main(void) {
    Alias *table = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        while (*p == ' ') p++;
        char *arg1 = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ') p++;
        char *arg2 = p;

        if (strcmp(cmd, "DEFINE") == 0) {
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].name, arg1) == 0) {
                    char *temp = strdup(arg2);
                    if (temp) {
                        free(table[i].expansion);
                        table[i].expansion = temp;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Alias *temp = realloc(table, new_cap * sizeof(Alias));
                    if (!temp) break;
                    table = temp;
                    capacity = new_cap;
                }
                table[count].name = strdup(arg1);
                table[count].expansion = strdup(arg2);
                if (table[count].name && table[count].expansion) {
                    count++;
                }
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].name, arg1) == 0) {
                    char *t = table[i].expansion;
                    while (*t) {
                        if (*t == ' ') {
                            putchar('_');
                        } else {
                            putchar(*t);
                        }
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].name, arg1) == 0) {
                    char *temp = strdup(arg2);
                    if (temp) {
                        free(table[i].name);
                        table[i].name = temp;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].name, arg1) == 0) {
                    free(table[i].name);
                    free(table[i].expansion);
                    for (size_t j = i; j < count - 1; j++) {
                        table[j] = table[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", table[i].name, table[i].expansion);
            }
        }
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(table[i].name);
        free(table[i].expansion);
    }
    free(table);
    return 0;
}
