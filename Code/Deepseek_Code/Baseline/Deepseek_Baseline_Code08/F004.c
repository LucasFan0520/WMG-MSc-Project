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
    Var *cur = head;
    while (cur) {
        if (strcmp(cur->key, key) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void set_var(const char *key, const char *value) {
    Var *v = find_var(key);
    if (v) {
        free(v->value);
        v->value = strdup(value);
    } else {
        Var *new = malloc(sizeof(Var));
        new->key = strdup(key);
        new->value = strdup(value);
        new->next = NULL;
        if (!head) head = new;
        else {
            Var *cur = head;
            while (cur->next) cur = cur->next;
            cur->next = new;
        }
    }
}

void unset_var(const char *key) {
    Var *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->key);
            free(cur->value);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void free_all() {
    Var *cur = head;
    while (cur) {
        Var *tmp = cur;
        cur = cur->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

void render(const char *text) {
    while (*text) {
        if (*text == '{') {
            const char *end = strchr(text + 1, '}');
            if (end) {
                size_t len = end - text - 1;
                char *key = malloc(len + 1);
                strncpy(key, text + 1, len);
                key[len] = 0;
                Var *v = find_var(key);
                if (v) printf("%s", v->value);
                else printf("{%.*s}", (int)len, text + 1);
                free(key);
                text = end + 1;
                continue;
            }
        }
        putchar(*text);
        text++;
    }
    putchar('\n');
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            if (!key) continue;
            char *val = strtok(NULL, "");
            if (!val) val = "";
            set_var(key, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) unset_var(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text = strtok(NULL, "");
            if (!text) text = "";
            render(text);
        } else if (strcmp(cmd, "VARS") == 0) {
            Var *cur = head;
            while (cur) {
                printf("%s %s\n", cur->key, cur->value);
                cur = cur->next;
            }
        }
    }
    free_all();
    return 0;
}