// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Var {
    char *key;
    char *value;
    struct Var *next;
} Var;

Var *vars = NULL;

Var* find_var(const char *key) {
    for (Var *v = vars; v; v = v->next)
        if (strcmp(v->key, key) == 0) return v;
    return NULL;
}

void render_text(const char *text) {
    const char *p = text;
    while (*p) {
        if (*p == '{') {
            const char *end = strchr(p, '}');
            if (end) {
                size_t len = end - p - 1;
                char key[256];
                if (len < sizeof(key)) {
                    strncpy(key, p + 1, len);
                    key[len] = '\0';
                    Var *v = find_var(key);
                    if (v) {
                        printf("%s", v->value);
                        p = end + 1;
                        continue;
                    }
                }
            }
        }
        putchar(*p);
        p++;
    }
    putchar('\n');
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "SET") == 0) {
            char key[256];
            int n = 0;
            if (sscanf(rest, "%255s%n", key, &n) != 1) continue;
            char *val = rest + n;
            while (*val == ' ') val++;
            Var *v = find_var(key);
            if (v) {
                free(v->value);
                v->value = strdup(val);
            } else {
                v = malloc(sizeof(Var));
                if (!v) continue;
                v->key = strdup(key);
                v->value = strdup(val);
                v->next = vars;
                vars = v;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) != 1) continue;
            Var *prev = NULL;
            for (Var *v = vars; v; prev = v, v = v->next) {
                if (strcmp(v->key, key) == 0) {
                    if (prev) prev->next = v->next;
                    else vars = v->next;
                    free(v->key);
                    free(v->value);
                    free(v);
                    break;
                }
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            render_text(rest);
        } else if (strcmp(cmd, "VARS") == 0) {
            for (Var *v = vars; v; v = v->next) {
                printf("%s=%s\n", v->key, v->value);
            }
        }
    }
    while (vars) {
        Var *v = vars;
        vars = vars->next;
        free(v->key);
        free(v->value);
        free(v);
    }
    return 0;
}