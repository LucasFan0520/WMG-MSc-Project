/* F003.c */
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
        len--;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t l = strlen(s);
    char *d = malloc(l + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *name;
    char *exp;
} Alias;

int main(void) {
    Alias *table = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        if (*p == '\0') {
            free(line);
            continue;
        }
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p == ' ') {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        if (strcmp(cmd, "DEFINE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *exp = p;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                char *ne = mystrdup(exp);
                if (ne) {
                    free(table[found].exp);
                    table[found].exp = ne;
                }
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Alias *nt = realloc(table, cap * sizeof(Alias));
                    if (nt) table = nt;
                }
                char *nn = mystrdup(name);
                char *ne = mystrdup(exp);
                if (nn && ne) {
                    table[count].name = nn;
                    table[count].exp = ne;
                    count++;
                } else {
                    free(nn);
                    free(ne);
                }
            }
        } else if (strcmp(cmd, "EXPAND") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                char *t = table[found].exp;
                while (*t) {
                    if (*t == ' ') putchar('_');
                    else putchar(*t);
                    t++;
                }
                putchar('\n');
            }
        } else if (strcmp(cmd, "RENAME") == 0) {
            while (*p == ' ') p++;
            char *old = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *newname = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].name, old) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                char *nn = mystrdup(newname);
                if (nn) {
                    free(table[found].name);
                    table[found].name = nn;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            while (*p == ' ') p++;
            char *name = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].name, name) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(table[found].name);
                free(table[found].exp);
                for (size_t i = found; i < count - 1; i++) {
                    table[i] = table[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", table[i].name, table[i].exp);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(table[i].name);
        free(table[i].exp);
    }
    free(table);
    return 0;
}
