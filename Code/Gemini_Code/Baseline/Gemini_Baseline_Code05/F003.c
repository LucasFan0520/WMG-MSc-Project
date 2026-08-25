// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *expansion;
} Alias;

Alias *table = NULL;
int count = 0;
int capacity = 0;

int find_alias(const char *name) {
    for (int i = 0; i < count; i++) {
        if (strcmp(table[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void define_alias(const char *name, const char *expansion) {
    int idx = find_alias(name);
    if (idx != -1) {
        free(table[idx].expansion);
        table[idx].expansion = strdup(expansion);
    } else {
        if (count >= capacity) {
            capacity = capacity == 0 ? 4 : capacity * 2;
            table = realloc(table, capacity * sizeof(Alias));
        }
        table[count].name = strdup(name);
        table[count].expansion = strdup(expansion);
        count++;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "DEFINE ", 7) == 0) {
            char *p1 = line + 7;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                define_alias(p1, p2 + 1);
            }
        } else if (strncmp(line, "EXPAND ", 7) == 0) {
            char *name = line + 7;
            int idx = find_alias(name);
            if (idx != -1) {
                char *e = table[idx].expansion;
                while (*e) {
                    if (*e == ' ') {
                        putchar('_');
                    } else {
                        putchar(*e);
                    }
                    e++;
                }
                putchar('\n');
            }
        } else if (strncmp(line, "RENAME ", 7) == 0) {
            char *p1 = line + 7;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                int idx = find_alias(p1);
                if (idx != -1) {
                    free(table[idx].name);
                    table[idx].name = strdup(p2 + 1);
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *name = line + 6;
            int idx = find_alias(name);
            if (idx != -1) {
                free(table[idx].name);
                free(table[idx].expansion);
                for (int i = idx; i < count - 1; i++) {
                    table[i] = table[i + 1];
                }
                count--;
            }
        } else if (strcmp(line, "DUMP") == 0) {
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
