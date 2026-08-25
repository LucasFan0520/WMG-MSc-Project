// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Var {
    char *key;
    char *value;
    struct Var *next;
} Var;

Var *head = NULL;

Var *find_var(const char *key) {
    Var *v = head;
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
        v->value = malloc(strlen(value) + 1);
        strcpy(v->value, value);
        return;
    }
    Var *newv = malloc(sizeof(Var));
    newv->key = malloc(strlen(key) + 1);
    strcpy(newv->key, key);
    newv->value = malloc(strlen(value) + 1);
    strcpy(newv->value, value);
    newv->next = head;
    head = newv;
}

void unset_var(const char *key) {
    Var *v = head, *prev = NULL;
    while (v) {
        if (strcmp(v->key, key) == 0) {
            if (prev) prev->next = v->next;
            else head = v->next;
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
    for (const char *p = text; *p; ) {
        if (*p == '{') {
            const char *end = strchr(p, '}');
            if (end) {
                size_t len = end - p - 1;
                char *key = malloc(len + 1);
                strncpy(key, p + 1, len);
                key[len] = '\0';
                Var *v = find_var(key);
                if (v) {
                    printf("%s", v->value);
                } else {
                    for (const char *q = p; q <= end; q++) putchar(*q);
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

void vars_list(void) {
    Var *v = head;
    while (v) {
        printf("%s %s\n", v->key, v->value);
        v = v->next;
    }
}

void free_all(void) {
    Var *v = head;
    while (v) {
        Var *tmp = v->next;
        free(v->key);
        free(v->value);
        free(v);
        v = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "SET") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) == 1) {
                const char *val = rest + strlen(key) + 1;
                while (*val == ' ') val++;
                if (*val) set_var(key, val);
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
    free_all();
    return 0;
}