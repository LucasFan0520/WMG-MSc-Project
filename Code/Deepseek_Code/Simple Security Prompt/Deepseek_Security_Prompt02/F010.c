/* F010.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

void free_values(Value *v) {
    while (v) {
        Value *tmp = v;
        v = v->next;
        free(tmp->text);
        free(tmp);
    }
}

void free_key(Key *k) {
    free(k->name);
    free_values(k->values);
    free(k);
}

Key *add_key(const char *name) {
    Key *k = malloc(sizeof(Key));
    k->name = strdup(name);
    k->values = NULL;
    k->next = keys;
    keys = k;
    return k;
}

void add_value(const char *key, const char *value) {
    Key *k = find_key(key);
    if (!k) k = add_key(key);
    Value *v = malloc(sizeof(Value));
    v->text = strdup(value);
    v->next = k->values;
    k->values = v;
}

void remove_value(const char *key, const char *value) {
    Key *k = find_key(key);
    if (!k) return;
    Value *prev = NULL, *curr = k->values;
    while (curr) {
        if (strcmp(curr->text, value) == 0) {
            if (prev) prev->next = curr->next;
            else k->values = curr->next;
            free(curr->text);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void erase_key(const char *key) {
    Key *prev = NULL, *curr = keys;
    while (curr) {
        if (strcmp(curr->name, key) == 0) {
            if (prev) prev->next = curr->next;
            else keys = curr->next;
            free_key(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void values_key(const char *key) {
    Key *k = find_key(key);
    if (!k || !k->values) {
        printf("NOT_FOUND\n");
        return;
    }
    Value *v = k->values;
    while (v) {
        printf("%s\n", v->text);
        v = v->next;
    }
}

void keys_list(void) {
    Key *k = keys;
    while (k) {
        printf("%s\n", k->name);
        k = k->next;
    }
}

void trim_newline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len-1] == '\n') s[len-1] = '\0';
}

int main(void) {
    char line[4096];
    char cmd[32];
    char key[256], value[4096];

    while (fgets(line, sizeof(line), stdin)) {
        trim_newline(line);
        if (strcmp(line, "END") == 0) break;
        if (sscanf(line, "%31s", cmd) != 1) continue;

        if (strcmp(cmd, "ADD") == 0) {
            char *p = line + 3;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", key) != 1) continue;
            p += strlen(key);
            while (*p == ' ') p++;
            add_value(key, p);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", key) != 1) continue;
            p += strlen(key);
            while (*p == ' ') p++;
            remove_value(key, p);
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *p = line + 5;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", key) == 1) erase_key(key);
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            if (sscanf(p, "%255s", key) == 1) values_key(key);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_list();
        }
    }

    while (keys) {
        Key *tmp = keys;
        keys = keys->next;
        free_key(tmp);
    }
    return 0;
}