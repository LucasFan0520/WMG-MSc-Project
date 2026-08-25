// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

typedef struct {
    char *key;
    char *value;
} Setting;

typedef struct {
    Setting *items;
    size_t count;
    size_t capacity;
} Config;

typedef struct {
    Setting *items;
    size_t count;
    size_t capacity;
} Transaction;

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

static void config_init(Config *c) {
    c->items = NULL;
    c->count = 0;
    c->capacity = 0;
}

static void config_free(Config *c) {
    for (size_t i = 0; i < c->count; i++) {
        free(c->items[i].key);
        free(c->items[i].value);
    }
    free(c->items);
    c->items = NULL;
    c->count = 0;
    c->capacity = 0;
}

static int config_find(Config *c, const char *key) {
    for (size_t i = 0; i < c->count; i++) {
        if (strcmp(c->items[i].key, key) == 0)
            return (int)i;
    }
    return -1;
}

static void config_set(Config *c, const char *key, const char *value) {
    int idx = config_find(c, key);
    if (idx >= 0) {
        char *new = safe_strdup(value);
        if (!new) return;
        free(c->items[idx].value);
        c->items[idx].value = new;
        return;
    }
    if (c->count == c->capacity) {
        size_t newcap = c->capacity ? c->capacity * 2 : 4;
        Setting *tmp = realloc(c->items, newcap * sizeof(Setting));
        if (!tmp) return;
        c->items = tmp;
        c->capacity = newcap;
    }
    c->items[c->count].key = safe_strdup(key);
    c->items[c->count].value = safe_strdup(value);
    if (c->items[c->count].key && c->items[c->count].value)
        c->count++;
    else {
        free(c->items[c->count].key);
        free(c->items[c->count].value);
    }
}

static void config_unset(Config *c, const char *key) {
    int idx = config_find(c, key);
    if (idx < 0) return;
    free(c->items[idx].key);
    free(c->items[idx].value);
    for (size_t i = idx; i < c->count - 1; i++)
        c->items[i] = c->items[i+1];
    c->count--;
}

static void config_get(Config *c, const char *key) {
    int idx = config_find(c, key);
    if (idx < 0) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", c->items[idx].value);
}

static void config_dump(Config *c) {
    for (size_t i = 0; i < c->count; i++) {
        printf("%s %s\n", c->items[i].key, c->items[i].value);
    }
}

static void trans_init(Transaction *t) {
    t->items = NULL;
    t->count = 0;
    t->capacity = 0;
}

static void trans_free(Transaction *t) {
    for (size_t i = 0; i < t->count; i++) {
        free(t->items[i].key);
        free(t->items[i].value);
    }
    free(t->items);
    t->items = NULL;
    t->count = 0;
    t->capacity = 0;
}

static void trans_set(Transaction *t, const char *key, const char *value) {
    int idx = -1;
    for (size_t i = 0; i < t->count; i++) {
        if (strcmp(t->items[i].key, key) == 0) { idx = (int)i; break; }
    }
    if (idx >= 0) {
        char *new = safe_strdup(value);
        if (!new) return;
        free(t->items[idx].value);
        t->items[idx].value = new;
        return;
    }
    if (t->count == t->capacity) {
        size_t newcap = t->capacity ? t->capacity * 2 : 4;
        Setting *tmp = realloc(t->items, newcap * sizeof(Setting));
        if (!tmp) return;
        t->items = tmp;
        t->capacity = newcap;
    }
    t->items[t->count].key = safe_strdup(key);
    t->items[t->count].value = safe_strdup(value);
    if (t->items[t->count].key && t->items[t->count].value)
        t->count++;
    else {
        free(t->items[t->count].key);
        free(t->items[t->count].value);
    }
}

static void trans_commit(Config *c, Transaction *t) {
    for (size_t i = 0; i < t->count; i++) {
        config_set(c, t->items[i].key, t->items[i].value);
    }
    trans_free(t);
    trans_init(t);
}

static void trans_abort(Transaction *t) {
    trans_free(t);
    trans_init(t);
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
    Config c;
    config_init(&c);
    Transaction t;
    trans_init(&t);
    int in_trans = 0;
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        char cmd[32], a1[256], a2[4096];
        parse_cmd(line, cmd, sizeof(cmd), a1, sizeof(a1), a2, sizeof(a2));
        if (strcmp(cmd, "SET") == 0) {
            if (in_trans) {
                trans_set(&t, a1, a2);
            } else {
                config_set(&c, a1, a2);
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            if (!in_trans) {
                trans_free(&t);
                trans_init(&t);
                in_trans = 1;
            }
        } else if (strcmp(cmd, "TSET") == 0) {
            if (in_trans) {
                trans_set(&t, a1, a2);
            }
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_trans) {
                trans_commit(&c, &t);
                in_trans = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_trans) {
                trans_abort(&t);
                in_trans = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            if (in_trans) {
                // check transaction first
                int idx = -1;
                for (size_t i = 0; i < t.count; i++) {
                    if (strcmp(t.items[i].key, a1) == 0) { idx = (int)i; break; }
                }
                if (idx >= 0) {
                    printf("%s\n", t.items[idx].value);
                } else {
                    config_get(&c, a1);
                }
            } else {
                config_get(&c, a1);
            }
        } else if (strcmp(cmd, "DUMP") == 0) {
            if (!in_trans) {
                config_dump(&c);
            }
        }
    }
    config_free(&c);
    trans_free(&t);
    return 0;
}