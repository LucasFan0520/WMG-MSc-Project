/* F004.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Variable;

int main(void) {
    Variable *vars = NULL;
    size_t count = 0;
    size_t capacity = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t str_len = strlen(line);
        while (str_len > 0 && (line[str_len - 1] == '\n' || line[str_len - 1] == '\r')) {
            line[str_len - 1] = '\0';
            str_len--;
        }
        char *cmd = line;
        while (*cmd == ' ') cmd++;
        if (*cmd == '\0') continue;
        char *p = cmd;
        while (*p && *p != ' ') p++;
        if (*p) {
            *p = '\0';
            p++;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        while (*p == ' ') p++;
        char *arg = p;

        if (strcmp(cmd, "SET") == 0) {
            char *key = arg;
            while (*p && *p != ' ') p++;
            if (*p) {
                *p = '\0';
                p++;
            }
            while (*p == ' ') p++;
            char *value = p;

            int found = 0;
            for (size_t i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    char *temp = strdup(value);
                    if (temp) {
                        free(vars[i].value);
                        vars[i].value = temp;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                if (count >= capacity) {
                    size_t new_cap = capacity == 0 ? 4 : capacity * 2;
                    Variable *temp = realloc(vars, new_cap * sizeof(Variable));
                    if (!temp) break;
                    vars = temp;
                    capacity = new_cap;
                }
                vars[count].key = strdup(key);
                vars[count].value = strdup(value);
                if (vars[count].key && vars[count].value) {
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = arg;
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
            char *t = arg;
            while (*t) {
                if (*t == '{') {
                    char *close = strchr(t, '}');
                    if (close) {
                        *close = '\0';
                        char *k = t + 1;
                        int found = 0;
                        for (size_t i = 0; i < count; i++) {
                            if (strcmp(vars[i].key, k) == 0) {
                                printf("%s", vars[i].value);
                                found = 1;
                                break;
                            }
                        }
                        if (!found) {
                            *close = '}';
                            printf("{%s}", k);
                        } else {
                            *close = '}';
                        }
                        t = close + 1;
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
    }
    free(line);
    for (size_t i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
    return 0;
}
