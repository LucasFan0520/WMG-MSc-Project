// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 128;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_buf = realloc(buf, size);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

typedef struct {
    char *name;
    char *expansion;
} Alias;

int main() {
    Alias *table = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "DEFINE ", 7) == 0) {
            char *p = line + 7;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *exp = p + 1;
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].name, name) == 0) {
                        free(table[i].expansion);
                        table[i].expansion = strdup(exp);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    table = realloc(table, (count + 1) * sizeof(Alias));
                    table[count].name = strdup(name);
                    table[count].expansion = strdup(exp);
                    count++;
                }
            }
        } else if (strncmp(line, "EXPAND ", 7) == 0) {
            char *name = line + 7;
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].name, name) == 0) {
                    char *e = table[i].expansion;
                    for (int j = 0; e[j] != '\0'; j++) {
                        if (e[j] == ' ') printf("_");
                        else printf("%c", e[j]);
                    }
                    printf("\n");
                    found = 1;
                    break;
                }
            }
            if (!found) printf("NOT_FOUND\n");
        } else if (strncmp(line, "RENAME ", 7) == 0) {
            char old_name[128], new_name[128];
            if (sscanf(line + 7, "%127s %127s", old_name, new_name) == 2) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(table[i].name, old_name) == 0) {
                        free(table[i].name);
                        table[i].name = strdup(new_name);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *name = line + 6;
            for (int i = 0; i < count; i++) {
                if (strcmp(table[i].name, name) == 0) {
                    free(table[i].name);
                    free(table[i].expansion);
                    for (int j = i; j < count - 1; j++) {
                        table[j] = table[j + 1];
                    }
                    count--;
                    table = realloc(table, count * sizeof(Alias));
                    break;
                }
            }
        } else if (strcmp(line, "DUMP") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", table[i].name, table[i].expansion);
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(table[i].name);
        free(table[i].expansion);
    }
    free(table);
    return 0;
}
