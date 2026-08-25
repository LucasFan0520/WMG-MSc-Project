// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Value {
    char *val;
    struct Value *next;
} Value;

typedef struct KeyList {
    char *key;
    Value *values;
    struct KeyList *next;
} KeyList;

KeyList *head = NULL;

KeyList *find_key(const char *key) {
    KeyList *k = head;
    while (k) {
        if (strcmp(k->key, key) == 0) return k;
        k = k->next;
    }
    return NULL;
}

void add_value(const char *key, const char *value) {
    KeyList *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(KeyList));
        if (!k) return;
        k->key = strdup(key);
        k->values = NULL;
        k->next = NULL;
        if (!k->key) { free(k); return; }
        if (!head) head = k;
        else {
            KeyList *last = head;
            while (last->next) last = last->next;
            last->next = k;
        }
    }
    Value *v = malloc(sizeof(Value));
    if (!v) return;
    v->val = strdup(value);
    v->next = NULL;
    if (!v->val) { free(v); return; }
    if (!k->values) k->values = v;
    else {
        Value *last = k->values;
        while (last->next) last = last->next;
        last->next = v;
    }
}

void remove_value(const char *key, const char *value) {
    KeyList *k = find_key(key);
    if (!k) return;
    Value *v = k->values, *prev = NULL;
    while (v) {
        if (strcmp(v->val, value) == 0) {
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
    KeyList *k = head, *prev = NULL;
    while (k) {
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
        k = k->next;
    }
}

void values_key(const char *key) {
    KeyList *k = find_key(key);
    if (!k) return;
    Value *v = k->values;
    while (v) {
        printf("%s\n", v->val);
        v = v->next;
    }
}

void keys_list(void) {
    KeyList *k = head;
    while (k) {
        printf("%s\n", k->key);
        k = k->next;
    }
}

void free_all(void) {
    KeyList *k = head;
    while (k) {
        KeyList *next = k->next;
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

int main(void) {
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[64], key[256], value[4096];
        if (sscanf(line, "%63s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, value) == 2)
                add_value(key, value);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, value) == 2)
                remove_value(key, value);
        } else if (strcmp(cmd, "ERASE") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                erase_key(key);
        } else if (strcmp(cmd, "VALUES") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                values_key(key);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_list();
        }
    }
    free(line);
    free_all();
    return 0;
}