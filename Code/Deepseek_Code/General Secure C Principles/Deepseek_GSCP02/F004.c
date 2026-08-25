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
        v->value = strdup(value);
    } else {
        v = malloc(sizeof(Var));
        if (!v) return;
        v->key = strdup(key);
        v->value = strdup(value);
        v->next = head;
        head = v;
    }
}

void unset_var(const char *key) {
    Var *prev = NULL;
    Var *v = head;
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
    char *out = malloc(strlen(text) * 2 + 1);
    if (!out) return;
    char *p = out;
    const char *s = text;
    while (*s) {
        if (*s == '{') {
            const char *end = strchr(s, '}');
            if (end) {
                size_t keylen = end - s - 1;
                char *key = malloc(keylen + 1);
                if (key) {
                    strncpy(key, s + 1, keylen);
                    key[keylen] = '\0';
                    Var *v = find_var(key);
                    if (v) {
                        strcpy(p, v->value);
                        p += strlen(v->value);
                    } else {
                        strncpy(p, s, end - s + 1);
                        p += end - s + 1;
                    }
                    free(key);
                    s = end + 1;
                    continue;
                }
            }
        }
        *p++ = *s++;
    }
    *p = '\0';
    for (char *q = out; *q; q++) {
        putchar(*q == ' ' ? '_' : *q);
    }
    putchar('\n');
    free(out);
}

void print_vars(void) {
    Var *v = head;
    while (v) {
        printf("%s ", v->key);
        for (char *p = v->value; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        v = v->next;
    }
}

void free_all(void) {
    while (head) {
        Var *next = head->next;
        free(head->key);
        free(head->value);
        free(head);
        head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "SET") == 0) {
            char key[256], val[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", key, val) == 2) {
                set_var(key, val);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) == 1) {
                unset_var(key);
            }
        } else if (strcmp(cmd, "RENDER") == 0) {
            char text[1024];
            if (sscanf(line, "%*s %1023[^\n]", text) == 1) {
                render_text(text);
            }
        } else if (strcmp(cmd, "VARS") == 0) {
            print_vars();
        }
    }

    free(line);
    free_all();
    return 0;
}