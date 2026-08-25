// F004.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *key;
    char *value;
} Var;

typedef struct {
    Var *items;
    size_t count;
    size_t capacity;
} VarTable;

static void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len && s[len-1] == '\n') s[len-1] = '\0';
}

static char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *p = malloc(len + 1);
    if (p) memcpy(p, s, len + 1);
    return p;
}

static void vt_init(VarTable *v) {
    v->items = NULL;
    v->count = 0;
    v->capacity = 0;
}

static void vt_free(VarTable *v) {
    for (size_t i = 0; i < v->count; i++) {
        free(v->items[i].key);
        free(v->items[i].value);
    }
    free(v->items);
    v->items = NULL;
    v->count = 0;
    v->capacity = 0;
}

static int vt_find(VarTable *v, const char *key) {
    for (size_t i = 0; i < v->count; i++) {
        if (strcmp(v->items[i].key, key) == 0)
            return (int)i;
    }
    return -1;
}

static void vt_set(VarTable *v, const char *key, const char *val) {
    int idx = vt_find(v, key);
    if (idx >= 0) {
        char *new = safe_strdup(val);
        if (!new) return;
        free(v->items[idx].value);
        v->items[idx].value = new;
        return;
    }
    if (v->count == v->capacity) {
        size_t newcap = v->capacity ? v->capacity * 2 : 4;
        Var *tmp = realloc(v->items, newcap * sizeof(Var));
        if (!tmp) return;
        v->items = tmp;
        v->capacity = newcap;
    }
    v->items[v->count].key = safe_strdup(key);
    v->items[v->count].value = safe_strdup(val);
    if (v->items[v->count].key && v->items[v->count].value)
        v->count++;
    else {
        free(v->items[v->count].key);
        free(v->items[v->count].value);
    }
}

static void vt_unset(VarTable *v, const char *key) {
    int idx = vt_find(v, key);
    if (idx < 0) return;
    free(v->items[idx].key);
    free(v->items[idx].value);
    for (size_t i = idx; i < v->count - 1; i++)
        v->items[i] = v->items[i+1];
    v->count--;
}

static char *vt_get(VarTable *v, const char *key) {
    int idx = vt_find(v, key);
    return idx >= 0 ? v->items[idx].value : NULL;
}

static void vt_vars(VarTable *v) {
    for (size_t i = 0; i < v->count; i++) {
        printf("%s\n", v->items[i].key);
    }
}

static void render(VarTable *v, const char *text) {
    const char *p = text;
    while (*p) {
        if (*p == '{') {
            const char *start = p + 1;
            const char *end = strchr(start, '}');
            if (end) {
                size_t keylen = end - start;
                char key[256];
                if (keylen < sizeof(key)) {
                    memcpy(key, start, keylen);
                    key[keylen] = '\0';
                    char *val = vt_get(v, key);
                    if (val) {
                        fputs(val, stdout);
                        p = end + 1;
                        continue;
                    }
                }
            }
        }
        putchar(*p);
        p++;
    }
    putchar('\n');
}

static void parse_cmd(const char *line, char *cmd, size_t cmdsz,
                      char *a1, size_t a1sz, char *a2, size_t a2sz) {
    const char *p = line;
    while (isspace(*p)) p++;
    size_t i = 0;
    while (*p && !isspace(*p) && i < cmdsz - 1) cmd[i++] = *p++;
    cmd[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && !isspace(*p) && i < a1sz - 1) a1[i++] = *p++;
    a1[i] = '\0';
    while (isspace(*p)) p++;
    i = 0;
    while (*p && i < a2sz - 1) a2[i++] = *p++;
    a2[i] = '\0';
}

int main(void) {
    VarTable v;
    vt_init(&v);
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "SET") == 0) {
            vt_set(&v, a1, a2);
        } else if (strcmp(cmd, "UNSET") == 0) {
            vt_unset(&v, a1);
        } else if (strcmp(cmd, "RENDER") == 0) {
            render(&v, a1); // text is in a1
        } else if (strcmp(cmd, "VARS") == 0) {
            vt_vars(&v);
        }
    }
    vt_free(&v);
    return 0;
}