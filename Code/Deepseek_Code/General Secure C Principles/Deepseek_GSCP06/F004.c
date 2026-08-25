// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Var {
    char *key;
    char *value;
    struct Var *next;
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
    } else {
        v = malloc(sizeof(Var));
        v->key = strdup(key);
        v->value = strdup(value);
        v->next = vars;
        vars = v;
    }
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

char *get_var(const char *key) {
    Var *v = find_var(key);
    return v ? v->value : NULL;
}

void render_text(const char *text) {
    const char *p = text;
    while (*p) {
        if (*p == '{') {
            const char *end = strchr(p, '}');
            if (end) {
                int keylen = end - p - 1;
                if (keylen > 0) {
                    char *key = malloc(keylen + 1);
                    strncpy(key, p + 1, keylen);
                    key[keylen] = 0;
                    char *val = get_var(key);
                    if (val) printf("%s", val);
                    else {
                        for (const char *q = p; q <= end; q++) putchar(*q);
                    }
                    free(key);
                    p = end + 1;
                    continue;
                }
            }
        }
        putchar(*p++);
    }
    putchar('\n');
}

void print_vars(void) {
    Var *v = vars;
    while (v) {
        printf("%s %s\n", v->key, v->value);
        v = v->next;
    }
}

void cleanup_vars(void) {
    Var *v = vars;
    while (v) {
        Var *tmp = v;
        v = v->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], key[256], rest[2048];
        int n = sscanf(line, "%31s %255s %2047s", cmd, key, rest);
        if (n < 2) continue;
        if (strcmp(cmd, "SET") == 0 && n >= 3) {
            char *val = line + strlen(cmd) + 1 + strlen(key) + 1;
            set_var(key, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            unset_var(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text = line + strlen(cmd) + 1;
            render_text(text);
        } else if (strcmp(cmd, "VARS") == 0) {
            print_vars();
        }
    }
    cleanup_vars();
    return 0;
}