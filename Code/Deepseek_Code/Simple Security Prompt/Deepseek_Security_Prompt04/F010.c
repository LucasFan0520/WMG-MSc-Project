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

Key *head = NULL;

Key *find_key(char *key) {
    for (Key *k = head; k; k = k->next)
        if (strcmp(k->key, key) == 0) return k;
    return NULL;
}

void add_value(char *key, char *value) {
    Key *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(Key));
        k->key = strdup(key);
        k->values = NULL;
        k->next = head;
        head = k;
    }
    Value *v = malloc(sizeof(Value));
    v->val = strdup(value);
    v->next = k->values;
    k->values = v;
}

void remove_value(char *key, char *value) {
    Key *k = find_key(key);
    if (!k) return;
    Value *prev = NULL;
    for (Value *v = k->values; v; v = v->next) {
        if (strcmp(v->val, value) == 0) {
            if (prev) prev->next = v->next;
            else k->values = v->next;
            free(v->val);
            free(v);
            return;
        }
        prev = v;
    }
}

void erase_key(char *key) {
    Key *prev = NULL;
    for (Key *k = head; k; k = k->next) {
        if (strcmp(k->key, key) == 0) {
            if (prev) prev->next = k->next;
            else head = k->next;
            free(k->key);
            Value *v = k->values;
            while (v) {
                Value *next = v->next;
                free(v->val);
                free(v);
                v = next;
            }
            free(k);
            return;
        }
        prev = k;
    }
}

void values_for_key(char *key) {
    Key *k = find_key(key);
    if (!k) return;
    for (Value *v = k->values; v; v = v->next)
        printf("%s\n", v->val);
}

void keys_all(void) {
    for (Key *k = head; k; k = k->next)
        printf("%s\n", k->key);
}

void free_all(void) {
    Key *k = head;
    while (k) {
        Key *next = k->next;
        free(k->key);
        Value *v = k->values;
        while (v) {
            Value *vnext = v->next;
            free(v->val);
            free(v);
            v = vnext;
        }
        free(k);
        k = next;
    }
}

int main() {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[1024];
        int n = sscanf(line, "%15s %255s %[^\n]", cmd, arg1, arg2);
        if (n < 1) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_value(arg1, arg2);
        } else if (strcmp(cmd, "REMOVE") == 0 && n == 3) {
            remove_value(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0 && n == 2) {
            erase_key(arg1);
        } else if (strcmp(cmd, "VALUES") == 0 && n == 2) {
            values_for_key(arg1);
        } else if (strcmp(cmd, "KEYS") == 0 && n == 1) {
            keys_all();
        }
    }
    free(line);
    free_all();
    return 0;
}