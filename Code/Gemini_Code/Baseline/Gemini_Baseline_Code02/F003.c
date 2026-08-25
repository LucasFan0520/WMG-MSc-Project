// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    char *expansion;
} Alias;

int main() {
    Alias *aliases = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "DUMP") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", aliases[i].name, aliases[i].expansion);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "DEFINE") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = args;
                char *exp = p2 + 1;
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(aliases[i].name, name) == 0) {
                        free(aliases[i].expansion);
                        aliases[i].expansion = strdup(exp);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    aliases = realloc(aliases, sizeof(Alias) * (count + 1));
                    aliases[count].name = strdup(name);
                    aliases[count].expansion = strdup(exp);
                    count++;
                }
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *name = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(aliases[i].name, name) == 0) {
                    for (int j = 0; aliases[i].expansion[j]; j++) {
                        if (aliases[i].expansion[j] == ' ') printf("_");
                        else printf("%c", aliases[i].expansion[j]);
                    }
                    printf("\n");
                    break;
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *old_name = args;
                char *new_name = p2 + 1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(aliases[i].name, old_name) == 0) {
                        free(aliases[i].name);
                        aliases[i].name = strdup(new_name);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *name = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(aliases[i].name, name) == 0) {
                    free(aliases[i].name);
                    free(aliases[i].expansion);
                    for (int j = i; j < count - 1; j++) {
                        aliases[j] = aliases[j + 1];
                    }
                    count--;
                    if (count == 0) { free(aliases); aliases = NULL; }
                    else { aliases = realloc(aliases, sizeof(Alias) * count); }
                    break;
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(aliases[i].name);
        free(aliases[i].expansion);
    }
    free(aliases);
    return 0;
}
