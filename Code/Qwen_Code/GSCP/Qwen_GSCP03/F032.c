// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct KV {
    char *key;
    char *val;
    struct KV *next;
} KV;

KV *committed = NULL;
KV *staged = NULL;
int in_transaction = 0;

void free_kv(KV *list) {
    while (list) { KV *t = list; list = list->next; free(t->key); free(t->val); free(t); }
}

KV *find_kv(KV *list, const char *key) {
    for (KV *k = list; k; k = k->next)
        if (strcmp(k->key, key) == 0) return k;
    return NULL;
}

void set_kv(KV **list, const char *key, const char *val) {
    KV *k = find_kv(*list, key);
    if (k) { free(k->val); k->val = strdup(val); }
    else {
        k = malloc(sizeof(KV));
        k->key = strdup(key); k->val = strdup(val);
        k->next = *list; *list = k;
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], key[256], val[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, val) < 1) continue;
            set_kv(&committed, key, val);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            free_kv(staged);
            staged = NULL;
            in_transaction = 1;
        } else if (strcmp(cmd, "TSET") == 0) {
            if (!in_transaction) continue;
            if (sscanf(line, "%*s %255s %4095[^\n]", key, val) < 1) continue;
            set_kv(&staged, key, val);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (!in_transaction) continue;
            for (KV *s = staged; s; s = s->next) {
                set_kv(&committed, s->key, s->val);
            }
            free_kv(staged);
            staged = NULL;
            in_transaction = 0;
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (!in_transaction) continue;
            free_kv(staged);
            staged = NULL;
            in_transaction = 0;
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s", key) != 1) continue;
            KV *k = in_transaction ? find_kv(staged, key) : NULL;
            if (!k) k = find_kv(committed, key);
            if (k) printf("%s\n", k->val);
        } else if (strcmp(cmd, "DUMP") == 0) {
            for (KV *k = committed; k; k = k->next) {
                printf("%s=%s\n", k->key, k->val);
            }
        }
    }
    free_kv(committed);
    free_kv(staged);
    return 0;
}