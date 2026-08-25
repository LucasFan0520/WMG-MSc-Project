// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t cap = 128;
    size_t len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
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
    if (c == EOF && len == 0) {
        free(buf);
        return NULL;
    }
    buf[len] = '\0';
    return buf;
}

char *mystrdup(const char *s) {
    size_t len = strlen(s);
    char *d = malloc(len + 1);
    if (d) strcpy(d, s);
    return d;
}

typedef struct {
    char *key;
    char *value;
} Variable;

int main() {
    Variable *vars = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        char *p = line;
        while (*p == ' ') p++;
        char *cmd = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            free(line);
            break;
        }
        while (*p == ' ') p++;
        char *key = p;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        while (*p == ' ') p++;
        char *value = p;
        if (strcmp(cmd, "SET") == 0) {
            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    free(vars[i].value);
                    vars[i].value = mystrdup(value);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    vars = realloc(vars, cap * sizeof(Variable));
                }
                vars[count].key = mystrdup(key);
                vars[count].value = mystrdup(value);
                count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            for (size_t i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    free(vars[i].key);
                    free(vars[i].value);
                    for (size_t j = i; j < count - 1; j++) {
                        vars[j] = vars[j + 1];
                    }
                    count--;
                    break;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *t = key;
            if (value && *value) {
                *(value - 1) = ' ';
            }
            while (*t) {
                if (*t == '{') {
                    char *end = strchr(t, '}');
                    if (end) {
                        size_t klen = end - t - 1;
                        char *kbuf = malloc(klen + 1);
                        strncpy(kbuf, t + 1, klen);
                        kbuf[klen] = '\0';
                        char *val = NULL;
                        for (size_t i = 0; i < count; i++) {
                            if (strcmp(vars[i].key, kbuf) == 0) {
                                val = vars[i].value;
                                break;
                            }
                        }
                        if (val) {
                            printf("%s", val);
                            t = end + 1;
                        } else {
                            putchar(*t);
                            t++;
                        }
                        free(kbuf);
                    } else {
                        putchar(*t);
                        t++;
                    }
                } else {
                    putchar(*t);
                    t++;
                }
            }
            putchar('\n');
        } else if (strcmp(cmd, "VARS") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", vars[i].key, vars[i].value);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
    return 0;
}
