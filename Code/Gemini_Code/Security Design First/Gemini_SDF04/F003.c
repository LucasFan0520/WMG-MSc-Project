// F003.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *expansion;
} Alias;

Alias *aliases = NULL;
int count = 0;
int capacity = 0;

int find_alias(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(aliases[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "DEFINE") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *exp = space2 + 1;
                    int idx = find_alias(arg1);
                    if (idx != -1) {
                        char *new_exp = strdup(exp);
                        if (new_exp) {
                            free(aliases[idx].expansion);
                            aliases[idx].expansion = new_exp;
                        }
                    } else {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            Alias *tmp = realloc(aliases, capacity * sizeof(Alias));
                            if (!tmp) {
                                free(line);
                                return 1;
                            }
                            aliases = tmp;
                        }
                        aliases[count].name = strdup(arg1);
                        aliases[count].expansion = strdup(exp);
                        if (aliases[count].name && aliases[count].expansion) {
                            count++;
                        }
                    }
                }
            } else if (strcmp(cmd, "EXPAND") == 0) {
                int idx = find_alias(arg1);
                if (idx != -1) {
                    char *p = aliases[idx].expansion;
                    for (size_t i = 0; p[i] != '\0'; i++) {
                        if (p[i] == ' ') putchar('_');
                        else putchar(p[i]);
                    }
                    putchar('\n');
                }
            } else if (strcmp(cmd, "RENAME") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *arg2 = space2 + 1;
                    int idx = find_alias(arg1);
                    if (idx != -1) {
                        char *new_name = strdup(arg2);
                        if (new_name) {
                            free(aliases[idx].name);
                            aliases[idx].name = new_name;
                        }
                    }
                }
            } else if (strcmp(cmd, "ERASE") == 0) {
                int idx = find_alias(arg1);
                if (idx != -1) {
                    free(aliases[idx].name);
                    free(aliases[idx].expansion);
                    for (int i = idx; i < count - 1; i++) {
                        aliases[i] = aliases[i + 1];
                    }
                    count--;
                }
            }
        } else {
            if (strcmp(cmd, "DUMP") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", aliases[i].name, aliases[i].expansion);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
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