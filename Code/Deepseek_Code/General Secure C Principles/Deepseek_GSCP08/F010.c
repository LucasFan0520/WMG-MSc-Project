// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Val { char *val; struct Val *next; } Val;
typedef struct Key { char *key; Val *vals; struct Key *next; } Key;
Key *keys = NULL;

Key *find_key(const char *key) {
    for (Key *k = keys; k; k = k->next)
        if (strcmp(k->key, key) == 0) return k;
    return NULL;
}

void add_value(const char *key, const char *val) {
    Key *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(Key));
        if (!k) return;
        k->key = strdup(key); k->vals = NULL; k->next = keys; keys = k;
    }
    Val *v = malloc(sizeof(Val));
    if (!v) return;
    v->val = strdup(val);
    v->next = k->vals;
    k->vals = v;
}

void remove_value(const char *key, const char *val) {
    Key *k = find_key(key);
    if (!k) return;
    Val *prev = NULL;
    for (Val *v = k->vals; v; prev = v, v = v->next) {
        if (strcmp(v->val, val) == 0) {
            if (prev) prev->next = v->next; else k->vals = v->next;
            free(v->val); free(v);
            return;
        }
    }
}

void erase_key(const char *key) {
    Key *prev = NULL;
    for (Key *k = keys; k; prev = k, k = k->next) {
        if (strcmp(k->key, key) == 0) {
            if (prev) prev->next = k->next; else keys = k->next;
            while (k->vals) { Val *v = k->vals; k->vals = v->next; free(v->val); free(v); }
            free(k->key); free(k);
            return;
        }
    }
}

void values_key(const char *key) {
    Key *k = find_key(key);
    if (!k) return;
    for (Val *v = k->vals; v; v = v->next) printf("%s\n", v->val);
}

void keys_list(void) {
    for (Key *k = keys; k; k = k->next) printf("%s\n", k->key);
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            char key[256], val[4096];
            if (sscanf(rest, "%255s", key) == 1) {
                char *v = rest + strlen(key);
                while (*v == ' ') v++;
                strcpy(val, v);
                add_value(key, val);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char key[256], val[4096];
            if (sscanf(rest, "%255s", key) == 1) {
                char *v = rest + strlen(key);
                while (*v == ' ') v++;
                strcpy(val, v);
                remove_value(key, val);
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) == 1) erase_key(key);
        } else if (strcmp(cmd, "VALUES") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) == 1) values_key(key);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_list();
        }
    }
    while (keys) { Key *k = keys; keys = k->next; while (k->vals) { Val *v = k->vals; k->vals = v->next; free(v->val); free(v); } free(k->key); free(k); }
    return 0;
}