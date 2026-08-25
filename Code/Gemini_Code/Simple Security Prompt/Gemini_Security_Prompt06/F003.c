// F003.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *name;
    char *expansion;
} Alias;

char *read_line(FILE *f) {
    size_t size = 32;
    size_t len = 0;
    char *buf = malloc(size);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

int main() {
    Alias *list = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        char *p1 = strchr(line, ' ');
        if (!p1) {
            if (strcmp(line, "DUMP") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", list[i].name, list[i].expansion);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *cmd = line;
        char *rest = p1 + 1;
        if (strcmp(cmd, "DEFINE") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *name = rest;
                char *exp = p2 + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(list[found].expansion);
                    list[found].expansion = strdup(exp);
                } else {
                    list = realloc(list, (count + 1) * sizeof(Alias));
                    list[count].name = strdup(name);
                    list[count].expansion = strdup(exp);
                    count++;
                }
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            char *name = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                for (int i = 0; list[found].expansion[i] != '\0'; i++) {
                    if (list[found].expansion[i] == ' ') {
                        putchar('_');
                    } else {
                        putchar(list[found].expansion[i]);
                    }
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            char *p2 = strchr(rest, ' ');
            if (p2) {
                *p2 = '\0';
                char *old_name = rest;
                char *new_name = p2 + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(list[i].name, old_name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(list[found].name);
                    list[found].name = strdup(new_name);
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *name = rest;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(list[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(list[found].name);
                free(list[found].expansion);
                for (int i = found; i < count - 1; i++) {
                    list[i] = list[i + 1];
                }
                count--;
                if (count == 0) {
                    free(list);
                    list = NULL;
                } else {
                    list = realloc(list, count * sizeof(Alias));
                }
            }
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(list[i].name);
        free(list[i].expansion);
    }
    free(list);
    return 0;
}