/* F003.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *expansion;
} Alias;

int main(void) {
    Alias *aliases = NULL;
    int count = 0;
    int capacity = 0;
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
            char *name = NULL;
            char *exp = NULL;
            if (space) {
                *space = '\0';
                name = args;
                exp = space + 1;
            } else {
                name = args;
                exp = "";
            }
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(aliases[i].name, name) == 0) {
                    char *new_exp = strdup(exp);
                    if (new_exp) {
                        free(aliases[i].expansion);
                        aliases[i].expansion = new_exp;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                char *n_name = strdup(name);
                char *n_exp = strdup(exp);
                if (n_name && n_exp) {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        Alias *tmp = realloc(aliases, new_cap * sizeof(Alias));
                        if (tmp) {
                            aliases = tmp;
                            capacity = new_cap;
                        } else {
                            free(n_name);
                            free(n_exp);
                            if (space) *space = ' ';
                            continue;
                        }
                    }
                    aliases[count].name = n_name;
                    aliases[count].expansion = n_exp;
                    count++;
                } else {
                    free(n_name);
                    free(n_exp);
                }
            }
            if (space) *space = ' ';
        } else if (strncmp(line, "EXPAND ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(aliases[i].name, name) == 0) {
                    char *p = aliases[i].expansion;
                    while (*p) {
                        if (*p == ' ') putchar('_');
                        else putchar(*p);
                        p++;
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
                for (int i = 0; i < count; i++) {
                    if (strcmp(aliases[i].name, old_name) == 0) {
                        char *n_name = strdup(new_name);
                        if (n_name) {
                            free(aliases[i].name);
                            aliases[i].name = n_name;
                        }
                        break;
                    }
                }
                *space = ' ';
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *name = line + 6;
            for (int i = 0; i < count; i++) {
                if (strcmp(aliases[i].name, name) == 0) {
                    free(aliases[i].name);
                    free(aliases[i].expansion);
                    for (int j = i; j < count - 1; j++) {
                        aliases[j] = aliases[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "DUMP") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", aliases[i].name, aliases[i].expansion);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(aliases[i].name);
        free(aliases[i].expansion);
    }
    free(aliases);
    return 0;
}
