// F010.c
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

KeyNode *keys = NULL;

KeyNode *find_key(const char *key) {
    KeyNode *k = keys;
    while (k) {
        if (strcmp(k->key, key) == 0) return k;
        k = k->next;
    }
    return NULL;
}

void add_value(KeyNode *k, const char *value) {
    ValNode *v = malloc(sizeof(ValNode));
    v->value = malloc(strlen(value) + 1);
    strcpy(v->value, value);
    v->next = k->values;
    k->values = v;
}

void add_key_value(const char *key, const char *value) {
    KeyNode *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(KeyNode));
        k->key = malloc(strlen(key) + 1);
        strcpy(k->key, key);
        k->values = NULL;
        k->next = keys;
        keys = k;
    }
    add_value(k, value);
}

void remove_value(KeyNode *k, const char *value) {
    ValNode *v = k->values, *prev = NULL;
    while (v) {
        if (strcmp(v->value, value) == 0) {
            if (prev) prev->next = v->next;
            else k->values = v->next;
            free(v->value);
            free(v);
            return;
        }
        prev = v;
        v = v->next;
    }
}

void erase_key(const char *key) {
    KeyNode *k = keys, *prev = NULL;
    while (k) {
        if (strcmp(k->key, key) == 0) {
            if (prev) prev->next = k->next;
            else keys = k->next;
            ValNode *v = k->values;
            while (v) {
                ValNode *tmp = v->next;
                free(v->value);
                free(v);
                v = tmp;
            }
            free(k->key);
            free(k);
            return;
        }
        prev = k;
        k = k->next;
    }
}

void values_key(const char *key) {
    KeyNode *k = find_key(key);
    if (!k) return;
    ValNode *v = k->values;
    while (v) {
        printf("%s\n", v->value);
        v = v->next;
    }
}

void keys_list(void) {
    KeyNode *k = keys;
    while (k) {
        printf("%s\n", k->key);
        k = k->next;
    }
}

void free_all(void) {
    KeyNode *k = keys;
    while (k) {
        KeyNode *tmp = k->next;
        ValNode *v = k->values;
        while (v) {
            ValNode *tv = v->next;
            free(v->value);
            free(v);
            v = tv;
        }
        free(k->key);
        free(k);
        k = tmp;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char key[256], val[4096];
        if (sscanf(line, "%31s", cmd) < 1) continue;
        const char *rest = line + strlen(cmd) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(rest, "%255s", key) == 1) {
                const char *v = rest + strlen(key) + 1;
                while (*v == ' ') v++;
                if (*v) add_key_value(key, v);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(rest, "%255s", key) == 1) {
                const char *v = rest + strlen(key) + 1;
                while (*v == ' ') v++;
                if (*v) {
                    KeyNode *k = find_key(key);
                    if (k) remove_value(k, v);
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (sscanf(rest, "%255s", key) == 1) erase_key(key);
        } else if (strcmp(cmd, "VALUES") == 0) {
            if (sscanf(rest, "%255s", key) == 1) values_key(key);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_list();
        }
    }
    free_all();
    return 0;
}