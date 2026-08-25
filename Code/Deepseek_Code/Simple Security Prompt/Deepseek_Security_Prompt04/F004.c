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

Var *find_var(char *key) {
    for (Var *v = head; v; v = v->next)
        if (strcmp(v->key, key) == 0) return v;
    return NULL;
}

void set_var(char *key, char *value) {
    Var *v = find_var(key);
    if (v) {
        free(v->value);
        v->value = strdup(value);
    } else {
        Var *new = malloc(sizeof(Var));
        new->key = strdup(key);
        new->value = strdup(value);
        new->next = head;
        head = new;
    }
}

void unset_var(char *key) {
    Var *prev = NULL;
    for (Var *v = head; v; v = v->next) {
        if (strcmp(v->key, key) == 0) {
            if (prev) prev->next = v->next;
            else head = v->next;
            free(v->key);
            free(v->value);
            free(v);
            return;
        }
        prev = v;
    }
}

void render_text(char *text) {
    char *p = text;
    while (*p) {
        if (*p == '{') {
            char *end = strchr(p, '}');
            if (end) {
                int keylen = end - p - 1;
                char key[256];
                if (keylen < 256) {
                    strncpy(key, p + 1, keylen);
                    key[keylen] = '\0';
                    Var *v = find_var(key);
                    if (v) fputs(v->value, stdout);
                    else fwrite(p, 1, end - p + 1, stdout);
                    p = end + 1;
                    continue;
                }
            }
        }
        putchar(*p++);
    }
    putchar('\n');
}

void vars_all(void) {
    for (Var *v = head; v; v = v->next)
        printf("%s %s\n", v->key, v->value);
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
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "SET") == 0 && n == 3) {
            set_var(arg1, arg2);
        } else if (strcmp(cmd, "UNSET") == 0 && n == 2) {
            unset_var(arg1);
        } else if (strcmp(cmd, "RENDER") == 0 && n >= 2) {
            // arg1 is the rest of the line; if n==2, arg2 is empty? We need full text.
            // Use sscanf with %[^\n] but if text starts with space, we lose it.
            // Better: get the rest after command.
            char *rest = line;
            while (*rest && *rest != ' ') rest++;
            while (*rest == ' ') rest++;
            render_text(rest);
        } else if (strcmp(cmd, "VARS") == 0 && n == 1) {
            vars_all();
        }
    }
    free(line);
    free_all();
    return 0;
}