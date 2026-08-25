// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_line() {
    size_t cap = 256, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    while (1) {
        if (!fgets(buf + len, cap - len, stdin)) {
            if (len == 0) { free(buf); return NULL; }
            break;
        }
        len = strlen(buf);
        if (len > 0 && buf[len-1] == '\n') { buf[len-1] = '\0'; break; }
        if (len == cap - 1) {
            cap *= 2;
            char *tmp = realloc(buf, cap);
            if (!tmp) { free(buf); return NULL; }
            buf = tmp;
        }
    }
    return buf;
}

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
    for (Key *k = keys; k; k = k->next) {
        if (strcmp(k->key, key) == 0) return k;
    }
    return NULL;
}

void add_value(const char *key, const char *val) {
    Key *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(Key));
        k->key = strdup(key);
        k->values = NULL;
        k->next = keys;
        keys = k;
    }
    Value *v = malloc(sizeof(Value));
    v->val = strdup(val);
    v->next = k->values;
    k->values = v;
}

void remove_value(const char *key, const char *val) {
    Key *k = find_key(key);
    if (!k) return;
    Value *prev = NULL, *curr = k->values;
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
    Key *prev = NULL, *curr = keys;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else keys = curr->next;
            Value *v = curr->values;
            while (v) {
                Value *next = v->next;
                free(v->val);
                free(v);
                v = next;
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
    Key *k = find_key(key);
    if (k) {
        for (Value *v = k->values; v; v = v->next) {
            printf("%s\n", v->val);
        }
    }
}

void print_keys() {
    for (Key *k = keys; k; k = k->next) {
        printf("%s\n", k->key);
    }
}

int main() {
    char *line;
    while ((line = read_line())) {
        if (strcmp(line, "END") == 0) break;
        char cmd[16] = {0}, arg1[256] = {0}, arg2[256] = {0};
        sscanf(line, "%15s %255s %255s", cmd, arg1, arg2);
        
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
        free(line);
    }
    
    Key *k = keys;
    while (k) {
        Key *next = k->next;
        Value *v = k->values;
        while (v) {
            Value *next_v = v->next;
            free(v->val);
            free(v);
            v = next_v;
        }
        free(k->key);
        free(k);
        k = next;
    }
    return 0;
}