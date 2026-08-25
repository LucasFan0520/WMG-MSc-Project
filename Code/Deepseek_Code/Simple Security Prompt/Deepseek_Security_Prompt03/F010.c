/*
 * F010.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct val {
    char *text;
    struct val *next;
} Val;

typedef struct keyrec {
    char *key;
    Val *values;
    struct keyrec *next;
} KeyRec;

KeyRec *keys = NULL;

KeyRec *find_key(const char *key) {
    KeyRec *k = keys;
    while (k) {
        if (strcmp(k->key, key) == 0) return k;
        k = k->next;
    }
    return NULL;
}

void add_value(const char *key, const char *value) {
    KeyRec *k = find_key(key);
    if (!k) {
        k = malloc(sizeof(KeyRec));
        k->key = strdup(key);
        k->values = NULL;
        k->next = keys;
        keys = k;
    }
    Val *v = malloc(sizeof(Val));
    v->text = strdup(value);
    v->next = k->values;
    k->values = v;
}

void remove_value(const char *key, const char *value) {
    KeyRec *k = find_key(key);
    if (!k) return;
    Val *prev = NULL, *v = k->values;
    while (v) {
        if (strcmp(v->text, value) == 0) {
            if (prev) prev->next = v->next;
            else k->values = v->next;
            free(v->text);
            free(v);
            return;
        }
        prev = v;
        v = v->next;
    }
}

void erase_key(const char *key) {
    KeyRec *prev = NULL, *k = keys;
    while (k) {
        if (strcmp(k->key, key) == 0) {
            if (prev) prev->next = k->next;
            else keys = k->next;
            Val *v = k->values;
            while (v) {
                Val *tmp = v;
                v = v->next;
                free(tmp->text);
                free(tmp);
            }
            free(k->key);
            free(k);
            return;
        }
        prev = k;
        k = k->next;
    }
}

void values_key(const char *key) {
    KeyRec *k = find_key(key);
    if (!k) return;
    Val *v = k->values;
    while (v) {
        printf("%s\n", v->text);
        v = v->next;
    }
}

void keys_all(void) {
    KeyRec *k = keys;
    while (k) {
        printf("%s\n", k->key);
        k = k->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], arg1[256], arg2[1024];
        if (sscanf(line, "%31s %255s", cmd, arg1) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(arg1) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "ADD") == 0) {
            add_value(arg1, rest);
        } else if (strcmp(cmd, "REMOVE") == 0) {
            remove_value(arg1, rest);
        } else if (strcmp(cmd, "ERASE") == 0) {
            erase_key(arg1);
        } else if (strcmp(cmd, "VALUES") == 0) {
            values_key(arg1);
        } else if (strcmp(cmd, "KEYS") == 0) {
            keys_all();
        }
    }
    return 0;
}