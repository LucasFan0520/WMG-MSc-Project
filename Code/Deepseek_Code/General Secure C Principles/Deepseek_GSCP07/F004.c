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

Var* find_var(const char *key) {
    Var *v = head;
    while (v) {
        if (strcmp(v->key, key) == 0) return v;
        v = v->next;
    }
    return NULL;
}

void free_var(Var *v) {
    free(v->key);
    free(v->value);
    free(v);
}

void handle_set(char *cmd) {
    char *key = strtok(NULL, " ");
    if (!key) return;
    char *val = strtok(NULL, "");
    if (!val) val = "";
    Var *v = find_var(key);
    if (v) {
        free(v->value);
        v->value = strdup(val);
    } else {
        Var *new = malloc(sizeof(Var));
        new->key = strdup(key);
        new->value = strdup(val);
        new->next = NULL;
        if (!head) head = new;
        else {
            Var *last = head;
            while (last->next) last = last->next;
            last->next = new;
        }
    }
}

void handle_unset(char *cmd) {
    char *key = strtok(NULL, " ");
    if (!key) return;
    Var *v = head, *prev = NULL;
    while (v) {
        if (strcmp(v->key, key) == 0) {
            if (prev) prev->next = v->next;
            else head = v->next;
            free_var(v);
            return;
        }
        prev = v;
        v = v->next;
    }
}

void handle_render(char *cmd) {
    char *text = strtok(NULL, "");
    if (!text) return;
    char *p = text;
    while (*p) {
        if (*p == '{') {
            char *end = strchr(p, '}');
            if (end) {
                *end = '\0';
                char *key = p + 1;
                Var *v = find_var(key);
                if (v) printf("%s", v->value);
                else printf("{%s}", key);
                p = end + 1;
                continue;
            }
        }
        putchar(*p);
        p++;
    }
    putchar('\n');
}

void vars() {
    Var *v = head;
    while (v) {
        printf("%s %s\n", v->key, v->value);
        v = v->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "SET") == 0) handle_set(cmd);
        else if (strcmp(cmd, "UNSET") == 0) handle_unset(cmd);
        else if (strcmp(cmd, "RENDER") == 0) handle_render(cmd);
        else if (strcmp(cmd, "VARS") == 0) vars();
    }
    while (head) {
        Var *tmp = head;
        head = head->next;
        free_var(tmp);
    }
    return 0;
}