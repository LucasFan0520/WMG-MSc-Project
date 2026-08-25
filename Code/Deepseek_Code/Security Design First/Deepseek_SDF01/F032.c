// filename: F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Setting {
    char *key;
    char *value;
    struct Setting *next;
} Setting;

static Setting *committed = NULL;
static Setting *staged = NULL;
static int in_transaction = 0;

static void free_settings(Setting *s) {
    while (s) {
        Setting *tmp = s;
        s = s->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

static void cleanup(void) {
    free_settings(committed);
    free_settings(staged);
}

static Setting *find_setting(Setting *list, const char *key) {
    for (Setting *s = list; s; s = s->next)
        if (strcmp(s->key, key) == 0)
            return s;
    return NULL;
}

static void set_committed(const char *key, const char *value) {
    Setting *exist = find_setting(committed, key);
    if (exist) {
        char *newv = strdup(value);
        if (!newv) return;
        free(exist->value);
        exist->value = newv;
        return;
    }
    Setting *s = malloc(sizeof(Setting));
    if (!s) return;
    s->key = strdup(key);
    s->value = strdup(value);
    if (!s->key || !s->value) {
        free(s->key);
        free(s->value);
        free(s);
        return;
    }
    s->next = committed;
    committed = s;
}

static void unset_committed(const char *key) {
    Setting *prev = NULL;
    for (Setting *s = committed; s; s = s->next) {
        if (strcmp(s->key, key) == 0) {
            if (prev) prev->next = s->next;
            else committed = s->next;
            free(s->key);
            free(s->value);
            free(s);
            return;
        }
        prev = s;
    }
}

static void tset(const char *key, const char *value) {
    if (!in_transaction) return;
    Setting *exist = find_setting(staged, key);
    if (exist) {
        char *newv = strdup(value);
        if (!newv) return;
        free(exist->value);
        exist->value = newv;
        return;
    }
    Setting *s = malloc(sizeof(Setting));
    if (!s) return;
    s->key = strdup(key);
    s->value = strdup(value);
    if (!s->key || !s->value) {
        free(s->key);
        free(s->value);
        free(s);
        return;
    }
    s->next = staged;
    staged = s;
}

static void commit(void) {
    if (!in_transaction) return;
    // apply staged to committed: overwrite or add
    for (Setting *s = staged; s; s = s->next) {
        set_committed(s->key, s->value);
    }
    free_settings(staged);
    staged = NULL;
    in_transaction = 0;
}

static void abort_transaction(void) {
    if (!in_transaction) return;
    free_settings(staged);
    staged = NULL;
    in_transaction = 0;
}

static void get_key(const char *key) {
    Setting *s = find_setting(staged, key);
    if (!s) s = find_setting(committed, key);
    if (s) printf("%s\n", s->value);
}

static void dump(void) {
    for (Setting *s = committed; s; s = s->next)
        printf("%s %s\n", s->key, s->value);
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    atexit(cleanup);
    while (getline(&line, &len, stdin) != -1) {
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
            if (*arg1) set_committed(arg1, arg2);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            if (!in_transaction) {
                in_transaction = 1;
                // clear any previous staged
                free_settings(staged);
                staged = NULL;
            }
        } else if (strcmp(cmd, "TSET") == 0) {
            if (*arg1) tset(arg1, arg2);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_transaction();
        } else if (strcmp(cmd, "GET") == 0) {
            if (*arg1) get_key(arg1);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump();
        }
    }
    free(line);
    return 0;
}