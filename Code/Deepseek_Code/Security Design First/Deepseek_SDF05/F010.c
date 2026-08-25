// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 4096

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

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    char *copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len + 1);
    return copy;
}

void add_value(const char *key_name, const char *value) {
    Key *k = find_key(key_name);
    if (!k) {
        k = malloc(sizeof(Key));
        if (!k) return;
        k->name = safe_strdup(key_name);
        k->values = NULL;
        k->next = keys;
        keys = k;
    }
    Value *v = malloc(sizeof(Value));
    if (!v) return;
    v->text = safe_strdup(value);
    v->next = k->values;
    k->values = v;
}

void remove_value(const char *key_name, const char *value) {
    Key *k = find_key(key_name);
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

void erase_key(const char *key_name) {
    Key *prev = NULL, *cur = keys;
    while (cur) {
        if (strcmp(cur->name, key_name) == 0) {
            if (prev) prev->next = cur->next;
            else keys = cur->next;
            free(cur->name);
            Value *v = cur->values;
            while (v) {
                Value *next = v->next;
                free(v->text);
                free(v);
                v = next;
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }
}

void values_for_key(const char *key_name) {
    Key *k = find_key(key_name);
    if (!k) return;
    Value *v = k->values;
    while (v) {
        printf("%s\n", v->text);
        v = v->next;
    }
}

void list_keys(void) {
    Key *k = keys;
    while (k) {
        printf("%s\n", k->name);
        k = k->next;
    }
}

void free_all(void) {
    Key *k = keys;
    while (k) {
        Key *next_k = k->next;
        free(k->name);
        Value *v = k->values;
        while (v) {
            Value *next_v = v->next;
            free(v->text);
            free(v);
            v = next_v;
        }
        free(k);
        k = next_k;
    }
}

int main(void) {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;

        char cmd[32];
        char arg1[512], arg2[512];
        int n = sscanf(line, "%31s %511s %511s", cmd, arg1, arg2);
        if (n < 1) continue;

        if (strcmp(cmd, "ADD") == 0 && n >= 3) {
            const char *rest = line;
            const char *p = rest;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            while (*p && !isspace(*p)) p++;
            while (*p && isspace(*p)) p++;
            add_value(arg1, p);
        } else if (strcmp(cmd, "REMOVE") == 0 && n >= 3) {
            remove_value(arg1, arg2);
        } else if (strcmp(cmd, "ERASE") == 0 && n >= 2) {
            erase_key(arg1);
        } else if (strcmp(cmd, "VALUES") == 0 && n >= 2) {
            values_for_key(arg1);
        } else if (strcmp(cmd, "KEYS") == 0) {
            list_keys();
        }
    }
    free_all();
    return 0;
}