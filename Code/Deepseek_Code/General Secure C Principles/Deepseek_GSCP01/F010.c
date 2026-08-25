/* F010.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Value {
    char *value;
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

void add_value(const char *key, const char *value) {
    Key *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(Key));
        k->key = strdup(key);
        k->values = NULL;
        k->next = keys;
        keys = k;
    }
    Value *v = malloc(sizeof(Value));
    v->value = strdup(value);
    v->next = k->values;
    k->values = v;
}

void remove_value(const char *key, const char *value) {
    Key *k = find_key(key);
    if (!k) return;
    Value *v = k->values, *prev = NULL;
    while (v && strcmp(v->value, value) != 0) {
        prev = v;
        v = v->next;
    }
    if (!v) return;
    if (prev) prev->next = v->next;
    else k->values = v->next;
    free(v->value);
    free(v);
}

void erase_key(const char *key) {
    Key *k = keys, *prev = NULL;
    while (k && strcmp(k->key, key) != 0) {
        prev = k;
        k = k->next;
    }
    if (!k) return;
    if (prev) prev->next = k->next;
    else keys = k->next;
    free(k->key);
    Value *v = k->values;
    while (v) {
        Value *next = v->next;
        free(v->value);
        free(v);
        v = next;
    }
    free(k);
}

void values_key(const char *key) {
    Key *k = find_key(key);
    if (!k) return;
    Value *v = k->values;
    int first = 1;
    while (v) {
        if (!first) putchar(' ');
        printf("%s", v->value);
        first = 0;
        v = v->next;
    }
    putchar('\n');
}

void keys_list(void) {
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

void free_all(void) {
    Key *k = keys;
    while (k) {
        Key *nextk = k->next;
        free(k->key);
        Value *v = k->values;
        while (v) {
            Value *nextv = v->next;
            free(v->value);
            free(v);
            v = nextv;
        }
        free(k);
        k = nextk;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char key[256], value[256];
            if (sscanf(line + 4, "%255s %255s", key, value) == 2)
                add_value(key, value);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char key[256], value[256];
            if (sscanf(line + 7, "%255s %255s", key, value) == 2)
                remove_value(key, value);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            erase_key(p);
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            values_key(p);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_list();
        }
    }
    free_all();
    free(line);
    return 0;
}