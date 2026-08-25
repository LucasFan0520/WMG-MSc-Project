/* F010.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Value {
    char *val;
    struct Value *next;
} Value;

typedef struct Key {
    char *key;
    Value *values;
    struct Key *next;
} Key;

Key *keys = NULL;

Key *find_key(const char *key) {
    Key *k = keys;
    while (k) {
        if (strcmp(k->key, key) == 0) return k;
        k = k->next;
    }
    return NULL;
}

void add_value(Key *k, const char *val) {
    Value *v = malloc(sizeof(Value));
    v->val = strdup(val);
    v->next = k->values;
    k->values = v;
}

void add_key_value(const char *key, const char *val) {
    Key *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(Key));
        k->key = strdup(key);
        k->values = NULL;
        k->next = keys;
        keys = k;
    }
    add_value(k, val);
}

void remove_value(Key *k, const char *val) {
    Value *prev = NULL, *cur = k->values;
    while (cur) {
        if (strcmp(cur->val, val) == 0) {
            if (prev) prev->next = cur->next;
            else k->values = cur->next;
            free(cur->val);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void erase_key(const char *key) {
    Key *prev = NULL, *cur = keys;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (prev) prev->next = cur->next;
            else keys = cur->next;
            Value *v = cur->values;
            while (v) {
                Value *tmp = v;
                v = v->next;
                free(tmp->val);
                free(tmp);
            }
            free(cur->key);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void print_values(Key *k) {
    Value *v = k->values;
    while (v) {
        printf("%s\n", v->val);
        v = v->next;
    }
}

void print_keys(void) {
    Key *k = keys;
    while (k) {
        printf("%s\n", k->key);
        k = k->next;
    }
}

void free_keys(void) {
    while (keys) {
        Key *tmp = keys;
        keys = keys->next;
        Value *v = tmp->values;
        while (v) {
            Value *tv = v;
            v = v->next;
            free(tv->val);
            free(tv);
        }
        free(tmp->key);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        char key[256], val[256];
        int n = sscanf(line, "%s %s %s", cmd, key, val);
        if (strcmp(cmd, "ADD") == 0) {
            if (n < 3) continue;
            add_key_value(key, val);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (n < 3) continue;
            Key *k = find_key(key);
            if (k) remove_value(k, val);
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (n < 2) continue;
            erase_key(key);
        } else if (strcmp(cmd, "VALUES") == 0) {
            if (n < 2) continue;
            Key *k = find_key(key);
            if (k) print_values(k);
        } else if (strcmp(cmd, "KEYS") == 0) {
            print_keys();
        }
    }
    free_keys();
    return 0;
}