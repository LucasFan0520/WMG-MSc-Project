// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct var {
    char *key;
    char *value;
    struct var *next;
} Var;

Var *vars = NULL;

Var *find_var(const char *key) {
    Var *v = vars;
    while (v) {
        if (strcmp(v->key, key) == 0) return v;
        v = v->next;
    }
    return NULL;
}

void set_var(const char *key, const char *value) {
    Var *v = find_var(key);
    if (v) {
        free(v->value);
        v->value = strdup(value);
        return;
    }
    v = malloc(sizeof(Var));
    v->key = strdup(key);
    v->value = strdup(value);
    v->next = vars;
    vars = v;
}

void unset_var(const char *key) {
    Var *v = vars, *prev = NULL;
    while (v) {
        if (strcmp(v->key, key) == 0) {
            if (prev) prev->next = v->next;
            else vars = v->next;
            free(v->key);
            free(v->value);
            free(v);
            return;
        }
        prev = v;
        v = v->next;
    }
}

void render_text(const char *text) {
    while (*text) {
        if (*text == '{') {
            const char *end = strchr(text + 1, '}');
            if (end) {
                char key[256];
                int len = end - text - 1;
                if (len < 256) {
                    strncpy(key, text + 1, len);
                    key[len] = '\0';
                    Var *v = find_var(key);
                    if (v) printf("%s", v->value);
                    else {
                        putchar('{');
                        printf("%s", key);
                        putchar('}');
                    }
                    text = end + 1;
                    continue;
                }
            }
        }
        putchar(*text);
        text++;
    }
    putchar('\n');
}

void vars_print(void) {
    Var *v = vars;
    while (v) {
        printf("%s %s\n", v->key, v->value);
        v = v->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", arg1, arg2) == 2)
                set_var(arg1, arg2);
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                unset_var(arg1);
        } else if (strcmp(cmd, "RENDER") == 0) {
            if (sscanf(line, "%*s %[^\n]", arg1) == 1)
                render_text(arg1);
        } else if (strcmp(cmd, "VARS") == 0) {
            vars_print();
        }
    }
    return 0;
}