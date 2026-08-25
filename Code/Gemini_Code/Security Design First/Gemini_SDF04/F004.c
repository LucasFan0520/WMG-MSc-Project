// F004.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Variable;

Variable *vars = NULL;
int count = 0;
int capacity = 0;

int find_var(const char *key) {
    for (int i = 0; i < count; i++) {
        if (strcmp(vars[i].key, key) == 0) {
            return i;
        }
    }
    return -1;
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;
    while ((nread = getline(&line, &len, stdin)) != -1) {
        size_t l = strlen(line);
        if (l > 0 && line[l - 1] == '\n') {
            line[l - 1] = '\0';
        }
        char *cmd = line;
        char *space1 = strchr(cmd, ' ');
        if (space1) {
            *space1 = '\0';
            char *arg1 = space1 + 1;
            if (strcmp(cmd, "SET") == 0) {
                char *space2 = strchr(arg1, ' ');
                if (space2) {
                    *space2 = '\0';
                    char *val = space2 + 1;
                    int idx = find_var(arg1);
                    if (idx != -1) {
                        char *nv = strdup(val);
                        if (nv) {
                            free(vars[idx].value);
                            vars[idx].value = nv;
                        }
                    } else {
                        if (count >= capacity) {
                            capacity = capacity == 0 ? 4 : capacity * 2;
                            Variable *tmp = realloc(vars, capacity * sizeof(Variable));
                            if (!tmp) {
                                free(line);
                                return 1;
                            }
                            vars = tmp;
                        }
                        vars[count].key = strdup(arg1);
                        vars[count].value = strdup(val);
                        if (vars[count].key && vars[count].value) {
                            count++;
                        }
                    }
                }
            } else if (strcmp(cmd, "UNSET") == 0) {
                int idx = find_var(arg1);
                if (idx != -1) {
                    free(vars[idx].key);
                    free(vars[idx].value);
                    for (int i = idx; i < count - 1; i++) {
                        vars[i] = vars[i + 1];
                    }
                    count--;
                }
            } else if (strcmp(cmd, "RENDER") == 0) {
                size_t i = 0;
                while (arg1[i] != '\0') {
                    if (arg1[i] == '{') {
                        size_t j = i + 1;
                        while (arg1[j] != '\0' && arg1[j] != '}') {
                            j++;
                        }
                        if (arg1[j] == '}') {
                            size_t klen = j - i - 1;
                            char *key = malloc(klen + 1);
                            if (key) {
                                strncpy(key, arg1 + i + 1, klen);
                                key[klen] = '\0';
                                int idx = find_var(key);
                                if (idx != -1) {
                                    printf("%s", vars[idx].value);
                                } else {
                                    for (size_t k = i; k <= j; k++) {
                                        putchar(arg1[k]);
                                    }
                                }
                                free(key);
                            }
                            i = j + 1;
                        } else {
                            putchar(arg1[i]);
                            i++;
                        }
                    } else {
                        putchar(arg1[i]);
                        i++;
                    }
                }
                putchar('\n');
            }
        } else {
            if (strcmp(cmd, "VARS") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", vars[i].key, vars[i].value);
                }
            } else if (strcmp(cmd, "END") == 0) {
                break;
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