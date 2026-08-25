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

Var *find_var(char *key) {
    for (Var *v = head; v; v = v->next) {
        if (strcmp(v->key, key) == 0) return v;
    }
    return NULL;
}

void set_var(char *key, char *value) {
    Var *v = find_var(key);
    if (!v) {
        v = malloc(sizeof(Var));
        v->key = strdup(key);
        v->next = head;
        head = v;
    } else {
        free(v->value);
    }
    v->value = strdup(value);
}

void unset_var(char *key) {
    Var *prev = NULL;
    Var *curr = head;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->key);
            free(curr->value);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void render_text(char *text) {
    char *out = malloc(strlen(text) * 2 + 1);
    char *p = out;
    char *t = text;
    while (*t) {
        if (*t == '{' && t[1] != '}') {
            char *end = strchr(t, '}');
            if (end) {
                char key[100];
                strncpy(key, t + 1, end - t - 1);
                key[end - t - 1] = 0;
                Var *v = find_var(key);
                if (v) {
                    strcpy(p, v->value);
                    p += strlen(v->value);
                    t = end + 1;
                    continue;
                }
            }
        }
        *p++ = *t++;
    }
    *p = 0;
    puts(out);
    free(out);
}

void print_vars() {
    for (Var *v = head; v; v = v->next) {
        printf("%s=%s\n", v->key, v->value);
    }
}

int main() {
    char cmd[20], key[100], text[2000];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "SET") == 0) {
            scanf("%99s", key);
            fgets(text, sizeof(text), stdin);
            while (text[0] == ' ') memmove(text, text + 1, strlen(text));
            text[strcspn(text, "\n")] = 0;
            set_var(key, text);
        } else if (strcmp(cmd, "UNSET") == 0) {
            scanf("%99s", key);
            unset_var(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            fgets(text, sizeof(text), stdin);
            text[strcspn(text, "\n")] = 0;
            render_text(text);
        } else if (strcmp(cmd, "VARS") == 0) {
            print_vars();
        }
    }
    return 0;
}