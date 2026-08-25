// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t cap = 16;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(fp)) != EOF && c != '\n') {
        if (len + 1 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) {
                free(buf);
                return NULL;
            }
            buf = nb;
        }
        buf[len++] = c;
    }
    if (len == 0 && c == EOF) {
        free(buf);
        return NULL;
    }
    if (len > 0 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    } else {
        buf[len] = '\0';
    }
    return buf;
}

char *safe_dup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) {
        strcpy(d, s);
    }
    return d;
}

typedef struct {
    char *name;
    char *expansion;
} Alias;

int main(void) {
    Alias *table = NULL;
    size_t count = 0;
    size_t cap = 0;
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
            char *expansion = "";
            if (*p == ' ') {
                *p = '\0';
                expansion = p + 1;
            }
            size_t found = (size_t)-1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != (size_t)-1) {
                free(table[found].expansion);
                table[found].expansion = safe_dup(expansion);
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Alias *nb = realloc(table, cap * sizeof(Alias));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    table = nb;
                }
                table[count].name = safe_dup(name);
                table[count].expansion = safe_dup(expansion);
                count++;
            }
        } else if (strncmp(line, "EXPAND ", 7) == 0) {
            char *name = line + 7;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].name, name) == 0) {
                    char *t = table[i].expansion;
                    while (*t) {
                        if (*t == ' ') {
                            putchar('_');
                        } else {
                            putchar(*t);
                        }
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strncmp(line, "RENAME ", 7) == 0) {
            char *p = line + 7;
            char *old_name = p;
            while (*p && *p != ' ') p++;
            char *new_name = "";
            if (*p == ' ') {
                *p = '\0';
                new_name = p + 1;
            }
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].name, old_name) == 0) {
                    free(table[i].name);
                    table[i].name = safe_dup(new_name);
                    break;
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
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(table[i].name);
        free(table[i].expansion);
    }
    free(table);
    return 0;
}
