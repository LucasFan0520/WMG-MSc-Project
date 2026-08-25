// filename: F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct VarNode {
    char *key;
    char *value;
    struct VarNode *next;
} VarNode;

static VarNode *head = NULL;

static void cleanup(void) {
    while (head) {
        VarNode *tmp = head;
        head = head->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

static VarNode *find_var(const char *key) {
    for (VarNode *n = head; n; n = n->next)
        if (strcmp(n->key, key) == 0)
            return n;
    return NULL;
}

static void set_var(const char *key, const char *value) {
    VarNode *exist = find_var(key);
    if (exist) {
        char *newv = strdup(value);
        if (!newv) return;
        free(exist->value);
        exist->value = newv;
        return;
    }
    VarNode *n = malloc(sizeof(VarNode));
    if (!n) return;
    n->key = strdup(key);
    n->value = strdup(value);
    if (!n->key || !n->value) {
        free(n->key);
        free(n->value);
        free(n);
        return;
    }
    n->next = head;
    head = n;
}

static void unset_var(const char *key) {
    VarNode *prev = NULL;
    for (VarNode *n = head; n; n = n->next) {
        if (strcmp(n->key, key) == 0) {
            if (prev) prev->next = n->next;
            else head = n->next;
            free(n->key);
            free(n->value);
            free(n);
            return;
        }
        prev = n;
    }
}

static void render_text(const char *text) {
    while (*text) {
        if (*text == '{') {
            const char *end = strchr(text + 1, '}');
            if (end) {
                size_t len = end - text - 1;
                char *key = malloc(len + 1);
                if (key) {
                    memcpy(key, text + 1, len);
                    key[len] = '\0';
                    VarNode *v = find_var(key);
                    if (v) {
                        fputs(v->value, stdout);
                    } else {
                        putchar('{');
                        fwrite(text + 1, 1, len, stdout);
                        putchar('}');
                    }
                    free(key);
                } else {
                    putchar(*text);
                }
                text = end + 1;
                continue;
            }
        }
        putchar(*text++);
    }
    putchar('\n');
}

static void vars_print(void) {
    for (VarNode *n = head; n; n = n->next)
        printf("%s %s\n", n->key, n->value);
}

int main(void) {
    char *line = NULL;
    size_t linelen = 0;
    atexit(cleanup);
    while (getline(&line, &linelen, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *p = line;
        while (*p == ' ') ++p;
        char *cmd = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg1 = p;
        while (*p && *p != ' ') ++p;
        if (*p) *p++ = '\0';
        while (*p == ' ') ++p;
        char *arg2 = p;
        if (strcmp(cmd, "SET") == 0) {
            if (*arg1) set_var(arg1, arg2);
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (*arg1) unset_var(arg1);
        } else if (strcmp(cmd, "RENDER") == 0) {
            render_text(arg1);
        } else if (strcmp(cmd, "VARS") == 0) {
            vars_print();
        }
    }
    free(line);
    return 0;
}