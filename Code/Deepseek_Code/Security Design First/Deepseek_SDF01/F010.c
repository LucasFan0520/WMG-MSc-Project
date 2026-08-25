// filename: F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ValNode {
    char *value;
    struct ValNode *next;
} ValNode;

typedef struct KeyNode {
    char *key;
    ValNode *values;
    struct KeyNode *next;
} KeyNode;

static KeyNode *keys = NULL;

static void free_values(ValNode *v) {
    while (v) {
        ValNode *tmp = v;
        v = v->next;
        free(tmp->value);
        free(tmp);
    }
}

static void cleanup(void) {
    while (keys) {
        KeyNode *tmp = keys;
        keys = keys->next;
        free(tmp->key);
        free_values(tmp->values);
        free(tmp);
    }
}

static KeyNode *find_key(const char *key) {
    for (KeyNode *k = keys; k; k = k->next)
        if (strcmp(k->key, key) == 0)
            return k;
    return NULL;
}

static void add_value(const char *key, const char *value) {
    KeyNode *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(KeyNode));
        if (!k) return;
        k->key = strdup(key);
        if (!k->key) { free(k); return; }
        k->values = NULL;
        k->next = keys;
        keys = k;
    }
    ValNode *v = malloc(sizeof(ValNode));
    if (!v) return;
    v->value = strdup(value);
    if (!v->value) { free(v); return; }
    v->next = k->values;
    k->values = v;
}

static void remove_value(const char *key, const char *value) {
    KeyNode *k = find_key(key);
    if (!k) return;
    ValNode *prev = NULL;
    for (ValNode *v = k->values; v; v = v->next) {
        if (strcmp(v->value, value) == 0) {
            if (prev) prev->next = v->next;
            else k->values = v->next;
            free(v->value);
            free(v);
            return;
        }
        prev = v;
    }
}

static void erase_key(const char *key) {
    KeyNode *prev = NULL;
    for (KeyNode *k = keys; k; k = k->next) {
        if (strcmp(k->key, key) == 0) {
            if (prev) prev->next = k->next;
            else keys = k->next;
            free(k->key);
            free_values(k->values);
            free(k);
            return;
        }
        prev = k;
    }
}

static void values_print(const char *key) {
    KeyNode *k = find_key(key);
    if (!k) return;
    for (ValNode *v = k->values; v; v = v->next)
        printf("%s\n", v->value);
}

static void keys_print(void) {
    for (KeyNode *k = keys; k; k = k->next)
        printf("%s\n", k->key);
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
        if (strcmp(cmd, "ADD") == 0) {
            if (*arg1 && *arg2) add_value(arg1, arg2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (*arg1 && *arg2) remove_value(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (*arg1) erase_key(arg1);
        } else if (strcmp(cmd, "VALUES") == 0) {
            if (*arg1) values_print(arg1);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_print();
        }
    }
    free(line);
    return 0;
}