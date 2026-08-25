// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ValueNode {
    char *val;
    struct ValueNode *next;
} ValueNode;

typedef struct KeyNode {
    char *key;
    ValueNode *values;
    struct KeyNode *next;
} KeyNode;

KeyNode *keys = NULL;

KeyNode *find_key(const char *key) {
    KeyNode *curr = keys;
    while (curr) {
        if (strcmp(curr->key, key) == 0) return curr;
        curr = curr->next;
    }
    return NULL;
}

void add_value(const char *key, const char *val) {
    KeyNode *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(KeyNode));
        if (!k) exit(EXIT_FAILURE);
        k->key = strdup(key);
        k->values = NULL;
        k->next = keys;
        keys = k;
    }
    ValueNode *v = malloc(sizeof(ValueNode));
    if (!v) exit(EXIT_FAILURE);
    v->val = strdup(val);
    v->next = NULL;
    if (!k->values) {
        k->values = v;
    } else {
        ValueNode *curr = k->values;
        while (curr->next) curr = curr->next;
        curr->next = v;
    }
}

void remove_value(const char *key, const char *val) {
    KeyNode *k = find_key(key);
    if (!k) return;
    ValueNode *curr = k->values;
    ValueNode *prev = NULL;
    while (curr) {
        if (strcmp(curr->val, val) == 0) {
            if (prev) prev->next = curr->next;
            else k->values = curr->next;
            free(curr->val);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void erase_key(const char *key) {
    KeyNode *curr = keys;
    KeyNode *prev = NULL;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else keys = curr->next;
            ValueNode *v = curr->values;
            while (v) {
                ValueNode *next_v = v->next;
                free(v->val);
                free(v);
                v = next_v;
            }
            free(curr->key);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_values(const char *key) {
    KeyNode *k = find_key(key);
    if (!k) return;
    ValueNode *v = k->values;
    while (v) {
        printf("%s\n", v->val);
        v = v->next;
    }
}

void print_keys() {
    KeyNode *curr = keys;
    while (curr) {
        printf("%s\n", curr->key);
        curr = curr->next;
    }
}

void free_all() {
    KeyNode *curr = keys;
    while (curr) {
        KeyNode *next_k = curr->next;
        ValueNode *v = curr->values;
        while (v) {
            ValueNode *next_v = v->next;
            free(v->val);
            free(v);
            v = next_v;
        }
        free(curr->key);
        free(curr);
        curr = next_k;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32] = {0};
        char arg1[256] = {0};
        char arg2[256] = {0};
        
        sscanf(line, "%31s %255s %255s", cmd, arg1, arg2);
        
        if (strcmp(cmd, "ADD") == 0) {
            add_value(arg1, arg2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_value(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0) {
            erase_key(arg1);
        } else if (strcmp(cmd, "VALUES") == 0) {
            print_values(arg1);
        } else if (strcmp(cmd, "KEYS") == 0) {
            print_keys();
        }
    }
    free_all();
    return 0;
}