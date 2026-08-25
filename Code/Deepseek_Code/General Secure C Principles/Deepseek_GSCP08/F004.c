// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Var { char *key; char *val; struct Var *next; } Var;
Var *vars = NULL;

Var *find_var(const char *key) {
    for (Var *v = vars; v; v = v->next)
        if (strcmp(v->key, key) == 0) return v;
    return NULL;
}

void set_var(const char *key, const char *val) {
    Var *v = find_var(key);
    if (v) { free(v->val); v->val = strdup(val); return; }
    v = malloc(sizeof(Var));
    if (!v) return;
    v->key = strdup(key); v->val = strdup(val); v->next = vars; vars = v;
}

void unset_var(const char *key) {
    Var *prev = NULL;
    for (Var *v = vars; v; prev = v, v = v->next) {
        if (strcmp(v->key, key) == 0) {
            if (prev) prev->next = v->next; else vars = v->next;
            free(v->key); free(v->val); free(v);
            return;
        }
    }
}

void render_text(const char *text) {
    for (const char *p = text; *p; ) {
        if (*p == '{') {
            const char *end = strchr(p, '}');
            if (end) {
                size_t len = end - p - 1;
                char *key = malloc(len + 1);
                if (key) {
                    strncpy(key, p + 1, len); key[len] = 0;
                    Var *v = find_var(key);
                    if (v) fputs(v->val, stdout);
                    else {
                        putchar('{');
                        for (const char *q = p + 1; q < end; q++) putchar(*q);
                        putchar('}');
                    }
                    free(key);
                }
                p = end + 1;
                continue;
            }
        }
        putchar(*p++);
    }
    putchar('\n');
}

void vars_list(void) {
    for (Var *v = vars; v; v = v->next) printf("%s\n", v->key);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "SET") == 0) {
            char key[256], val[4096];
            if (sscanf(rest, "%255s", key) == 1) {
                char *v = rest + strlen(key);
                while (*v == ' ') v++;
                strcpy(val, v);
                set_var(key, val);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) == 1) unset_var(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            render_text(rest);
        } else if (strcmp(cmd, "VARS") == 0) {
            vars_list();
        }
    }
    while (vars) { Var *v = vars; vars = v->next; free(v->key); free(v->val); free(v); }
    return 0;
}