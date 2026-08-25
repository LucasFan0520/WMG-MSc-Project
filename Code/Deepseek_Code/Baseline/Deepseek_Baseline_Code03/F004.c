// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct variable {
    char *key;
    char *value;
    struct variable *next;
};

struct variable *vars = NULL;

struct variable *find_var(char *key) {
    struct variable *v = vars;
    while (v) {
        if (strcmp(v->key, key) == 0) return v;
        v = v->next;
    }
    return NULL;
}

void set_var(char *key, char *value) {
    struct variable *v = find_var(key);
    if (v) {
        free(v->value);
        v->value = malloc(strlen(value) + 1);
        strcpy(v->value, value);
    } else {
        v = malloc(sizeof(struct variable));
        v->key = malloc(strlen(key) + 1);
        strcpy(v->key, key);
        v->value = malloc(strlen(value) + 1);
        strcpy(v->value, value);
        v->next = vars;
        vars = v;
    }
}

void unset_var(char *key) {
    struct variable *v = vars, *prev = NULL;
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

void render_text(char *text) {
    char output[8192] = "";
    char *p = text;
    while (*p) {
        if (*p == '{') {
            char *end = strchr(p, '}');
            if (end) {
                int keylen = end - p - 1;
                char key[256];
                strncpy(key, p+1, keylen);
                key[keylen] = 0;
                struct variable *v = find_var(key);
                if (v) {
                    strcat(output, v->value);
                } else {
                    strncat(output, p, end - p + 1);
                }
                p = end + 1;
                continue;
            }
        }
        strncat(output, p, 1);
        p++;
    }
    printf("%s\n", output);
}

void print_vars() {
    struct variable *v = vars;
    while (v) {
        printf("%s %s\n", v->key, v->value);
        v = v->next;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char key[256];
        char value[4096];
        int n = sscanf(line, "%31s %255s", cmd, key);
        if (strcmp(cmd, "SET") == 0 && n >= 2) {
            char *rest = line + strlen(cmd) + 1 + strlen(key) + 1;
            set_var(key, rest);
        } else if (strcmp(cmd, "UNSET") == 0 && n >= 2) {
            unset_var(key);
        } else if (strcmp(cmd, "RENDER") == 0 && n >= 2) {
            char *rest = line + strlen(cmd) + 1;
            render_text(rest);
        } else if (strcmp(cmd, "VARS") == 0) {
            print_vars();
        }
    }
    return 0;
}