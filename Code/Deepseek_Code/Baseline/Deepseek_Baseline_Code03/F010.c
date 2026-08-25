// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct value_node {
    char *value;
    struct value_node *next;
};

struct key_node {
    char *key;
    struct value_node *values;
    struct key_node *next;
};

struct key_node *keys = NULL;

struct key_node *find_key(char *key) {
    struct key_node *k = keys;
    while (k) {
        if (strcmp(k->key, key) == 0) return k;
        k = k->next;
    }
    return NULL;
}

void add_value(char *key, char *value) {
    struct key_node *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(struct key_node));
        k->key = malloc(strlen(key) + 1);
        strcpy(k->key, key);
        k->values = NULL;
        k->next = keys;
        keys = k;
    }
    struct value_node *v = malloc(sizeof(struct value_node));
    v->value = malloc(strlen(value) + 1);
    strcpy(v->value, value);
    v->next = k->values;
    k->values = v;
}

void remove_value(char *key, char *value) {
    struct key_node *k = find_key(key);
    if (!k) return;
    struct value_node *v = k->values, *prev = NULL;
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

void erase_key(char *key) {
    struct key_node *k = keys, *prev = NULL;
    while (k) {
        if (strcmp(k->key, key) == 0) {
            if (prev) prev->next = k->next;
            else keys = k->next;
            // free values
            struct value_node *v = k->values;
            while (v) {
                struct value_node *next = v->next;
                free(v->value);
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

void values_key(char *key) {
    struct key_node *k = find_key(key);
    if (k) {
        struct value_node *v = k->values;
        while (v) {
            printf("%s\n", v->value);
            v = v->next;
        }
    }
}

void keys_list() {
    struct key_node *k = keys;
    while (k) {
        printf("%s\n", k->key);
        k = k->next;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32];
        char key[256], value[4096];
        int n = sscanf(line, "%31s %255s %4095s", cmd, key, value);
        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            add_value(key, value);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 3) {
            remove_value(key, value);
        } else if (strcmp(cmd, "ERASE") == 0 && n >= 2) {
            erase_key(key);
        } else if (strcmp(cmd, "VALUES") == 0 && n >= 2) {
            values_key(key);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_list();
        }
    }
    return 0;
}