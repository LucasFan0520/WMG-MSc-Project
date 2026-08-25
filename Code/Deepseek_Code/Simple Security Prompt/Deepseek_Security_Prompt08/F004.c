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
    } else {
        Var *new = malloc(sizeof(Var));
        new->key = malloc(strlen(key) + 1);
        strcpy(new->key, key);
        new->value = malloc(strlen(value) + 1);
        strcpy(new->value, value);
        new->next = head;
        head = new;
    }
}

void unset_var(const char *key) {
    Var *prev = NULL, *curr = head;
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

void render_text(const char *text) {
    char *p = strdup(text);
    char *cursor = p;
    while (*cursor) {
        if (*cursor == '{') {
            char *end = strchr(cursor, '}');
            if (end) {
                *end = '\0';
                char *key = cursor + 1;
                Var *v = find_var(key);
                if (v) {
                    printf("%s", v->value);
                } else {
                    printf("{%s}", key);
                }
                cursor = end + 1;
                continue;
            }
        }
        putchar(*cursor);
        cursor++;
    }
    putchar('\n');
    free(p);
}

void print_vars(void) {
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
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, "");
            if (key && value) set_var(key, value);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = strtok(NULL, " ");
            if (key) unset_var(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text = strtok(NULL, "");
            if (text) render_text(text);
        } else if (strcmp(cmd, "VARS") == 0) {
            print_vars();
        }
    }
    free(line);
    free_all();
    return 0;
}