/* F010.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Value {
    char *text;
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
        if (!k) return;
        k->key = strdup(key);
        k->values = NULL;
        k->next = head;
        head = k;
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
    Key *prev = NULL, *curr = head;
    while (curr) {
        if (strcmp(curr->key, key) == 0) {
            if (prev) prev->next = curr->next;
            else head = curr->next;
            Value *v = curr->values;
            while (v) {
                Value *tmp = v;
                v = v->next;
                free(tmp->text);
                free(tmp);
            }
            free(curr->key);
            free(curr);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void values_key(const char *key) {
    Key *k = find_key(key);
    if (!k) return;
    Value *v = k->values;
    int first = 1;
    while (v) {
        if (!first) printf(" ");
        printf("%s", v->text);
        first = 0;
        v = v->next;
    }
    printf("\n");
}

void keys_all(void) {
    Key *k = head;
    int first = 1;
    while (k) {
        if (!first) printf(" ");
        printf("%s", k->key);
        first = 0;
        k = k->next;
    }
    printf("\n");
}

void free_all(void) {
    Key *k = head;
    while (k) {
        Key *tmp = k;
        k = k->next;
        Value *v = tmp->values;
        while (v) {
            Value *tmpv = v;
            v = v->next;
            free(tmpv->text);
            free(tmpv);
        }
        free(tmp->key);
        free(tmp);
    }
}

int main(void) {
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "ADD") == 0) {
            char key[256], val[256];
            if (sscanf(line + 4, "%255s %255s", key, val) == 2) {
                add_value(key, val);
            }
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char key[256], val[256];
            if (sscanf(line + 7, "%255s %255s", key, val) == 2) {
                remove_value(key, val);
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char *p = line + 6;
            while (*p == ' ') p++;
            erase_key(p);
        } else if (strcmp(cmd, "VALUES") == 0) {
            char *p = line + 7;
            while (*p == ' ') p++;
            values_key(p);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_all();
        }
    }
    free_all();
    return 0;
}