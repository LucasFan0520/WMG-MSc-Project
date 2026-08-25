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
    v = malloc(sizeof(Var));
    v->key = malloc(strlen(key) + 1);
    strcpy(v->key, key);
    v->value = malloc(strlen(value) + 1);
    strcpy(v->value, value);
    v->next = head;
    head = v;
}

void unset_var(const char *key) {
    Var *prev = NULL;
    Var *v = head;
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
    char buf[4096];
    strcpy(buf, text);
    char *p = buf;
    while (*p) {
        if (*p == '{') {
            char *end = strchr(p, '}');
            if (end) {
                *end = 0;
                char *key = p + 1;
                Var *v = find_var(key);
                if (v) {
                    printf("%s", v->value);
                } else {
                    printf("{%s}", key);
                }
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
        printf("%s:%s\n", v->key, v->value);
        v = v->next;
    }
}

void free_all(void) {
    Var *v = head;
    while (v) {
        Var *next = v->next;
        free(v->key);
        free(v->value);
        free(v);
        v = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
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
    return 0;
}