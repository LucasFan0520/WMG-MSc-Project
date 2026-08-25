// F004.c
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
    char *key;
    char *value;
} Variable;

int main(void) {
    Variable *vars = NULL;
    size_t count = 0;
    size_t cap = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) {
            free(line);
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            char *key = p;
            while (*p && *p != ' ') p++;
            char *value = "";
            if (*p == ' ') {
                *p = '\0';
                value = p + 1;
            }
            size_t found = (size_t)-1;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != (size_t)-1) {
                free(vars[found].value);
                vars[found].value = safe_dup(value);
            } else {
                if (count >= cap) {
                    cap = cap == 0 ? 4 : cap * 2;
                    Variable *nb = realloc(vars, cap * sizeof(Variable));
                    if (!nb) {
                        free(line);
                        break;
                    }
                    vars = nb;
                }
                vars[count].key = safe_dup(key);
                vars[count].value = safe_dup(value);
                count++;
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
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
        } else if (strncmp(line, "RENDER ", 7) == 0) {
            char *text = line + 7;
            char *p = text;
            while (*p) {
                if (*p == '{') {
                    char *end = strchr(p, '}');
                    if (end) {
                        size_t klen = end - (p + 1);
                        char *k = malloc(klen + 1);
                        if (k) {
                            memcpy(k, p + 1, klen);
                            k[klen] = '\0';
                            char *val = NULL;
                            for (size_t i = 0; i < count; i++) {
                                if (strcmp(vars[i].key, k) == 0) {
                                    val = vars[i].value;
                                    break;
                                }
                            }
                            if (val) {
                                printf("%s", val);
                            } else {
                                fwrite(p, 1, end - p + 1, stdout);
                            }
                            free(k);
                            p = end + 1;
                        } else {
                            putchar(*p);
                            p++;
                        }
                    } else {
                        putchar(*p);
                        p++;
                    }
                } else {
                    putchar(*p);
                    p++;
                }
            }
            putchar('\n');
        } else if (strcmp(line, "VARS") == 0) {
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
