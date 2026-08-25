// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line(FILE *f) {
    size_t size = 16;
    size_t len = 0;
    char *str = malloc(size);
    if (!str) return NULL;
    int c;
    while ((c = fgetc(f)) != EOF && c != '\n') {
        if (len + 1 >= size) {
            size *= 2;
            char *new_str = realloc(str, size);
            if (!new_str) { free(str); return NULL; }
            str = new_str;
        }
        str[len++] = c;
    }
    if (len == 0 && c == EOF) { free(str); return NULL; }
    str[len] = '\0';
    return str;
}

typedef struct {
    char *key;
    char *value;
} Var;

int main() {
    Var *vars = NULL;
    int count = 0;
    while (1) {
        char *line = read_line(stdin);
        if (!line) break;
        if (strcmp(line, "END") == 0) { free(line); break; }
        char *cmd = line;
        char *p1 = strchr(cmd, ' ');
        if (!p1) {
            if (strcmp(cmd, "VARS") == 0) {
                for (int i = 0; i < count; i++) {
                    printf("%s %s\n", vars[i].key, vars[i].value);
                }
            }
            free(line);
            continue;
        }
        *p1 = '\0';
        char *args = p1 + 1;
        if (strcmp(cmd, "SET") == 0) {
            char *p2 = strchr(args, ' ');
            if (p2) {
                *p2 = '\0';
                char *key = args;
                char *val = p2 + 1;
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
                    vars = realloc(vars, sizeof(Var) * (count + 1));
                    vars[count].key = strdup(key);
                    vars[count].value = strdup(val);
                    count++;
                }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = args;
            for (int i = 0; i < count; i++) {
                if (strcmp(vars[i].key, key) == 0) {
                    free(vars[i].key);
                    free(vars[i].value);
                    for (int j = i; j < count - 1; j++) {
                        vars[j] = vars[j + 1];
                    }
                    count--;
                    if (count == 0) { free(vars); vars = NULL; }
                    else { vars = realloc(vars, sizeof(Var) * count); }
                    break;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text = args;
            int i = 0;
            while (text[i] != '\0') {
                if (text[i] == '{') {
                    char *close = strchr(text + i, '}');
                    if (close) {
                        *close = '\0';
                        char *k = text + i + 1;
                        char *v = NULL;
                        for (int v_idx = 0; v_idx < count; v_idx++) {
                            if (strcmp(vars[v_idx].key, k) == 0) {
                                v = vars[v_idx].value;
                                break;
                            }
                        }
                        if (v) {
                            printf("%s", v);
                        } else {
                            printf("{%s}", k);
                        }
                        *close = '}';
                        i = (close - text) + 1;
                    } else {
                        printf("%c", text[i]);
                        i++;
                    }
                } else {
                    printf("%c", text[i]);
                    i++;
                }
            }
            printf("\n");
        }
        free(line);
    }
    for (int i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
    return 0;
}
