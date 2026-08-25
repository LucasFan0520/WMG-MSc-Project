/* F004.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct var {
    char *key;
    char *value;
    struct var *next;
} var_t;

var_t *vars = NULL;

var_t *find_var(const char *key) {
    var_t *v = vars;
    while (v) {
        if (strcmp(v->key, key) == 0) return v;
        v = v->next;
    }
    return NULL;
}

void set_var(const char *key, const char *value) {
    var_t *v = find_var(key);
    if (v) {
        char *new = strdup(value);
        if (!new) return;
        free(v->value);
        v->value = new;
        return;
    }
    v = malloc(sizeof(var_t));
    if (!v) return;
    v->key = strdup(key);
    v->value = strdup(value);
    v->next = vars;
    vars = v;
    if (!v->key || !v->value) {
        free(v->key); free(v->value); free(v);
    }
}

void unset_var(const char *key) {
    var_t *prev = NULL, *v = vars;
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
    const char *p = text;
    while (*p) {
        if (*p == '{') {
            const char *start = p + 1;
            const char *end = strchr(start, '}');
            if (end) {
                size_t keylen = end - start;
                char *key = malloc(keylen + 1);
                if (key) {
                    memcpy(key, start, keylen);
                    key[keylen] = 0;
                    var_t *v = find_var(key);
                    free(key);
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

void print_vars(void) {
    var_t *v = vars;
    while (v) {
        printf("%s %s\n", v->key, v->value);
        v = v->next;
    }
}

void free_all(void) {
    var_t *v = vars;
    while (v) {
        var_t *nxt = v->next;
        free(v->key);
        free(v->value);
        free(v);
        v = nxt;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], key[256];
        if (sscanf(line, "%9s %255s", cmd, key) >= 1) {
            const char *rest = line + strlen(cmd) + 1;
            if (*rest) rest++;
            if (strcmp(cmd, "SET") == 0) {
                set_var(key, rest);
            } else if (strcmp(cmd, "UNSET") == 0) {
                unset_var(key);
            } else if (strcmp(cmd, "RENDER") == 0) {
                render_text(rest);
            } else if (strcmp(cmd, "VARS") == 0) {
                print_vars();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}