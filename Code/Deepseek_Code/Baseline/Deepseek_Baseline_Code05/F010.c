/* F010.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct val {
    char *value;
    struct val *next;
} Val;

typedef struct key {
    char *key;
    Val *values;
    struct key *next;
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
    Val *v = k->values, *prev = NULL;
    while (v) {
        if (strcmp(v->value, value) == 0) {
            if (prev) prev->next = v->next;
            else k->values = v->next;
            free(v->value);
            free(v);
            return;
        }
        prev = v;
        v = v->next;
    }
}

void erase_key(const char *key) {
    Key *k = keys, *prev = NULL;
    while (k) {
        if (strcmp(k->key, key) == 0) {
            if (prev) prev->next = k->next;
            else keys = k->next;
            Val *v = k->values;
            while (v) { Val *tmp = v; v = v->next; free(tmp->value); free(tmp); }
            free(k->key);
            free(k);
            return;
        }
        prev = k;
        k = k->next;
    }
}

void values(const char *key) {
    Key *k = find_key(key);
    if (!k) return;
    Val *v = k->values;
    while (v) {
        for (char *c = v->value; *c; c++) putchar(*c == ' ' ? '_' : *c);
        putchar('\n');
        v = v->next;
    }
}

void keys_print() {
    Key *k = keys;
    while (k) {
        printf("%s\n", k->key);
        k = k->next;
    }
}

int main() {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], key[256], val[256];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "KEYS") == 0) { keys_print(); continue; }
        if (strcmp(cmd, "ADD") == 0 || strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s %255s", key, val) == 2) {
                if (strcmp(cmd, "ADD") == 0) add_value(key, val);
                else remove_value(key, val);
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1) erase_key(key);
        } else if (strcmp(cmd, "VALUES") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1) values(key);
        }
    }
    return 0;
}