// F004.c
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

void set_var(const char *key, const char *value) {
    int idx = find_var(key);
    if (idx != -1) {
        free(vars[idx].value);
        vars[idx].value = strdup(value);
    } else {
        if (count >= capacity) {
            capacity = capacity == 0 ? 4 : capacity * 2;
            vars = realloc(vars, capacity * sizeof(Variable));
        }
        vars[count].key = strdup(key);
        vars[count].value = strdup(value);
        count++;
    }
}

void render_text(const char *text) {
    const char *p = text;
    while (*p) {
        if (*p == '{') {
            const char *end = strchr(p, '}');
            if (end) {
                int len = end - p - 1;
                char *k = malloc(len + 1);
                strncpy(k, p + 1, len);
                k[len] = 0;
                int idx = find_var(k);
                if (idx != -1) {
                    printf("%s", vars[idx].value);
                    p = end + 1;
                } else {
                    putchar('{');
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

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\r\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        if (strncmp(line, "SET ", 4) == 0) {
            char *p1 = line + 4;
            char *p2 = strchr(p1, ' ');
            if (p2) {
                *p2 = 0;
                set_var(p1, p2 + 1);
            }
        } else if (strncmp(line, "UNSET ", 6) == 0) {
            char *key = line + 6;
            int idx = find_var(key);
            if (idx != -1) {
                free(vars[idx].key);
                free(vars[idx].value);
                for (int i = idx; i < count - 1; i++) {
                    vars[i] = vars[i + 1];
                }
                count--;
            }
        } else if (strncmp(line, "RENDER ", 7) == 0) {
            render_text(line + 7);
        } else if (strcmp(line, "VARS") == 0) {
            for (int i = 0; i < count; i++) {
                printf("%s=%s\n", vars[i].key, vars[i].value);
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
