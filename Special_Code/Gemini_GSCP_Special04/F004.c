/* F004.c */
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
    char *val;
} Variable;

int main(void) {
    Variable *vars = NULL;
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
        if (strcmp(cmd, "SET") == 0) {
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p == ' ') {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *val = p;
            int found = -1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                char *nv = mystrdup(val);
                if (nv) {
                    free(vars[found].val);
                    vars[found].val = nv;
                }
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Variable *nv = realloc(vars, cap * sizeof(Variable));
                    if (nv) vars = nv;
                }
                char *nk = mystrdup(key);
                char *nv = mystrdup(val);
                if (nk && nv) {
                    vars[count].key = nk;
                    vars[count].val = nv;
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
                if (strcmp(vars[i].key, key) == 0) {
                    found = (int)i;
                    break;
                }
            }
            if (found != -1) {
                free(vars[found].key);
                free(vars[found].val);
                for (size_t i = found; i < count - 1; i++) {
                    vars[i] = vars[i + 1];
                }
                count--;
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            while (*p == ' ') p++;
            char *t = p;
            while (*t) {
                if (*t == '{') {
                    char *end = strchr(t, '}');
                    if (end) {
                        size_t klen = end - (t + 1);
                        char *kbuf = malloc(klen + 1);
                        if (kbuf) {
                            strncpy(kbuf, t + 1, klen);
                            kbuf[klen] = '\0';
                            int found = -1;
                            for (size_t i = 0; i < count; i++) {
                                if (strcmp(vars[i].key, kbuf) == 0) {
                                    found = (int)i;
                                    break;
                               }
                            }
                            free(kbuf);
                            if (found != -1) {
                                printf("%s", vars[found].val);
                                t = end + 1;
                                continue;
                            }
                        }
                    }
                }
                putchar(*t);
                t++;
            }
            putchar('\n');
        } else if (strcmp(cmd, "VARS") == 0) {
            for (size_t i = 0; i < count; i++) {
                printf("%s %s\n", vars[i].key, vars[i].val);
            }
        }
        free(line);
    }
    for (size_t i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].val);
    }
    free(vars);
    return 0;
}
