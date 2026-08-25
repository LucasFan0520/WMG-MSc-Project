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
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "DEFINE ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *name = args;
                char *expansion = space + 1;
                int found = -1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(table[i].name, name) == 0) {
                        found = (int)i;
                        break;
                    }
                }
                if (found != -1) {
                    free(table[found].expansion);
                    table[found].expansion = strdup(expansion);
                } else {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Alias *next = realloc(table, capacity * sizeof(Alias));
                        if (!next) break;
                        table = next;
                    }
                    table[count].name = strdup(name);
                    table[count].expansion = strdup(expansion);
                    count++;
                }
            }
        } else if (strncmp(line, "EXPAND ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].name, name) == 0) {
                    char *e = table[i].expansion;
                    for (size_t k = 0; e[k] != '\0'; k++) {
                        if (e[k] == ' ') putchar('_');
                        else putchar(e[k]);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strncmp(line, "RENAME ", 7) == 0) {
            char *args = line + 7;
            char *space = strchr(args, ' ');
            if (space) {
                *space = '\0';
                char *old_name = args;
                char *new_name = space + 1;
                for (size_t i = 0; i < count; i++) {
                    if (strcmp(table[i].name, old_name) == 0) {
                        free(table[i].name);
                        table[i].name = strdup(new_name);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *name = line + 6;
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
        } else if (strcmp(line, "DUMP") == 0) {
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
