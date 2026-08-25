/*
 * F004.c
 */
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
    } else {
        v = malloc(sizeof(Var));
        v->key = strdup(key);
        v->value = strdup(value);
        v->next = vars;
        vars = v;
    }
}

void unset_var(const char *key) {
    Var *prev = NULL, *v = vars;
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
    char buffer[4096];
    strcpy(buffer, text);
    Var *v = vars;
    while (v) {
        char pattern[256];
        sprintf(pattern, "{%s}", v->key);
        char *pos;
        while ((pos = strstr(buffer, pattern)) != NULL) {
            char tmp[4096];
            strcpy(tmp, pos + strlen(pattern));
            *pos = 0;
            strcat(buffer, v->value);
            strcat(buffer, tmp);
        }
        v = v->next;
    }
    printf("%s\n", buffer);
}

void print_vars(void) {
    Var *v = vars;
    while (v) {
        printf("%s %s\n", v->key, v->value);
        v = v->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "SET") == 0) {
            char key[256], value[4096];
            if (sscanf(rest, "%255s", key) == 1) {
                char *valstart = rest + strlen(key);
                while (*valstart == ' ') valstart++;
                set_var(key, valstart);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) == 1) unset_var(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            render(rest);
        } else if (strcmp(cmd, "VARS") == 0) {
            print_vars();
        }
    }
    return 0;
}