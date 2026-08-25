/* F004.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} KV;

int main(void) {
    KV *vars = NULL;
    int count = 0;
    int capacity = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *args = line + 4;
            char *space = strchr(args, ' ');
            char *key = NULL;
            char *val = NULL;
            if (space) {
                *space = '\0';
                key = args;
                val = space + 1;
            } else {
                key = args;
                val = "";
            }
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    char *new_val = strdup(val);
                    if (new_val) {
                        free(vars[i].value);
                        vars[i].value = new_val;
                    }
                    found = 1;
                    break;
                }
            }
            if (!found) {
                char *n_key = strdup(key);
                char *n_val = strdup(val);
                if (n_key && n_val) {
                    if (count >= capacity) {
                        int new_cap = capacity == 0 ? 4 : capacity * 2;
                        KV *tmp = realloc(vars, new_cap * sizeof(KV));
                        if (tmp) {
                            vars = tmp;
                            capacity = new_cap;
                        } else {
                            free(n_key);
                            free(n_val);
                            if (space) *space = ' ';
                            continue;
                        }
                    }
                    vars[count].key = n_key;
                    vars[count].value = n_val;
                    count++;
                } else {
                    free(n_key);
                    free(n_val);
                }
            }
            if (space) *space = ' ';
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
            for (int i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    free(vars[i].key);
                    free(vars[i].value);
                    for (int j = i; j < count - 1; j++) {
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
                    char *close = strchr(p, '}');
                    if (close) {
                        *close = '\0';
                        char *key = p + 1;
                        char *val = NULL;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(vars[i].key, key) == 0) {
                                val = vars[i].value;
                                break;
                            }
                        }
                        if (val) {
                            printf("%s", val);
                        } else {
                            printf("{%s}", key);
                        }
                        *close = '}';
                        p = close + 1;
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
            for (int i = 0; i < count; i++) {
                printf("%s %s\n", vars[i].key, vars[i].value);
            }
        }
    }
    free(line);
    for (int i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
    return 0;
}
