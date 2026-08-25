// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Value {
    char *val;
    struct Value *next;
} Value;

typedef struct Key {
    char *name;
    Value *vals;
    struct Key *next;
} Key;

Key *keys = NULL;

Key *find_key(const char *name) {
    Key *curr = keys;
    while (curr) {
        if (strcmp(curr->name, name) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_value(const char *key_name, const char *val) {
    Key *k = find_key(key_name);
    if (!k) {
        k = malloc(sizeof(Key));
        k->name = strdup(key_name);
        k->vals = NULL;
        k->next = keys;
        keys = k;
    }
    Value *v = malloc(sizeof(Value));
    v->val = strdup(val);
    v->next = k->vals;
    k->vals = v;
}

void remove_value(const char *key_name, const char *val) {
    Key *k = find_key(key_name);
    if (!k) return;
    Value *curr = k->vals, *prev = NULL;
    while (curr) {
        if (strcmp(curr->val, val) == 0) {
            if (prev) prev->next = curr->next;
            else k->vals = curr->next;
            free(curr->val);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void erase_key(const char *key_name) {
    Key *curr = keys, *prev = NULL;
    while (curr) {
        if (strcmp(curr->name, key_name) == 0) {
            if (prev) prev->next = curr->next;
            else keys = curr->next;
            Value *v = curr->vals;
            while (v) {
                Value *next = v->next;
                free(v->val);
                free(v);
                v = next;
            }
            free(curr->name);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_values(const char *key_name) {
    Key *k = find_key(key_name);
    if (!k) return;
    Value *v = k->vals;
    while (v) {
        printf("%s\n", v->val);
        v = v->next;
    }
}

void print_keys() {
    Key *curr = keys;
    while (curr) {
        printf("%s\n", curr->name);
        curr = curr->next;
    }
}

void free_all() {
    Key *curr = keys;
    while (curr) {
        Key *next = curr->next;
        Value *v = curr->vals;
        while (v) {
            Value *next_v = v->next;
            free(v->val);
            free(v);
            v = next_v;
        }
        free(curr->name);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32], key[256], val[4096] = {0};
        int n = sscanf(line, "%31s %255s %4095[^\n]", cmd, key, val);
        
        if (strcmp(cmd, "ADD") == 0 && n >= 2) add_value(key, val);
        else if (strcmp(cmd, "REMOVE") == 0 && n >= 2) remove_value(key, val);
        else if (strcmp(cmd, "ERASE") == 0 && n >= 1) erase_key(key);
        else if (strcmp(cmd, "VALUES") == 0 && n >= 1) print_values(key);
        else if (strcmp(cmd, "KEYS") == 0) print_keys();
    }
    free_all();
    return 0;
}