// F003.c
#define _GNU_SOURCE
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
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0') continue;
        char *cmd = p;
        while (*p && *p != ' ' && *p != '\t') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "DEFINE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *expansion = p;
            if (*name == '\0') continue;

            int found_idx = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].name, name) == 0) {
                    found_idx = (int)i;
                    break;
                }
            }
            if (found_idx != -1) {
                char *exp = strdup(expansion);
                if (exp) {
                    free(table[found_idx].expansion);
                    table[found_idx].expansion = exp;
                }
            } else {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Alias *new_table = realloc(table, new_cap * sizeof(Alias));
                    if (!new_table) continue;
                    table = new_table;
                    capacity = new_cap;
                }
                char *n = strdup(name);
                char *e = strdup(expansion);
                if (n && e) {
                    table[count].name = n;
                    table[count].expansion = e;
                    count++;
                } else {
                    free(n);
                    free(e);
                }
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].name, name) == 0) {
                    for (size_t j = 0; table[i].expansion[j] != '\0'; j++) {
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
        } else if (strcmp(cmd, "RENAME") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *old = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '\t') p++;
            char *new_name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';

            if (*old == '\0' || *new_name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].name, old) == 0) {
                    char *nn = strdup(new_name);
                    if (nn) {
                        free(table[i].name);
                        table[i].name = nn;
                    }
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            while (*p == ' ' || *p == '\t') p++;
            char *name = p;
            while (*p && *p != ' ' && *p != '\t') p++;
            if (*p) *p = '\0';
            if (*name == '\0') continue;

            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].name, name) == 0) {
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

    for (size_t i = 0; i < count; i++) {
        free(table[i].name);
        free(table[i].expansion);
    }
    free(table);
    free(line);
    return 0;
}
