// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Value {
    char *val;
    struct Value *next;
} Value;

typedef struct Key {
    char *key;
    Value *vals;
    struct Key *next;
} Key;

Key *head = NULL;

Key *find_key(char *key) {
    for (Key *k = head; k; k = k->next) {
        if (strcmp(k->key, key) == 0) return k;
    }
    return NULL;
}

void add_value(char *key, char *val) {
    Key *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(Key));
        k->key = strdup(key);
        k->vals = NULL;
        k->next = head;
        head = k;
    }
    Value *v = malloc(sizeof(Value));
    v->val = strdup(val);
    v->next = NULL;
    if (!k->vals) {
        k->vals = v;
    } else {
        Value *last = k->vals;
        while (last->next) last = last->next;
        last->next = v;
    }
}

void remove_value(char *key, char *val) {
    Key *k = find_key(key);
    if (!k) return;
    Value *prev = NULL;
    Value *curr = k->vals;
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

void erase_key(char *key) {
    Key *prev = NULL;
    Key *curr = head;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            free(curr->key);
            Value *v = curr->vals;
            while (v) {
                Value *next = v->next;
                free(v->val);
                free(v);
                v = next;
            }
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void print_values(char *key) {
    Key *k = find_key(key);
    if (k) {
        for (Value *v = k->vals; v; v = v->next) {
            puts(v->val);
        }
    }
}

void print_keys() {
    for (Key *k = head; k; k = k->next) {
        puts(k->key);
    }
}

int main() {
    char cmd[20], a[100], b[100];
    while (scanf("%19s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%99s%99s", a, b);
            add_value(a, b);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%99s%99s", a, b);
            remove_value(a, b);
        } else if (strcmp(cmd, "ERASE") == 0) {
            scanf("%99s", a);
            erase_key(a);
        } else if (strcmp(cmd, "VALUES") == 0) {
            scanf("%99s", a);
            print_values(a);
        } else if (strcmp(cmd, "KEYS") == 0) {
            print_keys();
        }
    }
    return 0;
}