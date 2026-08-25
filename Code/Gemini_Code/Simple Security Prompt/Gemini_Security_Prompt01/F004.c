// F004.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Variable;

int main() {
    Variable *vars = NULL;
    int count = 0;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strcmp(line, "END") == 0) {
            break;
        }

        char *cmd = line;
        char *space1 = strchr(line, ' ');
        char *args = NULL;
        if (space1) {
            *space1 = '\0';
            args = space1 + 1;
            while (*args == ' ') args++;
        }

        if (strcmp(cmd, "SET") == 0 && args) {
            char *key = args;
            char *space2 = strchr(args, ' ');
            char *value = "";
            if (space2) {
                *space2 = '\0';
                value = space2 + 1;
                while (*value == ' ') value++;
            }
            int found = 0;
            for (int i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    free(vars[i].value);
                    vars[i].value = strdup(value);
                    found = 1;
                    break;
                }
            }
            if (!found) {
                vars = realloc(vars, (count + 1) * sizeof(Variable));
                if (!vars) return 1;
                vars[count].key = strdup(key);
                vars[count].value = strdup(value);
                count++;
            }
        } else if (strcmp(cmd, "UNSET") == 0 && args) {
            for (int i = 0; i < count; i++) {
                if (strcmp(vars[i].key, args) == 0) {
                    free(vars[i].key);
                    free(vars[i].value);
                    for (int j = i; j < count - 1; j++) {
                        vars[j] = vars[j + 1];
                    }
                    count--;
                    vars = realloc(vars, count * sizeof(Variable));
                    if (count > 0 && !vars) return 1;
                    break;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0 && args) {
            char *p = args;
            while (*p != '\0') {
                if (*p == '{') {
                    char *end = strchr(p, '}');
                    if (end) {
                        size_t klen = end - p - 1;
                        char *kstr = malloc(klen + 1);
                        if (!kstr) return 1;
                        strncpy(kstr, p + 1, klen);
                        kstr[klen] = '\0';

                        char *vstr = NULL;
                        for (int i = 0; i < count; i++) {
                            if (strcmp(vars[i].key, kstr) == 0) {
                                vstr = vars[i].value;
                                break;
                            }
                        }

                        if (vstr) {
                            printf("%s", vstr);
                        } else {
                            fwrite(p, 1, klen + 2, stdout);
                        }
                        free(kstr);
                        p = end + 1;
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
        } else if (strcmp(cmd, "VARS") == 0) {
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
