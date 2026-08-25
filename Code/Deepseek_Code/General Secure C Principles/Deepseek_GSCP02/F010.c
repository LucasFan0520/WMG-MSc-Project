// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Value {
    char *text;
    struct Value *next;
} Value;

typedef struct Key {
    char *name;
    Value *val_head;
    Value *val_tail;
    struct Key *next;
} Key;

Key *k_head = NULL;

Key *find_key(const char *name) {
    Key *k = k_head;
    while (k) {
        if (strcmp(k->name, name) == 0) return k;
        k = k->next;
    }
    return NULL;
}

void add_value(Key *k, const char *val) {
    Value *v = malloc(sizeof(Value));
    if (!v) return;
    v->text = strdup(val);
    v->next = NULL;
    if (k->val_tail) {
        k->val_tail->next = v;
        k->val_tail = v;
    } else {
        k->val_head = k->val_tail = v;
    }
}

void add_key_value(const char *key, const char *val) {
    Key *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(Key));
        if (!k) return;
        k->name = strdup(key);
        k->val_head = k->val_tail = NULL;
        k->next = k_head;
        k_head = k;
    }
    add_value(k, val);
}

void remove_key_value(const char *key, const char *val) {
    Key *k = find_key(key);
    if (!k) return;
    Value *prev = NULL;
    Value *v = k->val_head;
    while (v) {
        if (strcmp(v->text, val) == 0) {
            if (prev) prev->next = v->next;
            else k->val_head = v->next;
            if (v == k->val_tail) k->val_tail = prev;
            free(v->text);
            free(v);
            return;
        }
        prev = v;
        v = v->next;
    }
}

void erase_key(const char *key) {
    Key *prev = NULL;
    Key *k = k_head;
    while (k) {
        if (strcmp(k->name, key) == 0) {
            if (prev) prev->next = k->next;
            else k_head = k->next;
            free(k->name);
            Value *v = k->val_head;
            while (v) {
                Value *next = v->next;
                free(v->text);
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
    Key *k = find_key(key);
    if (!k) return;
    Value *v = k->val_head;
    while (v) {
        for (char *p = v->text; *p; p++) {
            putchar(*p == ' ' ? '_' : *p);
        }
        putchar('\n');
        v = v->next;
    }
}

void keys_all(void) {
    Key *k = k_head;
    while (k) {
        printf("%s\n", k->name);
        k = k->next;
    }
}

void free_all(void) {
    while (k_head) {
        Key *next = k_head->next;
        free(k_head->name);
        Value *v = k_head->val_head;
        while (v) {
            Value *nextv = v->next;
            free(v->text);
            free(v);
            v = nextv;
        }
        free(k_head);
        k_head = next;
    }
}

int main(void) {
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    while ((nread = getline(&line, &len, stdin)) != -1) {
        if (line[nread-1] == '\n') line[nread-1] = '\0';
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            char key[256], val[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", key, val) == 2) {
                add_key_value(key, val);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char key[256], val[1024];
            if (sscanf(line, "%*s %255s %1023[^\n]", key, val) == 2) {
                remove_key_value(key, val);
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) == 1) {
                erase_key(key);
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char key[256];
            if (sscanf(line, "%*s %255s", key) == 1) {
                values_key(key);
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_all();
        }
    }

    free(line);
    free_all();
    return 0;
}