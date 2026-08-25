// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { char *key; char *val; } Var;
Var *vars = NULL;
int var_count = 0, var_cap = 0;

int find_var(const char *key) {
    for (int i = 0; i < var_count; i++)
        if (strcmp(vars[i].key, key) == 0) return i;
    return -1;
}

void set_var(const char *key, const char *val) {
    int idx = find_var(key);
    if (idx >= 0) {
        free(vars[idx].val);
        vars[idx].val = strdup(val);
    } else {
        if (var_count == var_cap) {
            var_cap = var_cap ? var_cap * 2 : 4;
            vars = realloc(vars, sizeof(Var) * var_cap);
        }
        vars[var_count].key = strdup(key);
        vars[var_count].val = strdup(val);
        var_count++;
    }
}

void unset_var(const char *key) {
    int idx = find_var(key);
    if (idx < 0) return;
    free(vars[idx].key);
    free(vars[idx].val);
    for (int i = idx; i < var_count - 1; i++) vars[i] = vars[i + 1];
    var_count--;
}

void render_text(const char *text) {
    const char *p = text;
    while (*p) {
        if (*p == '{') {
            const char *end = strchr(p + 1, '}');
            if (end) {
                int len = end - p - 1;
                char *key = malloc(len + 1);
                strncpy(key, p + 1, len);
                key[len] = '\0';
                int idx = find_var(key);
                if (idx >= 0) printf("%s", vars[idx].val);
                else printf("{%.*s}", len, p + 1);
                free(key);
                p = end + 1;
                continue;
            }
        }
        putchar(*p);
        p++;
    }
    putchar('\n');
}

void print_vars(void) {
    for (int i = 0; i < var_count; i++)
        printf("%s %s\n", vars[i].key, vars[i].val);
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], rest[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", arg1, rest) == 2)
                set_var(arg1, rest);
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1) unset_var(arg1);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            render_text(p);
        } else if (strcmp(cmd, "VARS") == 0) print_vars();
    }
    for (int i = 0; i < var_count; i++) { free(vars[i].key); free(vars[i].val); }
    free(vars);
    return 0;
}