/* F004.c */
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
        return;
    }
    v = malloc(sizeof(Var));
    if (!v) return;
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
    char *result = malloc(1);
    result[0] = '\0';
    size_t len = 0;
    const char *p = text;
    while (*p) {
        if (*p == '{') {
            const char *start = p + 1;
            const char *end = strchr(start, '}');
            if (end) {
                size_t keylen = end - start;
                char *key = malloc(keylen + 1);
                strncpy(key, start, keylen);
                key[keylen] = '\0';
                Var *v = find_var(key);
                if (v) {
                    result = realloc(result, len + strlen(v->value) + 1);
                    strcpy(result + len, v->value);
                    len += strlen(v->value);
                } else {
                    result = realloc(result, len + keylen + 3);
                    result[len++] = '{';
                    strcpy(result + len, key);
                    len += keylen;
                    result[len++] = '}';
                }
                free(key);
                p = end + 1;
                continue;
            }
        }
        result = realloc(result, len + 2);
        result[len++] = *p;
        result[len] = '\0';
        p++;
    }
    printf("%s\n", result);
    free(result);
}

void list_vars(void) {
    Var *v = vars;
    while (v) {
        printf("%s %s\n", v->key, v->value);
        v = v->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[2048];
        int n = sscanf(line, "%15s %255s %2047s", cmd, arg1, arg2);
        if (n < 2) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *val = line + strlen(cmd) + strlen(arg1) + 2;
            while (*val == ' ') val++;
            set_var(arg1, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            unset_var(arg1);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *txt = line + strlen(cmd) + 1;
            while (*txt == ' ') txt++;
            render_text(txt);
        } else if (strcmp(cmd, "VARS") == 0) {
            list_vars();
        }
    }
    while (vars) {
        Var *tmp = vars;
        vars = vars->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
    return 0;
}