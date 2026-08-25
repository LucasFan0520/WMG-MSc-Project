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
    int capacity = 0;
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
            char *key_start = line + 4;
            while (*key_start == ' ') key_start++;
            char *space = strchr(key_start, ' ');
            char *val_start = "";
            if (space) {
                *space = '\0';
                val_start = space + 1;
                while (*val_start == ' ') val_start++;
            }
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key_start) == 0) {
                    found = i;
                    break;
                }
            }
            if (found != -1) {
                char *nv = strdup(val_start);
                if (!nv) {
                    free(line);
                    for(int i=0; i<count; i++) { free(vars[i].key); free(vars[i].value); }
                    free(vars);
                    return 1;
                }
                free(vars[found].value);
                vars[found].value = nv;
            } else {
                if (count >= capacity) {
                    int new_cap = capacity == 0 ? 4 : capacity * 2;
                    Variable *tmp = realloc(vars, new_cap * sizeof(Variable));
                    if (!tmp) {
                        free(line);
                        for(int i=0; i<count; i++) { free(vars[i].key); free(vars[i].value); }
                        free(vars);
                        return 1;
                    }
                    vars = tmp;
                    capacity = new_cap;
                }
                char *nk = strdup(key_start);
                char *nv = strdup(val_start);
                if (!nk || !nv) {
                    free(nk); free(nv); free(line);
                    for(int i=0; i<count; i++) { free(vars[i].key); free(vars[i].value); }
                    free(vars);
                    return 1;
                }
                vars[count].key = nk;
                vars[count].value = nv;
                count++;
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key_start = line + 6;
            while (*key_start == ' ') key_start++;
            int found = -1;
            for (int i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key_start) == 0) {
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
            while (*text == ' ') text++;
            char *p = text;
            while (*p) {
                if (*p == '{') {
                    char *close = strchr(p, '}');
                    if (close) {
                        *close = '\0';
                        char *k = p + 1;
                        int found = -1;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(vars[i].key, k) == 0) {
                                found = i;
                                break;
                            }
                        }
                        if (found != -1) {
                            printf("%s", vars[found].value);
                        } else {
                            printf("{%s}", k);
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
