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
KeyNode *tail = NULL;

KeyNode *find_key(const char *key) {
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
        k->key = malloc(strlen(key)+1);
        strcpy(k->key, key);
        k->values = NULL;
        k->next = NULL;
        if (tail) tail->next = k;
        else head = k;
        tail = k;
    }
    ValNode *v = malloc(sizeof(ValNode));
    v->value = malloc(strlen(value)+1);
    strcpy(v->value, value);
    v->next = k->values;
    k->values = v;
}

void remove_value(const char *key, const char *value) {
    KeyNode *k = find_key(key);
    if (!k) return;
    ValNode *prev = NULL;
    ValNode *v = k->values;
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
    KeyNode *prev = NULL;
    KeyNode *k = head;
    while (k) {
        if (strcmp(k->key, key) == 0) {
            if (prev) prev->next = k->next;
            else head = k->next;
            if (k == tail) tail = prev;
            free(k->key);
            ValNode *v = k->values;
            while (v) {
                ValNode *next = v->next;
                free(v->value);
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
    KeyNode *k = find_key(key);
    if (!k) return;
    ValNode *v = k->values;
    while (v) {
        printf("%s\n", v->value);
        v = v->next;
    }
}

void keys_all(void) {
    KeyNode *k = head;
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
    KeyNode *k = head;
    while (k) {
        KeyNode *next = k->next;
        free(k->key);
        ValNode *v = k->values;
        while (v) {
            ValNode *vnext = v->next;
            free(v->value);
            free(v);
            v = vnext;
        }
        free(k);
        k = next;
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "ADD") == 0) {
            char *key = strtok(rest, " ");
            if (!key) continue;
            char *value = strtok(NULL, "");
            if (!value) value = "";
            add_value(key, value);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *key = strtok(rest, " ");
            if (!key) continue;
            char *value = strtok(NULL, "");
            if (value) remove_value(key, value);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *key = strtok(rest, " ");
            if (key) erase_key(key);
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key = strtok(rest, " ");
            if (key) values_key(key);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_all();
        }
    }
    free_all();
    return 0;
}