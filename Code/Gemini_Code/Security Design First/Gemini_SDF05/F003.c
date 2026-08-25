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
    int count = 0;
    int capacity = 0;
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
            char *name_start = line + 7;
            while (*name_start == ' ') name_start++;
            char *space = strchr(name_start, ' ');
            char *exp_start = "";
            if (space) {
                *space = '\0';
                exp_start = space + 1;
                while (*exp_start == ' ') exp_start++;
            }
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].name, name_start) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *ne = strdup(exp_start);
                if (!ne) {
                    free(line);
                    for(int i=0; i<count; i++) { free(table[i].name); free(table[i].expansion); }
                    free(table);
                    return 1;
                }
                free(table[found].expansion);
                table[found].expansion = ne;
            } else {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Alias *tmp = realloc(table, new_cap * sizeof(Alias));
                    if (!tmp) {
                        free(line);
                        for(int i=0; i<count; i++) { free(table[i].name); free(table[i].expansion); }
                        free(table);
                        return 1;
                    }
                    table = tmp;
                    capacity = new_cap;
                }
                char *nn = strdup(name_start);
                char *ne = strdup(exp_start);
                if (!nn || !ne) {
                    free(nn); free(ne); free(line);
                    for(int i=0; i<count; i++) { free(table[i].name); free(table[i].expansion); }
                    free(table);
                    return 1;
                }
                table[count].name = nn;
                table[count].expansion = ne;
                count++;
            }
        } else if (strncmp(line, "EXPAND ", 7) == 0) {
            char *name_start = line + 7;
            while (*name_start == ' ') name_start++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].name, name_start) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *e = table[found].expansion;
                while (*e) {
                    if (*e == ' ') putchar('_');
                    else putchar(*e);
                    e++;
                }
                putchar('\n');
            } else {
                printf("NOT_FOUND\n");
            }
        } else if (strncmp(line, "RENAME ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *old_name = p;
            char *space = strchr(old_name, ' ');
            if (!space) continue;
            *space = '\0';
            char *new_name = space + 1;
            while (*new_name == ' ') new_name++;
            
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].name, old_name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *nn = strdup(new_name);
                if (!nn) {
                    free(line);
                    for(int i=0; i<count; i++) { free(table[i].name); free(table[i].expansion); }
                    free(table);
                    return 1;
                }
                free(table[found].name);
                table[found].name = nn;
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *name_start = line + 6;
            while (*name_start == ' ') name_start++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].name, name_start) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(table[found].name);
                free(table[found].expansion);
                for (int i = found; i < count - 1; i++) {
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
    free(line);
    for (int i = 0; i < count; i++) {
        free(table[i].name);
        free(table[i].expansion);
    }
    free(table);
    return 0;
}
