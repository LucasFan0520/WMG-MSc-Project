// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Value {
    char *val;
    struct Value *next;
} Value;

typedef struct Key {
    char *key;
    Value *values;
    struct Key *next;
} Key;

Key *head = NULL;

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
        k->next = head;
        head = k;
    }
    Value *v = malloc(sizeof(Value));
    v->val = malloc(strlen(value) + 1);
    strcpy(v->val, value);
    v->next = k->values;
    k->values = v;
}

void remove_value(const char *key, const char *value) {
    Key *k = find_key(key);
    if (!k) return;
    Value *prev = NULL;
    Value *v = k->values;
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
    Key *prev = NULL;
    Key *k = head;
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

void keys_list(void) {
    Key *k = head;
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
    Key *k = head;
    while (k) {
        Key *nextk = k->next;
        free(k->key);
        Value *v = k->values;
        while (v) {
            Value *nextv = v->next;
            free(v->val);
            free(v);
            v = nextv;
        }
        free(k);
        k = nextk;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            add_value(key, p);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *key = p;
            while (*p && *p != ' ') p++;
            if (*p) *p++ = 0;
            while (*p == ' ') p++;
            remove_value(key, p);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            erase_key(p);
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            values_key(p);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_list();
        }
    }
    free_all();
    return 0;
}