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
    Value *values;
    struct Key *next;
} Key;

Key *head = NULL;

Key *find_key(const char *key) {
    for (Key *curr = head; curr; curr = curr->next) {
        if (strcmp(curr->key, key) == 0) return curr;
    }
    return NULL;
}

void add_value(const char *key, const char *val) {
    Key *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(Key));
        if (!k) return;
        k->key = strdup(key);
        k->values = NULL;
        k->next = head;
        head = k;
    }
    Value *v = malloc(sizeof(Value));
    if (!v) return;
    v->val = strdup(val);
    v->next = NULL;
    if (!k->values) {
        k->values = v;
    } else {
        Value *curr = k->values;
        while (curr->next) curr = curr->next;
        curr->next = v;
    }
}

void remove_value(const char *key, const char *val) {
    Key *k = find_key(key);
    if (!k) return;
    Value *curr = k->values;
    Value *prev = NULL;
    while (curr) {
        if (strcmp(curr->val, val) == 0) {
            Value *next = curr->next;
            free(curr->val);
            free(curr);
            if (prev) prev->next = next;
            else k->values = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void erase_key(const char *key) {
    Key *curr = head;
    Key *prev = NULL;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            Key *next = curr->next;
            Value *v = curr->values;
            while (v) {
                Value *vn = v->next;
                free(v->val);
                free(v);
                v = vn;
            }
            free(curr->key);
            free(curr);
            if (prev) prev->next = next;
            else head = next;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void values_key(const char *key) {
    Key *k = find_key(key);
    if (!k) return;
    for (Value *v = k->values; v; v = v->next) {
        printf("%s\n", v->val);
    }
}

void keys() {
    for (Key *curr = head; curr; curr = curr->next) {
        printf("%s\n", curr->key);
    }
}

void free_all() {
    Key *curr = head;
    while (curr) {
        Key *next = curr->next;
        Value *v = curr->values;
        while (v) {
            Value *vn = v->next;
            free(v->val);
            free(v);
            v = vn;
        }
        free(curr->key);
        free(curr);
        curr = next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[16];
        char arg1[1024], arg2[1024];
        
        int n = sscanf(line, "%15s %1023s %1023s", cmd, arg1, arg2);
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_value(arg1, arg2);
        } else if (strcmp(cmd, "REMOVE") == 0 && n == 3) {
            remove_value(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0 && n >= 2) {
            erase_key(arg1);
        } else if (strcmp(cmd, "VALUES") == 0 && n >= 2) {
            values_key(arg1);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys();
        }
    }
    free_all();
    return 0;
}