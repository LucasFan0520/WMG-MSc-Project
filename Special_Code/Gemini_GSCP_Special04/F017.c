/* F017.c */
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
    char *key;
    char *value;
} Config;

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    Config *table = NULL;
    size_t count = 0;
    size_t cap = 0;
    FILE *fp = fopen(argv[1], "r");
    if (fp) {
        while (1) {
            char *line = read_line(fp);
            if (!line) break;
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *key = line;
                char *value = eq + 1;
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Config *nt = realloc(table, cap * sizeof(Config));
                    if (nt) table = nt;
                }
                char *nk = mystrdup(key);
                char *nv = mystrdup(value);
                if (nk && nv) {
                    table[count].key = nk;
                    table[count].value = nv;
                    count++;
                } else {
                    free(nk);
                    free(nv);
                }
            }
            free(line);
        }
        fclose(fp);
    }
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
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *value = p;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                char *nv = mystrdup(value);
                if (nv) {
                    free(table[found].value);
                    table[found].value = nv;
                }
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Config *nt = realloc(table, cap * sizeof(Config));
                    if (nt) table = nt;
                }
                char *nk = mystrdup(key);
                char *nv = mystrdup(value);
                if (nk && nv) {
                    table[count].key = nk;
                    table[count].value = nv;
                    count++;
                } else {
                    free(nk);
                    free(nv);
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(table[found].key);
                free(table[found].value);
                for (size_t i = (size_t)found; i < count - 1; i++) {
                    table[i] = table[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(table[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                printf("%s\n", table[found].value);
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s=%s\n", table[i].key, table[i].value);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(table[i].key);
        free(table[i].value);
    }
    free(table);
    return 0;
}
