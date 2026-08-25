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
    } else {
        Var *nv = malloc(sizeof(Var));
        nv->key = strdup(key);
        nv->value = strdup(value);
        nv->next = vars;
        vars = nv;
    }
}

void unset_var(const char *key) {
    Var *prev = NULL, *cur = vars;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (prev) prev->next = cur->next;
            else vars = cur->next;
            free(cur->key);
            free(cur->value);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void free_vars(void) {
    while (vars) {
        Var *tmp = vars;
        vars = vars->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

char *render(const char *text) {
    char *result = malloc(strlen(text) * 2 + 1);
    result[0] = '\0';
    const char *p = text;
    while (*p) {
        if (*p == '{') {
            const char *end = strchr(p + 1, '}');
            if (end) {
                char key[256];
                int len = end - p - 1;
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
        char buf[2] = { *p, '\0' };
        strcat(result, buf);
        p++;
    }
    return result;
}

int main(void) {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char key[256];
        int n = sscanf(line, "%s", cmd);
        if (n < 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *k = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p++ = '\0';
            while (*p == ' ') p++;
            set_var(k, p);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *k = p;
            while (*p && *p != ' ') p++;
            if (*p) *p = '\0';
            unset_var(k);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *rendered = render(p);
            printf("%s\n", rendered);
            free(rendered);
        } else if (strcmp(cmd, "VARS") == 0) {
            Var *v = vars;
            while (v) {
                printf("%s %s\n", v->key, v->value);
                v = v->next;
            }
        }
    }
    free_vars();
    return 0;
}