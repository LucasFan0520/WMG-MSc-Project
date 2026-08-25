// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Var { char *key, *val; struct Var *next; } Var;
Var *head = NULL;

void set_var(const char *key, const char *val) {
    for (Var *v = head; v; v = v->next) {
        if (strcmp(v->key, key) == 0) {
            free(v->val);
            v->val = strdup(val);
            return;
        }
    }
    Var *v = malloc(sizeof(Var));
    v->key = strdup(key);
    v->val = strdup(val);
    v->next = head;
    head = v;
}

void unset_var(const char *key) {
    Var **p = &head;
    while (*p) {
        if (strcmp((*p)->key, key) == 0) {
            Var *tmp = *p;
            *p = tmp->next;
            free(tmp->key);
            free(tmp->val);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void render(const char *text) {
    char out[8192] = "";
    const char *p = text;
    while (*p) {
        if (*p == '{') {
            const char *end = strchr(p, '}');
            if (end) {
                char key[256];
                size_t len = end - p - 1;
                if (len < sizeof(key)) {
                    strncpy(key, p + 1, len);
                    key[len] = 0;
                    int found = 0;
                    for (Var *v = head; v; v = v->next) {
                        if (strcmp(v->key, key) == 0) {
                            strcat(out, v->val);
                            found = 1;
                            break;
                        }
                    }
                    if (!found) { strncat(out, p, len + 2); }
                    p = end + 1;
                    continue;
                }
            }
        }
        char c[2] = {*p, 0};
        strcat(out, c);
        p++;
    }
    printf("%s\n", out);
}

void vars() {
    for (Var *v = head; v; v = v->next) printf("%s %s\n", v->key, v->val);
}

void cleanup() {
    while (head) {
        Var *tmp = head->next;
        free(head->key);
        free(head->val);
        free(head);
        head = tmp;
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) set_var(a1, t + 1); }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (sscanf(line, "%*s %255s", a1) == 1) unset_var(a1);
        } else if (strcmp(cmd, "RENDER") == 0) {
            char *t = strchr(line, ' ');
            if (t) render(t + 1);
        } else if (strcmp(cmd, "VARS") == 0) vars();
    }
    cleanup();
    return 0;
}