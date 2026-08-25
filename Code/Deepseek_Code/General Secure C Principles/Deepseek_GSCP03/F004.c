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
        return;
    }
    Var *new_v = malloc(sizeof(Var));
    if (!new_v) return;
    new_v->key = strdup(key);
    new_v->value = strdup(value);
    new_v->next = head;
    head = new_v;
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
    while (*text) {
        if (*text == '{') {
            const char *end = strchr(text + 1, '}');
            if (end) {
                char key[256];
                int len = end - text - 1;
                if (len < 255) {
                    strncpy(key, text + 1, len);
                    key[len] = '\0';
                    Var *v = find_var(key);
                    if (v) {
                        fputs(v->value, stdout);
                    } else {
                        putchar('{');
                        fputs(key, stdout);
                        putchar('}');
                    }
                    text = end + 1;
                    continue;
                }
            }
        }
        putchar(*text++);
    }
    putchar('\n');
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
        Var *tmp = v;
        v = v->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key_end = p;
            while (*key_end && *key_end != ' ') key_end++;
            if (*key_end) {
                *key_end = '\0';
                char *val = key_end + 1;
                while (*val == ' ') val++;
                set_var(p, val);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            unset_var(p);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            render_text(p);
        } else if (strcmp(cmd, "VARS") == 0) {
            print_vars();
        }
    }
    free_all();
    return 0;
}