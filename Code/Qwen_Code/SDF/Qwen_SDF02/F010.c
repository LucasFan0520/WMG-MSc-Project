// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

typedef struct Val {
    char *value;
    struct Val *next;
} Val;

typedef struct Key {
    char *name;
    Val *vals;
    struct Key *next;
} Key;

Key *keys = NULL;

Key* find_key(const char *name) {
    for (Key *k = keys; k; k = k->next)
        if (strcmp(k->name, name) == 0) return k;
    return NULL;
}

int main() {
    char line[MAX_LINE];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        
        char cmd[32];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        char *rest = line + strlen(cmd);
        while (*rest == ' ') rest++;
        
        if (strcmp(cmd, "ADD") == 0) {
            char key[256], val[256];
            if (sscanf(rest, "%255s %255s", key, val) != 2) continue;
            Key *k = find_key(key);
            if (!k) {
                k = malloc(sizeof(Key));
                if (!k) continue;
                k->name = strdup(key);
                k->vals = NULL;
                k->next = keys;
                keys = k;
            }
            Val *v = malloc(sizeof(Val));
            if (!v) continue;
            v->value = strdup(val);
            v->next = k->vals;
            k->vals = v;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            char key[256], val[256];
            if (sscanf(rest, "%255s %255s", key, val) != 2) continue;
            Key *k = find_key(key);
            if (k) {
                Val *prev = NULL;
                for (Val *v = k->vals; v; prev = v, v = v->next) {
                    if (strcmp(v->value, val) == 0) {
                        if (prev) prev->next = v->next;
                        else k->vals = v->next;
                        free(v->value);
                        free(v);
                        break;
                    }
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) != 1) continue;
            Key *prev = NULL;
            for (Key *k = keys; k; prev = k, k = k->next) {
                if (strcmp(k->name, key) == 0) {
                    if (prev) prev->next = k->next;
                    else keys = k->next;
                    while (k->vals) {
                        Val *v = k->vals;
                        k->vals = v->next;
                        free(v->value);
                        free(v);
                    }
                    free(k->name);
                    free(k);
                    break;
                }
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            char key[256];
            if (sscanf(rest, "%255s", key) != 1) continue;
            Key *k = find_key(key);
            if (k) {
                for (Val *v = k->vals; v; v = v->next) printf("%s\n", v->value);
            }
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (Key *k = keys; k; k = k->next) printf("%s\n", k->name);
        }
    }
    while (keys) {
        Key *k = keys;
        keys = keys->next;
        while (k->vals) {
            Val *v = k->vals;
            k->vals = v->next;
            free(v->value);
            free(v);
        }
        free(k->name);
        free(k);
    }
    return 0;
}