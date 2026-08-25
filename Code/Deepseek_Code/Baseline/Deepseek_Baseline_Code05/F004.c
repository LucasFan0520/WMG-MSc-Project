/* F004.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct var {
    char *key;
    char *value;
    struct var *next;
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
    Var *new = malloc(sizeof(Var));
    new->key = strdup(key);
    new->value = strdup(value);
    new->next = vars;
    vars = new;
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

void render(const char *text) {
    char buf[2048] = "";
    const char *p = text;
    while (*p) {
        if (*p == '{') {
            const char *end = strchr(p, '}');
            if (end) {
                size_t len = end - p - 1;
                char key[256];
                if (len < 256) {
                    strncpy(key, p + 1, len);
                    key[len] = '\0';
                    Var *v = find_var(key);
                    if (v) strcat(buf, v->value);
                    else { strncat(buf, p, end - p + 1); }
                } else {
                    strncat(buf, p, end - p + 1);
                }
                p = end + 1;
                continue;
            }
        }
        strncat(buf, p, 1);
        p++;
    }
    for (char *c = buf; *c; c++) putchar(*c == ' ' ? '_' : *c);
    putchar('\n');
}

void print_vars() {
    Var *v = vars;
    while (v) {
        printf("%s ", v->key);
        for (char *c = v->value; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
        v = v->next;
    }
}

int main() {
    char line[2048];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], key[256], val[2048];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s", key) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(key) + 1;
            if (*rest == ' ') rest++;
            set_var(key, rest);
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1) unset_var(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *rest = line + strlen(cmd) + 1;
            if (*rest == ' ') rest++;
            render(rest);
        } else if (strcmp(cmd, "VARS") == 0) print_vars();
    }
    return 0;
}