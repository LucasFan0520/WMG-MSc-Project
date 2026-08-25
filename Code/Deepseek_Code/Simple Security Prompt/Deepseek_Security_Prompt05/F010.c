/* F010.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Value {
    char *text;
    struct Value *next;
} Value;

typedef struct Key {
    char *name;
    Value *values;
    struct Key *next;
} Key;

Key *keys = NULL;

Key *find_key(const char *name) {
    Key *k = keys;
    while (k) {
        if (strcmp(k->name, name) == 0) return k;
        k = k->next;
    }
    return NULL;
}

void add_value(Key *k, const char *val) {
    Value *v = malloc(sizeof(Value));
    if (!v) return;
    v->text = strdup(val);
    v->next = k->values;
    k->values = v;
}

void add_key_value(const char *key, const char *val) {
    Key *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(Key));
        if (!k) return;
        k->name = strdup(key);
        k->values = NULL;
        k->next = keys;
        keys = k;
    }
    add_value(k, val);
}

void remove_value(Key *k, const char *val) {
    Value *v = k->values, *prev = NULL;
    while (v) {
        if (strcmp(v->text, val) == 0) {
            if (prev) prev->next = v->next;
            else k->values = v->next;
            free(v->text);
            free(v);
            return;
        }
        prev = v;
        v = v->next;
    }
}

void remove_key_value(const char *key, const char *val) {
    Key *k = find_key(key);
    if (!k) return;
    remove_value(k, val);
}

void erase_key(const char *key) {
    Key *k = keys, *prev = NULL;
    while (k) {
        if (strcmp(k->name, key) == 0) {
            if (prev) prev->next = k->next;
            else keys = k->next;
            while (k->values) {
                Value *tmp = k->values;
                k->values = k->values->next;
                free(tmp->text);
                free(tmp);
            }
            free(k->name);
            free(k);
            return;
        }
        prev = k;
        k = k->next;
    }
}

void values_key(const char *key) {
    Key *k = find_key(key);
    if (!k) return;
    Value *v = k->values;
    while (v) {
        printf("%s\n", v->text);
        v = v->next;
    }
}

void keys_all(void) {
    Key *k = keys;
    while (k) {
        printf("%s\n", k->name);
        k = k->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], arg1[256], arg2[2048];
        int n = sscanf(line, "%15s %255s %2047s", cmd, arg1, arg2);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_key_value(arg1, arg2);
        } else if (strcmp(cmd, "REMOVE") == 0 && n == 3) {
            remove_key_value(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0) {
            erase_key(arg1);
        } else if (strcmp(cmd, "VALUES") == 0) {
            values_key(arg1);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_all();
        }
    }
    while (keys) {
        Key *tmp = keys;
        keys = keys->next;
        while (tmp->values) {
            Value *v = tmp->values;
            tmp->values = tmp->values->next;
            free(v->text);
            free(v);
        }
        free(tmp->name);
        free(tmp);
    }
    return 0;
}