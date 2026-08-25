// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Val {
    char *text;
    struct Val *next;
} Val;

typedef struct Key {
    char *name;
    Val *vals;
    struct Key *next;
} Key;

Key *head = NULL;

Key *find_key(const char *name) {
    for (Key *k = head; k; k = k->next) if (strcmp(k->name, name) == 0) return k;
    return NULL;
}

int main() {
    char cmd[20], k[256], v[256];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s", k, v);
            Key *key = find_key(k);
            if (!key) {
                key = malloc(sizeof(Key));
                key->name = strdup(k); key->vals = NULL; key->next = head; head = key;
            }
            Val *nv = malloc(sizeof(Val));
            nv->text = strdup(v); nv->next = key->vals; key->vals = nv;
        } else if (strcmp(cmd, "REMOVE") == 0) {
            scanf("%s %s", k, v);
            Key *key = find_key(k);
            if (key) {
                Val *prev = NULL, *curr = key->vals;
                while (curr) {
                    if (strcmp(curr->text, v) == 0) {
                        if (prev) prev->next = curr->next; else key->vals = curr->next;
                        free(curr->text); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "ERASE") == 0) {
            scanf("%s", k);
            Key *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->name, k) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    while (curr->vals) { Val *nv = curr->vals->next; free(curr->vals->text); free(curr->vals); curr->vals = nv; }
                    free(curr->name); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "VALUES") == 0) {
            scanf("%s", k);
            Key *key = find_key(k);
            if (key) for (Val *v = key->vals; v; v = v->next) printf("%s\n", v->text);
        } else if (strcmp(cmd, "KEYS") == 0) {
            for (Key *key = head; key; key = key->next) printf("%s\n", key->name);
        }
    }
    return 0;
}