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
    Var *prev = NULL;
    Var *v = vars;
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
    char result[4096] = "";
    char *p = (char *)text;
    while (*p) {
        if (*p == '{') {
            char *end = strchr(p, '}');
            if (end) {
                char key[256];
                int len = end - p - 1;
                if (len < (int)sizeof(key)) {
                    strncpy(key, p + 1, len);
                    key[len] = '\0';
                    Var *v = find_var(key);
                    if (v) {
                        strcat(result, v->value);
                    } else {
                        strncat(result, p, end - p + 1);
                    }
                    p = end + 1;
                    continue;
                }
            }
        }
        char tmp[2] = {*p, '\0'};
        strcat(result, tmp);
        p++;
    }
    for (char *s = result; *s; s++) putchar(*s == ' ' ? '_' : *s);
    putchar('\n');
}

void print_vars() {
    Var *v = vars;
    while (v) {
        printf("%s:", v->key);
        for (char *s = v->value; *s; s++) putchar(*s == ' ' ? '_' : *s);
        putchar('\n');
        v = v->next;
    }
}

void free_all() {
    Var *v = vars;
    while (v) {
        Var *next = v->next;
        free(v->key);
        free(v->value);
        free(v);
        v = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "SET") == 0) {
            char key[256], value[4096];
            if (sscanf(rest, "%255s %[^\n]", key, value) == 2) {
                set_var(key, value);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) == 1) {
                unset_var(key);
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            render_text(rest);
        } else if (strcmp(cmd, "VARS") == 0) {
            print_vars();
        }
    }
    free_all();
    return 0;
}