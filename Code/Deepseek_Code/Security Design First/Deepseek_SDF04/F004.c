// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Variable {
    char *key;
    char *value;
    struct Variable *next;
} Variable;

Variable *head = NULL;

Variable *find_var(const char *key) {
    Variable *v = head;
    while (v) {
        if (strcmp(v->key, key) == 0) return v;
        v = v->next;
    }
    return NULL;
}

void set_var(const char *key, const char *value) {
    Variable *v = find_var(key);
    if (v) {
        char *newval = strdup(value);
        if (!newval) return;
        free(v->value);
        v->value = newval;
        return;
    }
    Variable *nv = malloc(sizeof(Variable));
    if (!nv) return;
    nv->key = strdup(key);
    nv->value = strdup(value);
    nv->next = NULL;
    if (!nv->key || !nv->value) {
        free(nv->key); free(nv->value); free(nv);
        return;
    }
    if (!head) head = nv;
    else {
        Variable *last = head;
        while (last->next) last = last->next;
        last->next = nv;
    }
}

void unset_var(const char *key) {
    Variable *v = head, *prev = NULL;
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
    const char *p = text;
    char key[256];
    while (*p) {
        if (*p == '{') {
            p++;
            const char *start = p;
            while (*p && *p != '}') p++;
            if (*p == '}') {
                int len = p - start;
                if (len > 0 && len < 255) {
                    strncpy(key, start, len);
                    key[len] = '\0';
                    Variable *v = find_var(key);
                    if (v) printf("%s", v->value);
                    else printf("{%.*s}", len, start);
                } else {
                    printf("{%.*s}", (int)(p - start), start);
                }
                p++;
            } else {
                printf("{");
            }
        } else {
            putchar(*p++);
        }
    }
    putchar('\n');
}

void vars_list(void) {
    Variable *v = head;
    while (v) {
        printf("%s %s\n", v->key, v->value);
        v = v->next;
    }
}

void free_all(void) {
    Variable *v = head;
    while (v) {
        Variable *next = v->next;
        free(v->key);
        free(v->value);
        free(v);
        v = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], key[256], value[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, value) == 2)
                set_var(key, value);
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                unset_var(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            if (sscanf(line, "%*s %4095[^\n]", value) == 1)
                render_text(value);
        } else if (strcmp(cmd, "VARS") == 0) {
            vars_list();
        }
    }
    free(line);
    free_all();
    return 0;
}