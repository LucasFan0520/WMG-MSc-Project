// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *stream) {
    size_t capacity = 16;
    size_t len = 0;
    char *str = malloc(capacity);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(stream)) != EOF && c != '\n') {
        if (len + 1 >= capacity) {
            capacity *= 2;
            char *new_str = realloc(str, capacity);
            if (!new_str) {
                free(str);
                return NULL;
            }
            str = new_str;
        }
        str[len++] = (char)c;
    }
    if (len == 0 && c == EOF) {
        free(str);
        return NULL;
    }
    if (len > 0 && str[len - 1] == '\n') {
        len--;
    }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *name;
    char *expansion;
} Alias;

int main(void) {
    Alias *aliases = NULL;
    int count = 0;
    int capacity = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "DEFINE ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *name_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *exp_start = p + 1;
                while (*exp_start == ' ') exp_start++;
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(aliases[i].name, name_start) == 0) {
                        free(aliases[i].expansion);
                        aliases[i].expansion = strdup(exp_start);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    if (count >= capacity) {
                        capacity = capacity == 0 ? 4 : capacity * 2;
                        Alias *new_a = realloc(aliases, capacity * sizeof(Alias));
                        if (new_a) aliases = new_a;
                    }
                    if (count < capacity) {
                        aliases[count].name = strdup(name_start);
                        aliases[count].expansion = strdup(exp_start);
                        count++;
                    }
                }
            }
        } else if (strncmp(line, "EXPAND ", 7) == 0) {
            char *name_start = line + 7;
            while (*name_start == ' ') name_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(aliases[i].name, name_start) == 0) {
                    char *e = aliases[i].expansion;
                    while (*e) {
                        if (*e == ' ') putchar('_');
                        else putchar(*e);
                        e++;
                    }
                    putchar('\n');
                    break;
                }
            }
        } else if (strncmp(line, "RENAME ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *old_start = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *new_start = p + 1;
                while (*new_start == ' ') new_start++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(aliases[i].name, old_start) == 0) {
                        free(aliases[i].name);
                        aliases[i].name = strdup(new_start);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *name_start = line + 6;
            while (*name_start == ' ') name_start++;
            for (int i = 0; i < count; i++) {
                if (strcmp(aliases[i].name, name_start) == 0) {
                    free(aliases[i].name);
                    free(aliases[i].expansion);
                    for (int j = i; j < count - 1; j++) {
                        aliases[j] = aliases[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(line, "DUMP") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", aliases[i].name, aliases[i].expansion);
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
