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
    Alias *aliases = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "DEFINE ", 7) == 0) {
            char *p = line + 7;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *name = p;
                char *exp = space + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(aliases[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    char *new_exp = strdup(exp);
                    if (new_exp) {
                        free(aliases[found].expansion);
                        aliases[found].expansion = new_exp;
                    }
                } else {
                    Alias *tmp = realloc(aliases, sizeof(Alias) * (count + 1));
                    if (tmp) {
                        aliases = tmp;
                        aliases[count].name = strdup(name);
                        aliases[count].expansion = strdup(exp);
                        if (aliases[count].name && aliases[count].expansion) {
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "EXPAND ", 7) == 0) {
            char *name = line + 7;
            for (int i = 0; i < count; i++) {
                if (strcmp(aliases[i].name, name) == 0) {
                    char *t = aliases[i].expansion;
                    while (*t) {
                        if (*t == ' ') putchar('_');
                        else putchar(*t);
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strncmp(line, "RENAME ", 7) == 0) {
            char *p = line + 7;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *old_name = p;
                char *new_name = space + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(aliases[i].name, old_name) == 0) {
                        char *n = strdup(new_name);
                        if (n) {
                            free(aliases[i].name);
                            aliases[i].name = n;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *name = line + 6;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(aliases[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(aliases[found].name);
                free(aliases[found].expansion);
                for (int i = found; i < count - 1; i++) {
                    aliases[i] = aliases[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "DUMP") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", aliases[i].name, aliases[i].expansion);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(aliases[i].name);
        free(aliases[i].expansion);
    }
    free(aliases);
    free(line);
    return 0;
}
