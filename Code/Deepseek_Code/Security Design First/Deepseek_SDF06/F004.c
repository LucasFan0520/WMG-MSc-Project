// F004.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *key;
    char *value;
} Variable;

static Variable *vars = NULL;
static size_t count = 0;
static size_t capacity = 0;

static ssize_t find_var(const char *key) {
    for (size_t i = 0; i < count; i++)
        if (strcmp(vars[i].key, key) == 0)
            return (ssize_t)i;
    return -1;
}

static void set_var(const char *key, const char *value) {
    ssize_t idx = find_var(key);
    if (idx != -1) {
        free(vars[idx].value);
        vars[idx].value = strdup(value);
        if (!vars[idx].value) { fprintf(stderr, "Memory error\n"); exit(1); }
        return;
    }
    if (count == capacity) {
        capacity = capacity ? capacity * 2 : 4;
        Variable *new = realloc(vars, capacity * sizeof(Variable));
        if (!new) { fprintf(stderr, "Memory error\n"); exit(1); }
        vars = new;
    }
    vars[count].key = strdup(key);
    vars[count].value = strdup(value);
    if (!vars[count].key || !vars[count].value) { fprintf(stderr, "Memory error\n"); exit(1); }
    count++;
}

static void unset_var(const char *key) {
    ssize_t idx = find_var(key);
    if (idx == -1) return;
    free(vars[idx].key);
    free(vars[idx].value);
    for (size_t i = idx; i < count - 1; i++)
        vars[i] = vars[i + 1];
    count--;
}

static void render(const char *text) {
    size_t out_len = 0, out_cap = 1024;
    char *out = malloc(out_cap);
    if (!out) { fprintf(stderr, "Memory error\n"); exit(1); }
    out[0] = '\0';

    for (const char *p = text; *p; p++) {
        if (*p == '{') {
            const char *end = strchr(p + 1, '}');
            if (end) {
                size_t key_len = end - (p + 1);
                char *key = malloc(key_len + 1);
                if (!key) { fprintf(stderr, "Memory error\n"); exit(1); }
                strncpy(key, p + 1, key_len);
                key[key_len] = '\0';
                ssize_t idx = find_var(key);
                free(key);
                if (idx != -1) {
                    const char *val = vars[idx].value;
                    size_t val_len = strlen(val);
                    while (out_len + val_len + 1 >= out_cap) {
                        out_cap *= 2;
                        char *new_out = realloc(out, out_cap);
                        if (!new_out) { fprintf(stderr, "Memory error\n"); exit(1); }
                        out = new_out;
                    }
                    strcpy(out + out_len, val);
                    out_len += val_len;
                    p = end;
                    continue;
                }
            }
        }
        // normal char
        if (out_len + 2 >= out_cap) {
            out_cap *= 2;
            char *new_out = realloc(out, out_cap);
            if (!new_out) { fprintf(stderr, "Memory error\n"); exit(1); }
            out = new_out;
        }
        out[out_len++] = *p;
        out[out_len] = '\0';
    }
    printf("%s\n", out);
    free(out);
}

static void vars_list(void) {
    for (size_t i = 0; i < count; i++)
        printf("%s %s\n", vars[i].key, vars[i].value);
}

static void free_all(void) {
    for (size_t i = 0; i < count; i++) {
        free(vars[i].key);
        free(vars[i].value);
    }
    free(vars);
}

int main(void) {
    char *line = NULL;
    size_t line_len = 0;
    ssize_t nread;
    char cmd[32], key[256], val[4096];

    while ((nread = getline(&line, &line_len, stdin)) != -1) {
        if (line[nread - 1] == '\n') line[nread - 1] = '\0';
        if (strcmp(line, "END") == 0) break;

        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, val) == 2)
                set_var(key, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                unset_var(key);
        } else if (strcmp(cmd, "RENDER") == 0) {
            if (sscanf(line, "%*s %4095[^\n]", val) == 1)
                render(val);
        } else if (strcmp(cmd, "VARS") == 0) {
            vars_list();
        }
    }

    free(line);
    free_all();
    return 0;
}