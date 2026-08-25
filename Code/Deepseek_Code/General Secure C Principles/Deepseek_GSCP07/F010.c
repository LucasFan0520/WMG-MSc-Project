// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct ValNode {
    char *value;
    struct ValNode *next;
} ValNode;

typedef struct KeyNode {
    char *key;
    ValNode *values;
    struct KeyNode *next;
} KeyNode;

KeyNode *head = NULL;

KeyNode* find_key(const char *key) {
    KeyNode *k = head;
    while (k) {
        if (strcmp(k->key, key) == 0) return k;
        k = k->next;
    }
    return NULL;
}

void free_vals(ValNode *v) {
    while (v) {
        ValNode *tmp = v;
        v = v->next;
        free(tmp->value);
        free(tmp);
    }
}

void free_key(KeyNode *k) {
    free(k->key);
    free_vals(k->values);
    free(k);
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
            KeyNode *last = head;
            while (last->next) last = last->next;
            last->next = k;
        }
    }
    ValNode *v = malloc(sizeof(ValNode));
    v->value = strdup(value);
    v->next = NULL;
    if (!k->values) k->values = v;
    else {
        ValNode *last = k->values;
        while (last->next) last = last->next;
        last->next = v;
    }
}

void remove_value(const char *key, const char *value) {
    KeyNode *k = find_key(key);
    if (!k) return;
    ValNode *v = k->values, *prev = NULL;
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
    KeyNode *k = head, *prev = NULL;
    while (k) {
        if (strcmp(k->key, key) == 0) {
            if (prev) prev->next = k->next;
            else head = k->next;
            free_key(k);
            return;
        }
        prev = k;
        k = k->next;
    }
}

void values_key(const char *key) {
    KeyNode *k = find_key(key);
    if (!k) return;
    ValNode *v = k->values;
    while (v) {
        printf("%s\n", v->value);
        v = v->next;
    }
}

void keys() {
    KeyNode *k = head;
    while (k) {
        printf("%s\n", k->key);
        k = k->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "END") == 0) break;
        if (strcmp(cmd, "ADD") == 0) {
            char *key = strtok(NULL, " ");
            char *val = strtok(NULL, "");
            if (key && val) add_value(key, val);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *key = strtok(NULL, " ");
            char *val = strtok(NULL, "");
            if (key && val) remove_value(key, val);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *key = strtok(NULL, " ");
            if (key) erase_key(key);
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key = strtok(NULL, " ");
            if (key) values_key(key);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys();
        }
    }
    while (head) {
        KeyNode *tmp = head;
        head = head->next;
        free_key(tmp);
    }
    return 0;
}