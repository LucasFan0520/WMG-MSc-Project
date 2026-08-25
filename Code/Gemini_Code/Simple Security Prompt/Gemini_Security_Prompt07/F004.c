// F004.c
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
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        char *cmd = strtok(line, " ");
        if (!cmd) {
            continue;
        }
        if (strcmp(cmd, "END") == 0) {
            break;
        }
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            char *val = strtok(NULL, "");
            if (key && val) {
                int found = 0;
                for (int i = 0; i < count; i++) {
                    if (strcmp(vars[i].key, key) == 0) {
                        free(vars[i].value);
                        vars[i].value = strdup(val);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    vars = realloc(vars, (count + 1) * sizeof(Variable));
                    vars[count].key = strdup(key);
                    vars[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) {
                for (int i = 0; i < count; i++) {
                    if (strcmp(vars[i].key, key) == 0) {
                        free(vars[i].key);
                        free(vars[i].value);
                        for (int j = i; j < count - 1; j++) {
                            vars[j] = vars[j + 1];
                        }
                        count--;
                        i--;
                    }
                }
                if (count == 0) {
                    free(vars);
                    vars = NULL;
                } else {
                    vars = realloc(vars, count * sizeof(Variable));
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text = strtok(NULL, "");
            if (text) {
                char *p = text;
                while (*p) {
                    if (*p == '{') {
                        char *end = strchr(p, '}');
                        if (end) {
                            int key_len = end - p - 1;
                            char *k = malloc(key_len + 1);
                            strncpy(k, p + 1, key_len);
                            k[key_len] = '\0';
                            char *v = NULL;
                            for (int i = 0; i < count; i++) {
                                if (strcmp(vars[i].key, k) == 0) {
                                    v = vars[i].value;
                                    break;
                                }
                            }
                            if (v) {
                                printf("%s", v);
                                p = end + 1;
                            } else {
                                putchar(*p);
                                p++;
                            }
                            free(k);
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
            }
        } else if (strcmp(cmd, "VARS") == 0) {
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
    return 0;
}
