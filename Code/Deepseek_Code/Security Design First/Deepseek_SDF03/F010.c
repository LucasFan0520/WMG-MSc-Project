// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096
#define MAX_KEY 64
#define MAX_VAL 4096

typedef struct Value {
    char val[MAX_VAL];
    struct Value *next;
} Value;

typedef struct Key {
    char key[MAX_KEY];
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

void free_values(Value *v) {
    while (v) {
        Value *tmp = v;
        v = v->next;
        free(tmp);
    }
}

void free_all() {
    Key *k = keys;
    while (k) {
        Key *tmp = k;
        free_values(k->values);
        k = k->next;
        free(tmp);
    }
    keys = NULL;
}

void handle_add(char *line) {
    char key[MAX_KEY], val[MAX_VAL];
    int n = sscanf(line, "ADD %63s %4095[^\n]", key, val);
    if (n < 2) return;
    Key *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(Key));
        if (!k) return;
        strcpy(k->key, key);
        k->values = NULL;
        k->next = keys;
        keys = k;
    }
    Value *v = malloc(sizeof(Value));
    if (!v) return;
    strcpy(v->val, val);
    v->next = k->values;
    k->values = v;
}

void handle_remove(char *line) {
    char key[MAX_KEY], val[MAX_VAL];
    int n = sscanf(line, "REMOVE %63s %4095[^\n]", key, val);
    if (n < 2) return;
    Key *k = find_key(key);
    if (!k) return;
    Value *v = k->values, *prev = NULL;
    while (v) {
        if (strcmp(v->val, val) == 0) {
            if (prev) prev->next = v->next;
            else k->values = v->next;
            free(v);
            return;
        }
        prev = v;
        v = v->next;
    }
}

void handle_erase(char *line) {
    char key[MAX_KEY];
    if (sscanf(line, "ERASE %63s", key) != 1) return;
    Key *k = keys, *prev = NULL;
    while (k) {
        if (strcmp(k->key, key) == 0) {
            if (prev) prev->next = k->next;
            else keys = k->next;
            free_values(k->values);
            free(k);
            return;
        }
        prev = k;
        k = k->next;
    }
}

void handle_values(char *line) {
    char key[MAX_KEY];
    if (sscanf(line, "VALUES %63s", key) != 1) return;
    Key *k = find_key(key);
    if (!k) return;
    Value *v = k->values;
    int first = 1;
    while (v) {
        if (!first) putchar(' ');
        printf("%s", v->val);
        first = 0;
        v = v->next;
    }
    putchar('\n');
}

void handle_keys() {
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

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        if (strncmp(line, "ADD ", 4) == 0) handle_add(line);
        else if (strncmp(line, "REMOVE ", 7) == 0) handle_remove(line);
        else if (strncmp(line, "ERASE ", 6) == 0) handle_erase(line);
        else if (strncmp(line, "VALUES ", 7) == 0) handle_values(line);
        else if (strcmp(line, "KEYS") == 0) handle_keys();
    }
    free_all();
    return 0;
}