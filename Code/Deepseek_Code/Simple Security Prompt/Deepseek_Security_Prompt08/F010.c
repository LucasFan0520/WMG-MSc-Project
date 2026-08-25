// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Value {
    char *str;
    struct Value *next;
} Value;

typedef struct Key {
    char *key;
    Value *values;
    struct Key *next;
} Key;

Key *head = NULL, *tail = NULL;

Key *find_key(const char *key) {
    Key *k = head;
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
        k->key = malloc(strlen(key) + 1);
        strcpy(k->key, key);
        k->values = NULL;
        k->next = NULL;
        if (tail) {
            tail->next = k;
            tail = k;
        } else {
            head = tail = k;
        }
    }
    Value *v = malloc(sizeof(Value));
    v->str = malloc(strlen(value) + 1);
    strcpy(v->str, value);
    v->next = k->values;
    k->values = v;
}

void remove_value(const char *key, const char *value) {
    Key *k = find_key(key);
    if (!k) return;
    Value *prev = NULL, *curr = k->values;
    while (curr) {
        if (strcmp(curr->str, value) == 0) {
            if (prev) prev->next = curr->next;
            else k->values = curr->next;
            free(curr->str);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void erase_key(const char *key) {
    Key *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            if (curr == tail) tail = prev;
            Value *v = curr->values;
            while (v) {
                Value *next = v->next;
                free(v->str);
                free(v);
                v = next;
            }
            free(curr->key);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void values_for_key(const char *key) {
    Key *k = find_key(key);
    if (!k) return;
    Value *v = k->values;
    while (v) {
        printf("%s\n", v->str);
        v = v->next;
    }
}

void print_keys(void) {
    Key *k = head;
    while (k) {
        printf("%s\n", k->key);
        k = k->next;
    }
}

void free_all(void) {
    Key *k = head;
    while (k) {
        Key *nextk = k->next;
        Value *v = k->values;
        while (v) {
            Value *nextv = v->next;
            free(v->str);
            free(v);
            v = nextv;
        }
        free(k->key);
        free(k);
        k = nextk;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (line[read - 1] == '\n') line[read - 1] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, " ");
            if (key && value) add_value(key, value);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *key = strtok(NULL, " ");
            char *value = strtok(NULL, " ");
            if (key && value) remove_value(key, value);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *key = strtok(NULL, " ");
            if (key) erase_key(key);
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *key = strtok(NULL, " ");
            if (key) values_for_key(key);
        } else if (strcmp(cmd, "KEYS") == 0) {
            print_keys();
        }
    }
    free(line);
    free_all();
    return 0;
}