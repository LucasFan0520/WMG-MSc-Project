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
        Var *new = malloc(sizeof(Var));
        if (!new) return;
        new->key = strdup(key);
        new->value = strdup(value);
        new->next = vars;
        vars = new;
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

void render_text(const char *text) {
    char *result = malloc(strlen(text) + 1);
    if (!result) return;
    result[0] = '\0';
    char *p = (char *)text;
    while (*p) {
        if (*p == '{') {
            char *end = strchr(p + 1, '}');
            if (end) {
                int keylen = end - p - 1;
                char *key = malloc(keylen + 1);
                if (key) {
                    strncpy(key, p + 1, keylen);
                    key[keylen] = '\0';
                    Var *v = find_var(key);
                    if (v) {
                        strcat(result, v->value);
                    } else {
                        strncat(result, p, end - p + 1);
                    }
                    free(key);
                }
                p = end + 1;
                continue;
            }
        }
        // copy char
        size_t len = strlen(result);
        result[len] = *p;
        result[len + 1] = '\0';
        p++;
    }
    printf("%s\n", result);
    free(result);
}

void print_vars(void) {
    Var *v = vars;
    while (v) {
        printf("%s: %s\n", v->key, v->value);
        v = v->next;
    }
}

void free_all(void) {
    Var *v = vars;
    while (v) {
        Var *next = v->next;
        free(v->key);
        free(v->value);
        free(v);
        v = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (!space) continue;
            *space = '\0';
            char *val = space + 1;
            while (*val == ' ') val++;
            set_var(key, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (space) *space = '\0';
            unset_var(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            render_text(p);
        } else if (strcmp(cmd, "VARS") == 0) {
            print_vars();
        }
    }
    free_all();
    return 0;
}