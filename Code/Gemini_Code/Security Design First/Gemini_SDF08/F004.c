// F004.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Variable;

int main(void) {
    Variable *vars = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 0 && line[read - 1] == '\n') {
            line[read - 1] = '\0';
        }
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p = line + 4;
            char *space = strchr(p, ' ');
            if (space) {
                *space = '\0';
                char *key = p;
                char *val = space + 1;
                int found = -1;
                for (int i = 0; i < count; i++) {
                    if (strcmp(vars[i].key, key) == 0) {
                        found = i;
                        break;
                    }
                }
                if (found != -1) {
                    char *nv = strdup(val);
                    if (nv) {
                        free(vars[found].value);
                        vars[found].value = nv;
                    }
                } else {
                    Variable *tmp = realloc(vars, sizeof(Variable) * (count + 1));
                    if (tmp) {
                        vars = tmp;
                        vars[count].key = strdup(key);
                        vars[count].value = strdup(val);
                        if (vars[count].key && vars[count].value) {
                            count++;
                        }
                    }
                }
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                free(vars[found].key);
                free(vars[found].value);
                for (int i = found; i < count - 1; i++) {
                    vars[i] = vars[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "RENDER ", 7) == 0) {
            char *text = line + 7;
            while (*text) {
                if (*text == '{') {
                    char *close = strchr(text, '}');
                    if (close) {
                        *close = '\0';
                        char *key = text + 1;
                        int found = -1;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(vars[i].key, key) == 0) {
                                found = i;
                                break;
                            }
                        }
                        if (found != -1) {
                            printf("%s", vars[found].value);
                        } else {
                            printf("{%s}", key);
                        }
                        *close = '}';
                        text = close + 1;
                    } else {
                        putchar(*text);
                        text++;
                    }
                } else {
                    putchar(*text);
                    text++;
                }
            }
            putchar('\n');
        } else if (strcmp(line, "VARS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", vars[i].key, vars[i].value);
            }
        }
    }
    for (int i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
    free(line);
    return 0;
}
