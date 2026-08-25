// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *fp) {
    size_t size = 32;
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

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (!d) return NULL;
    strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *expansion;
} Alias;

int main(void) {
    Alias *table = NULL;
    size_t count = 0;
    size_t capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') {
            free(line);
            continue;
        }
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p != '\0') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "DEFINE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            char *exp = name_end;
            if (*name_end != '\0') {
                *name_end = '\0';
                exp++;
            }
            while (*exp == ' ') exp++;
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (table[i].name && strcmp(table[i].name, name) == 0) {
                    free(table[i].expansion);
                    table[i].expansion = mystrdup(exp);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    capacity = capacity == 0 ? 4 : capacity * 2;
                    Alias *new_table = realloc(table, capacity * sizeof(Alias));
                    if (!new_table) {
                        free(line);
                        break;
                    }
                    table = new_table;
                }
                table[count].name = mystrdup(name);
                table[count].expansion = mystrdup(exp);
                count++;
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (table[i].name && strcmp(table[i].name, name) == 0) {
                    char *t = table[i].expansion;
                    while (*t) {
                        if (*t == ' ') putchar('_');
                        else putchar(*t);
                        t++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *old_n = p;
            while (*old_n == ' ') old_n++;
            char *old_end = old_n;
            while (*old_end && *old_end != ' ') old_end++;
            char *new_n = old_end;
            if (*old_end != '\0') {
                *old_end = '\0';
                new_n++;
            }
            while (*new_n == ' ') new_n++;
            char *new_end = new_n;
            while (*new_end && *new_end != ' ') new_end++;
            *new_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (table[i].name && strcmp(table[i].name, old_n) == 0) {
                    free(table[i].name);
                    table[i].name = mystrdup(new_n);
                    break;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *name = p;
            while (*name == ' ') name++;
            char *name_end = name;
            while (*name_end && *name_end != ' ') name_end++;
            *name_end = '\0';
            for (size_t i = 0; i < count; i++) {
                if (table[i].name && strcmp(table[i].name, name) == 0) {
                    free(table[i].name);
                    free(table[i].expansion);
                    table[i].name = NULL;
                    table[i].expansion = NULL;
                    break;
                }
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (table[i].name) {
                    printf("%s %s\n", table[i].name, table[i].expansion);
                }
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        if (table[i].name) free(table[i].name);
        if (table[i].expansion) free(table[i].expansion);
    }
    free(table);
    return 0;
}
