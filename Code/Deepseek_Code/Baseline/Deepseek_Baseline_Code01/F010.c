// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct val {
    char *text;
    struct val *next;
} Val;

typedef struct keynode {
    char *key;
    Val *values;
    struct keynode *next;
} KeyNode;

KeyNode *keys = NULL;

KeyNode *find_key(const char *key) {
    KeyNode *k = keys;
    while (k) {
        if (strcmp(k->key, key) == 0) return k;
        k = k->next;
    }
    return NULL;
}

void add_value(KeyNode *k, const char *val) {
    Val *v = malloc(sizeof(Val));
    v->text = strdup(val);
    v->next = k->values;
    k->values = v;
}

void add_key_val(const char *key, const char *val) {
    KeyNode *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(KeyNode));
        k->key = strdup(key);
        k->values = NULL;
        k->next = keys;
        keys = k;
    }
    add_value(k, val);
}

void remove_key_val(const char *key, const char *val) {
    KeyNode *k = find_key(key);
    if (!k) return;
    Val *v = k->values, *prev = NULL;
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

void erase_key(const char *key) {
    KeyNode *k = keys, *prev = NULL;
    while (k) {
        if (strcmp(k->key, key) == 0) {
            if (prev) prev->next = k->next;
            else keys = k->next;
            Val *v = k->values;
            while (v) { Val *tmp = v; v = v->next; free(tmp->text); free(tmp); }
            free(k->key);
            free(k);
            return;
        }
        prev = k;
        k = k->next;
    }
}

void values_key(const char *key) {
    KeyNode *k = find_key(key);
    if (!k) return;
    Val *v = k->values;
    while (v) {
        printf("%s\n", v->text);
        v = v->next;
    }
}

void keys_print(void) {
    KeyNode *k = keys;
    while (k) {
        printf("%s\n", k->key);
        k = k->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char arg1[256], arg2[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", arg1, arg2) == 2)
                add_key_val(arg1, arg2);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", arg1, arg2) == 2)
                remove_key_val(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                erase_key(arg1);
        } else if (strcmp(cmd, "VALUES") == 0) {
            if (sscanf(line, "%*s %255s", arg1) == 1)
                values_key(arg1);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_print();
        }
    }
    return 0;
}