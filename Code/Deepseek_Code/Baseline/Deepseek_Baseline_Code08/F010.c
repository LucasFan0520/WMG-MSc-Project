// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ValueNode {
    char *value;
    struct ValueNode *next;
} ValueNode;

typedef struct KeyNode {
    char *key;
    ValueNode *values;
    struct KeyNode *next;
} KeyNode;

KeyNode *head = NULL;

KeyNode* find_key(const char *key) {
    KeyNode *cur = head;
    while (cur) {
        if (strcmp(cur->key, key) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void add_value(const char *key, const char *value) {
    KeyNode *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(KeyNode));
        k->key = strdup(key);
        k->values = NULL;
        k->next = NULL;
        if (!head) head = k;
        else {
            KeyNode *cur = head;
            while (cur->next) cur = cur->next;
            cur->next = k;
        }
    }
    ValueNode *v = malloc(sizeof(ValueNode));
    v->value = strdup(value);
    v->next = NULL;
    if (!k->values) k->values = v;
    else {
        ValueNode *cur = k->values;
        while (cur->next) cur = cur->next;
        cur->next = v;
    }
}

void remove_value(const char *key, const char *value) {
    KeyNode *k = find_key(key);
    if (!k) return;
    ValueNode *prev = NULL, *cur = k->values;
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
    KeyNode *prev = NULL, *cur = head;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            free(cur->key);
            ValueNode *v = cur->values;
            while (v) {
                ValueNode *tmp = v;
                v = v->next;
                free(tmp->value);
                free(tmp);
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void print_values(const char *key) {
    KeyNode *k = find_key(key);
    if (!k) return;
    ValueNode *v = k->values;
    while (v) {
        printf("%s\n", v->value);
        v = v->next;
    }
}

void print_keys() {
    KeyNode *cur = head;
    while (cur) {
        printf("%s\n", cur->key);
        cur = cur->next;
    }
}

void free_all() {
    KeyNode *cur = head;
    while (cur) {
        KeyNode *tmp = cur;
        cur = cur->next;
        free(tmp->key);
        ValueNode *v = tmp->values;
        while (v) {
            ValueNode *tv = v;
            v = v->next;
            free(tv->value);
            free(tv);
        }
        free(tmp);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *key = strtok(NULL, " ");
            if (!key) continue;
            char *val = strtok(NULL, "");
            if (!val) val = "";
            add_value(key, val);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *key = strtok(NULL, " ");
            if (!key) continue;
            char *val = strtok(NULL, "");
            if (!val) continue;
            remove_value(key, val);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *key = strtok(NULL, " ");
            if (key) erase_key(key);
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key = strtok(NULL, " ");
            if (key) print_values(key);
        } else if (strcmp(cmd, "KEYS") == 0) {
            print_keys();
        }
    }
    free_all();
    return 0;
}