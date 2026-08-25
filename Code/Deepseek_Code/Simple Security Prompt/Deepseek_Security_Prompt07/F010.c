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

Key *keys = NULL;

Key *find_key(const char *key) {
    Key *k = keys;
    while (k) {
        if (strcmp(k->key, key) == 0) return k;
        k = k->next;
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
    v->next = NULL;
    if (!k->values) {
        k->values = v;
    } else {
        Value *last = k->values;
        while (last->next) last = last->next;
        last->next = v;
    }
}

void remove_value(const char *key, const char *val) {
    Key *k = find_key(key);
    if (!k) return;
    Value *prev = NULL;
    Value *v = k->values;
    while (v) {
        if (strcmp(v->val, val) == 0) {
            if (prev) prev->next = v->next;
            else k->values = v->next;
            free(v->val);
            free(v);
            return;
        }
        prev = v;
        v = v->next;
    }
}

void erase_key(const char *key) {
    Key *prev = NULL;
    Key *k = keys;
    while (k) {
        if (strcmp(k->key, key) == 0) {
            if (prev) prev->next = k->next;
            else keys = k->next;
            Value *v = k->values;
            while (v) {
                Value *next = v->next;
                free(v->val);
                free(v);
                v = next;
            }
            free(k->key);
            free(k);
            return;
        }
        prev = k;
        k = k->next;
    }
}

void values_for_key(const char *key) {
    Key *k = find_key(key);
    if (!k || !k->values) return;
    Value *v = k->values;
    int first = 1;
    while (v) {
        if (!first) putchar(' ');
        for (char *s = v->val; *s; s++) putchar(*s == ' ' ? '_' : *s);
        first = 0;
        v = v->next;
    }
    putchar('\n');
}

void print_keys() {
    Key *k = keys;
    int first = 1;
    while (k) {
        if (!first) putchar(' ');
        printf("%s", k->key);
        first = 0;
        k = k->next;
    }
    putchar('\n');
}

void free_all() {
    Key *k = keys;
    while (k) {
        Key *nextk = k->next;
        Value *v = k->values;
        while (v) {
            Value *nextv = v->next;
            free(v->val);
            free(v);
            v = nextv;
        }
        free(k->key);
        free(k);
        k = nextk;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char rest[4096];
        char *p = line + strlen(cmd);
        while (*p == ' ') p++;
        strcpy(rest, p);
        if (strcmp(cmd, "ADD") == 0) {
            char key[256], val[4096];
            if (sscanf(rest, "%255s %[^\n]", key, val) == 2) {
                add_value(key, val);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char key[256], val[4096];
            if (sscanf(rest, "%255s %[^\n]", key, val) == 2) {
                remove_value(key, val);
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) == 1) {
                erase_key(key);
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) == 1) {
                values_for_key(key);
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            print_keys();
        }
    }
    free_all();
    return 0;
}