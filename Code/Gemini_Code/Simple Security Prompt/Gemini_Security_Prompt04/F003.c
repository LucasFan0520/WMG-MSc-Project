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
    buf[len] = '\0';
    if (len > 0 && buf[len-1] == '\r') {
        buf[len-1] = '\0';
    }
    return buf;
}

typedef struct {
    char *name;
    char *expansion;
} Alias;

int main(void) {
    Alias *aliases = NULL;
    int count = 0;
    int cap = 0;
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
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *exp = p + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(aliases[i].name, name) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    free(aliases[found].expansion);
                    aliases[found].expansion = strdup(exp);
                } else {
                    if (count >= cap) {
                        cap = cap == 0 ? 16 : cap * 2;
                        aliases = realloc(aliases, cap * sizeof(Alias));
                    }
                    aliases[count].name = strdup(name);
                    aliases[count].expansion = strdup(exp);
                    count++;
                }
            }
        } else if (strncmp(line, "EXPAND ", 7) == 0) {
            char *name = line + 7;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(aliases[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *e = aliases[found].expansion;
                while (*e) {
                    if (*e == ' ') putchar('_');
                    else putchar(*e);
                    e++;
                }
                putchar('\n');
            }
        } else if (strncmp(line, "RENAME ", 7) == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *old = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                char *newname = p + 1;
                while (*newname == ' ') newname++;
                for (int i = 0; i < count; i++) {
                    if (strcmp(aliases[i].name, old) == 0) {
                        free(aliases[i].name);
                        aliases[i].name = strdup(newname);
                        break;
                    }
                }
            }
        } else if (strncmp(line, "ERASE ", 6) == 0) {
            char *name = line + 6;
            while (*name == ' ') name++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(aliases[i].name, name) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(aliases[found].name);
                free(aliases[found].expansion);
                for (int i = found; i < count - 1; i++) {
                    aliases[i] = aliases[i + 1];
                }
                count--;
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
