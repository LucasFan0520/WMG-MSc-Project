/* F004.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Var {
    char *key;
    char *value;
    struct Var *next;
} Var;

Var *head = NULL;

Var* find_var(const char *key) {
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
    } else {
        v = malloc(sizeof(Var));
        v->key = strdup(key);
        v->value = strdup(value);
        v->next = head;
        head = v;
    }
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
    char buffer[8192];
    strcpy(buffer, text);
    char *pos = buffer;
    while (*pos) {
        if (*pos == '{') {
            char *end = strchr(pos, '}');
            if (end) {
                *end = '\0';
                char *key = pos + 1;
                Var *v = find_var(key);
                if (v) {
                    printf("%s", v->value);
                } else {
                    printf("{%s}", key);
                }
                pos = end + 1;
                continue;
            }
        }
        putchar(*pos++);
    }
    putchar('\n');
}

void vars_print() {
    Var *v = head;
    while (v) {
        printf("%s %s\n", v->key, v->value);
        v = v->next;
    }
}

void free_all() {
    while (head) {
        Var *tmp = head;
        head = head->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char key[256]; char rest[4096];
        int n = sscanf(line, "%31s %255s %[^\n]", cmd, key, rest);
        if (strcmp(cmd, "SET") == 0 && n == 3) {
            set_var(key, rest);
        } else if (strcmp(cmd, "UNSET") == 0 && n == 2) {
            unset_var(key);
        } else if (strcmp(cmd, "RENDER") == 0 && n >= 2) {
            char text[4096];
            sscanf(line, "%*s %[^\n]", text);
            render_text(text);
        } else if (strcmp(cmd, "VARS") == 0 && n == 1) {
            vars_print();
        }
    }
    free_all();
    return 0;
}