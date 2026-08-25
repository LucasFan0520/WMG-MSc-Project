// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct var {
    char *key;
    char *value;
    struct var *next;
} Var;

Var *head = NULL;

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

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
        v->value = strdup(value);
        if (!v->value) { fprintf(stderr, "Memory error\n"); exit(1); }
        return;
    }
    Var *newv = malloc(sizeof(Var));
    if (!newv) { fprintf(stderr, "Memory error\n"); exit(1); }
    newv->key = strdup(key);
    newv->value = strdup(value);
    if (!newv->key || !newv->value) { fprintf(stderr, "Memory error\n"); exit(1); }
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
    const char *p = text;
    while (*p) {
        if (*p == '{') {
            const char *start = p + 1;
            const char *end = strchr(start, '}');
            if (end) {
                size_t key_len = end - start;
                char *key = malloc(key_len + 1);
                if (!key) { fprintf(stderr, "Memory error\n"); exit(1); }
                memcpy(key, start, key_len);
                key[key_len] = '\0';
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
        Var *next = v->next;
        free(v->key);
        free(v->value);
        free(v);
        v = next;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        if (strcmp(cmd, "VARS") == 0) { vars_list(); continue; }
        char *rest = line + strlen(cmd);
        while (isspace(*rest)) rest++;
        if (strcmp(cmd, "SET") == 0) {
            char *key = rest;
            while (*rest && !isspace(*rest)) rest++;
            if (!*rest) continue;
            *rest = '\0';
            rest++;
            while (isspace(*rest)) rest++;
            set_var(key, rest);
        } else if (strcmp(cmd, "UNSET") == 0) {
            unset_var(rest);
        } else if (strcmp(cmd, "RENDER") == 0) {
            render_text(rest);
        }
    }
    free_all();
    return 0;
}