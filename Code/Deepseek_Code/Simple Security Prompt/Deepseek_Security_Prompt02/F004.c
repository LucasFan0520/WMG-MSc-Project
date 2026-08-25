/* F004.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void free_var(Var *v) {
    free(v->key);
    free(v->value);
    free(v);
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
    Var *prev = NULL, *curr = vars;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else vars = curr->next;
            free_var(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void render_text(const char *text) {
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
                    printf("%s", v->value);
                } else {
                    putchar('{');
                    printf("%s", key);
                    putchar('}');
                }
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
    Var *v = vars;
    while (v) {
        printf("%s %s\n", v->key, v->value);
        v = v->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[8192];
    char cmd[32];
    char key[256];
    char value[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", key) != 1) continue;
            p += strlen(key);
            while (*p == ' ') p++;
            set_var(key, p);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", key) == 1) unset_var(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            render_text(p);
        } else if (strcmp(cmd, "VARS") == 0) {
            print_vars();
        }
    }

    while (vars) {
        Var *tmp = vars;
        vars = vars->next;
        free_var(tmp);
    }
    return 0;
}