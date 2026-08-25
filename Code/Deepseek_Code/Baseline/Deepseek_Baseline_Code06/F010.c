/* F010.c */
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

void add_value(const char *key, const char *value) {
    KeyNode *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(KeyNode));
        k->key = strdup(key);
        k->values = NULL;
        k->next = head;
        head = k;
    }
    ValNode *v = malloc(sizeof(ValNode));
    v->value = strdup(value);
    v->next = k->values;
    k->values = v;
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
            ValNode *v = k->values;
            while (v) {
                ValNode *tmp = v;
                v = v->next;
                free(tmp->value);
                free(tmp);
            }
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
    ValNode *v = k->values;
    while (v) {
        printf("%s\n", v->value);
        v = v->next;
    }
}

void keys_print() {
    KeyNode *k = head;
    while (k) {
        printf("%s\n", k->key);
        k = k->next;
    }
}

void free_all() {
    while (head) {
        KeyNode *tmp = head;
        head = head->next;
        ValNode *v = tmp->values;
        while (v) {
            ValNode *vtmp = v;
            v = v->next;
            free(vtmp->value);
            free(vtmp);
        }
        free(tmp->key);
        free(tmp);
    }
}

int main() {
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char key[256]; char value[4096];
        int n = sscanf(line, "%31s %255s %[^\n]", cmd, key, value);
        if (strcmp(cmd, "ADD") == 0 && n == 3) {
            add_value(key, value);
        } else if (strcmp(cmd, "REMOVE") == 0 && n == 3) {
            remove_value(key, value);
        } else if (strcmp(cmd, "ERASE") == 0 && n == 2) {
            erase_key(key);
        } else if (strcmp(cmd, "VALUES") == 0 && n == 2) {
            values_key(key);
        } else if (strcmp(cmd, "KEYS") == 0 && n == 1) {
            keys_print();
        }
    }
    free_all();
    return 0;
}