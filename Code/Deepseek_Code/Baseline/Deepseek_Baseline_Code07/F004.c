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
Var *tail = NULL;

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
        v->value = malloc(strlen(value)+1);
        strcpy(v->value, value);
    } else {
        Var *new = malloc(sizeof(Var));
        new->key = malloc(strlen(key)+1);
        strcpy(new->key, key);
        new->value = malloc(strlen(value)+1);
        strcpy(new->value, value);
        new->next = NULL;
        if (tail) tail->next = new;
        else head = new;
        tail = new;
    }
}

void unset_var(const char *key) {
    Var *prev = NULL;
    Var *v = head;
    while (v) {
        if (strcmp(v->key, key) == 0) {
            if (prev) prev->next = v->next;
            else head = v->next;
            if (v == tail) tail = prev;
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
            const char *end = strchr(p+1, '}');
            if (end) {
                char *key = malloc(end - p);
                strncpy(key, p+1, end-p-1);
                key[end-p-1] = 0;
                Var *v = find_var(key);
                if (v) printf("%s", v->value);
                else {
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

void vars_print(void) {
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
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(rest, " ");
            if (!key) continue;
            char *value = strtok(NULL, "");
            if (!value) value = "";
            set_var(key, value);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = strtok(rest, " ");
            if (key) unset_var(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *text = rest;
            render_text(text);
        } else if (strcmp(cmd, "VARS") == 0) {
            vars_print();
        }
    }
    free_all();
    return 0;
}