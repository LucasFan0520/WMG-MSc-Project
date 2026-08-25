// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Var {
    char *key;
    char *val;
    struct Var *next;
} Var;

Var *head = NULL;

void set_var(const char *k, const char *v) {
    for (Var *p = head; p; p = p->next) {
        if (strcmp(p->key, k) == 0) {
            free(p->val);
            p->val = strdup(v);
            return;
        }
    }
    Var *p = malloc(sizeof(Var));
    p->key = strdup(k);
    p->val = strdup(v);
    p->next = head;
    head = p;
}

void unset_var(const char *k) {
    Var *p = head;
    Var *prev = NULL;
    while (p) {
        if (strcmp(p->key, k) == 0) {
            Var *next = p->next;
            free(p->key);
            free(p->val);
            free(p);
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }
            return;
        }
        prev = p;
        p = p->next;
    }
}

void render_text(const char *text) {
    const char *p = text;
    while (*p) {
        if (*p == '{') {
            const char *end = strchr(p, '}');
            if (end) {
                char key[1000];
                int len = end - p - 1;
                strncpy(key, p + 1, len);
                key[len] = 0;
                int found = 0;
                for (Var *v = head; v; v = v->next) {
                    if (strcmp(v->key, key) == 0) {
                        printf("%s", v->val);
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    printf("%.*s", len + 2, p);
                }
                p = end + 1;
            } else {
                putchar(*p++);
            }
        } else {
            putchar(*p++);
        }
    }
    putchar('\n');
}

void vars_list() {
    for (Var *v = head; v; v = v->next) {
        printf("%s=%s\n", v->key, v->val);
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char a[1000];
        char b[9000];
        b[0] = 0;
        sscanf(line, "%s %s %[^\n]", cmd, a, b);
        if (strcmp(cmd, "SET") == 0) {
            set_var(a, b);
        } else if (strcmp(cmd, "UNSET") == 0) {
            unset_var(a);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char text[9000];
            sscanf(line, "%*s %[^\n]", text);
            render_text(text);
        } else if (strcmp(cmd, "VARS") == 0) {
            vars_list();
        }
    }
    while (head) {
        Var *n = head->next;
        free(head->key);
        free(head->val);
        free(head);
        head = n;
    }
    return 0;
}