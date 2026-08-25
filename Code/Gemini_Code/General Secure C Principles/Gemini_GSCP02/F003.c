// F003.c
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
    char *line = NULL;
    size_t cap = 0;
    ssize_t read;
    while ((read = getline(&line, &cap, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "DEFINE ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') {
                name++;
            }
            char *sp = strchr(name, ' ');
            char *exp = "";
            if (sp) {
                *sp = '\0';
                exp = sp + 1;
                while (*exp == ' ') {
                    exp++;
                }
            }
            int idx = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].name, name) == 0) {
                    idx = i;
                    break;
                }
            }
            if (idx != -1) {
                char *nexp = strdup(exp);
                if (nexp) {
                    free(table[idx].expansion);
                    table[idx].expansion = nexp;
                }
            } else {
                Alias *tmp = realloc(table, (count + 1) * sizeof(Alias));
                if (tmp) {
                    table = tmp;
                    table[count].name = strdup(name);
                    table[count].expansion = strdup(exp);
                    count++;
                }
            }
        } else if (strncmp(line, "EXPAND ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') {
                name++;
            }
            char *found = NULL;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].name, name) == 0) {
                    found = table[i].expansion;
                    break;
                }
            }
            if (found) {
                for (int k = 0; found[k]; k++) {
                    putchar(found[k] == ' ' ? '_' : found[k]);
                }
                putchar('\n');
            }
        } else if (strncmp(line, "RENAME ", 7) == 0) {
            char old_n[256], new_n[256];
            if (sscanf(line + 7, "%255s %255s", old_n, new_n) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].name, old_n) == 0) {
                        char *nn = strdup(new_n);
                        if (nn) {
                            free(table[i].name);
                            table[i].name = nn;
                        }
                        break;
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *name = line + 6;
            while (*name == ' ') {
                name++;
            }
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].name, name) == 0) {
                    free(table[i].name);
                    free(table[i].expansion);
                    for (int j = i; j < count - 1; j++) {
                        table[j] = table[j + 1];
                    }
                    count--;
                    break;
                }
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
