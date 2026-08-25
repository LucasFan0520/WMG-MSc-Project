// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *expansion;
} Alias;

int main(void) {
    Alias *items = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;
    long read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
            read--;
        }

        if (strcmp(line, "END") == 0) {
            break;
        }

        char *p = line;
        while (*p == ' ' || *p == '	') p++;
        if (*p == '\0') continue;

        char *cmd = p;
        while (*p && *p != ' ' && *p != '	') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ' || *p == '	') p++;

        if (strcmp(cmd, "DEFINE") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *expansion = p;

            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }

            if (found != -1) {
                free(items[found].expansion);
                items[found].expansion = strdup(expansion);
                if (!items[found].expansion) exit(1);
            } else {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Alias *new_items = realloc(items, capacity * sizeof(Alias));
                    if (!new_items) exit(1);
                    items = new_items;
                }
                items[count].name = strdup(name);
                items[count].expansion = strdup(expansion);
                if (!items[count].name || !items[count].expansion) exit(1);
                count++;
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    char *e = items[i].expansion;
                    for (size_t j = 0; e[j] != '\0'; j++) {
                        if (e[j] == ' ') putchar('_');
                        else putchar(e[j]);
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *old_name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ' || *p == '	') p++;
            char *new_name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, old_name) == 0) {
                    free(items[i].name);
                    items[i].name = strdup(new_name);
                    if (!items[i].name) exit(1);
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *name = p;
            while (*p && *p != ' ' && *p != '	') p++;
            if (*p) *p = '\0';

            for (size_t i = 0; i < count; i++) {
                if (strcmp(items[i].name, name) == 0) {
                    free(items[i].name);
                    free(items[i].expansion);
                    for (size_t j = i; j < count - 1; j++) {
                        items[j] = items[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", items[i].name, items[i].expansion);
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        free(items[i].name);
        free(items[i].expansion);
    }
    free(items);
    free(line);
    return 0;
}
