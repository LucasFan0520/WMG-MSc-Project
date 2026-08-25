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
    Value *values;
    struct Key *next;
} Key;

Key *keys = NULL;

Key *find_key(const char *name) {
    Key *k = keys;
    while (k) {
        if (strcmp(k->name, name) == 0) return k;
        k = k->next;
    }
    return NULL;
}

void add_value(const char *key, const char *value) {
    Key *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(Key));
        if (!k) return;
        k->name = strdup(key);
        k->values = NULL;
        k->next = keys;
        keys = k;
    }
    Value *v = malloc(sizeof(Value));
    if (!v) return;
    v->text = strdup(value);
    v->next = k->values;
    k->values = v;
}

void remove_value(const char *key, const char *value) {
    Key *k = find_key(key);
    if (!k) return;
    Value *prev = NULL, *cur = k->values;
    while (cur) {
        if (strcmp(cur->text, value) == 0) {
            if (prev) prev->next = cur->next;
            else k->values = cur->next;
            free(cur->text);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void erase_key(const char *key) {
    Key *prev = NULL, *cur = keys;
    while (cur) {
        if (strcmp(cur->name, key) == 0) {
            if (prev) prev->next = cur->next;
            else keys = cur->next;
            Value *v = cur->values;
            while (v) {
                Value *next = v->next;
                free(v->text);
                free(v);
                v = next;
            }
            free(cur->name);
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void values_for_key(const char *key) {
    Key *k = find_key(key);
    if (!k) return;
    Value *v = k->values;
    while (v) {
        printf("%s\n", v->text);
        v = v->next;
    }
}

void print_keys(void) {
    Key *k = keys;
    while (k) {
        printf("%s\n", k->name);
        k = k->next;
    }
}

void free_all(void) {
    Key *k = keys;
    while (k) {
        Key *next = k->next;
        Value *v = k->values;
        while (v) {
            Value *nextv = v->next;
            free(v->text);
            free(v);
            v = nextv;
        }
        free(k->name);
        free(k);
        k = next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 4;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (!space) continue;
            *space = '\0';
            char *val = space + 1;
            while (*val == ' ') val++;
            add_value(key, val);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (!space) continue;
            *space = '\0';
            char *val = space + 1;
            while (*val == ' ') val++;
            remove_value(key, val);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (space) *space = '\0';
            erase_key(key);
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            char *key = p;
            char *space = strchr(key, ' ');
            if (space) *space = '\0';
            values_for_key(key);
        } else if (strcmp(cmd, "KEYS") == 0) {
            print_keys();
        }
    }
    free_all();
    return 0;
}