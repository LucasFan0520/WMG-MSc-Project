// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Val {
    char *value;
    struct Val *next;
} Val;

typedef struct Key {
    char *key;
    Val *values;
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

void add_value(const char *key, const char *value) {
    Key *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(Key));
        k->key = strdup(key);
        k->values = NULL;
        k->next = keys;
        keys = k;
    }
    Val *v = malloc(sizeof(Val));
    v->value = strdup(value);
    v->next = k->values;
    k->values = v;
}

void remove_value(const char *key, const char *value) {
    Key *k = find_key(key);
    if (!k) return;
    Val *cur = k->values, *prev = NULL;
    while (cur) {
        if (strcmp(cur->value, value) == 0) {
            if (prev) prev->next = cur->next;
            else k->values = cur->next;
            free(cur->value);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void erase_key(const char *key) {
    Key *k = find_key(key);
    if (!k) return;
    Key *cur = keys, *prev = NULL;
    while (cur) {
        if (cur == k) {
            if (prev) prev->next = cur->next;
            else keys = cur->next;
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    Val *v = k->values;
    while (v) {
        Val *tmp = v;
        v = v->next;
        free(tmp->value);
        free(tmp);
    }
    free(k->key);
    free(k);
}

void values_key(const char *key) {
    Key *k = find_key(key);
    if (!k) return;
    Val *v = k->values;
    while (v) {
        printf("%s\n", v->value);
        v = v->next;
    }
}

void keys_list(void) {
    Key *k = keys;
    while (k) {
        printf("%s\n", k->key);
        k = k->next;
    }
}

void cleanup_keys(void) {
    Key *k = keys;
    while (k) {
        Key *tmp = k;
        k = k->next;
        Val *v = tmp->values;
        while (v) {
            Val *tv = v;
            v = v->next;
            free(tv->value);
            free(tv);
        }
        free(tmp->key);
        free(tmp);
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[1024];
        int n = sscanf(line, "%31s %255s %1023s", cmd, arg1, arg2);
        if (n < 2) continue;
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_value(arg1, arg2);
        } else if (strcmp(cmd, "REMOVE") == 0 && n == 3) {
            remove_value(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0) {
            erase_key(arg1);
        } else if (strcmp(cmd, "VALUES") == 0) {
            values_key(arg1);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_list();
        }
    }
    cleanup_keys();
    return 0;
}