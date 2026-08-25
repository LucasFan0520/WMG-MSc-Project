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
        v = malloc(sizeof(Var));
        v->key = strdup(key);
        v->value = strdup(value);
        v->next = vars;
        vars = v;
    }
}

void unset_var(const char *key) {
    Var *v = vars, *prev = NULL;
    while (v && strcmp(v->key, key) != 0) {
        prev = v;
        v = v->next;
    }
    if (!v) return;
    if (prev) prev->next = v->next;
    else vars = v->next;
    free(v->key);
    free(v->value);
    free(v);
}

void render_text(const char *text) {
    while (*text) {
        if (*text == '{') {
            const char *end = strchr(text, '}');
            if (!end) {
                putchar('{');
                text++;
                continue;
            }
            size_t keylen = end - text - 1;
            char *key = malloc(keylen + 1);
            strncpy(key, text + 1, keylen);
            key[keylen] = '\0';
            Var *v = find_var(key);
            free(key);
            if (v) {
                printf("%s", v->value);
            } else {
                putchar('{');
                for (const char *c = text + 1; c < end; c++) putchar(*c);
                putchar('}');
            }
            text = end + 1;
        } else {
            putchar(*text);
            text++;
        }
    }
    putchar('\n');
}

void vars_list(void) {
    Var *v = vars;
    while (v) {
        printf("%s %s\n", v->key, v->value);
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
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (!*p) continue;
            *p = '\0';
            p++;
            while (*p == ' ') p++;
            set_var(key, p);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            unset_var(p);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            render_text(p);
        } else if (strcmp(cmd, "VARS") == 0) {
            vars_list();
        }
    }
    free_all();
    free(line);
    return 0;
}