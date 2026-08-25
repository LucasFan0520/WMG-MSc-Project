// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct kv {
    char *key;
    char *value;
    struct kv *next;
} KV;

KV *committed = NULL;
KV *staged = NULL;

KV *find_committed(const char *key) {
    KV *k = committed;
    while (k) {
        if (strcmp(k->key, key) == 0) return k;
        k = k->next;
    }
    return NULL;
}

KV *find_staged(const char *key) {
    KV *k = staged;
    while (k) {
        if (strcmp(k->key, key) == 0) return k;
        k = k->next;
    }
    return NULL;
}

void set_committed(const char *key, const char *value) {
    KV *k = find_committed(key);
    if (k) {
        free(k->value);
        k->value = strdup(value);
        return;
    }
    k = malloc(sizeof(KV));
    k->key = strdup(key);
    k->value = strdup(value);
    k->next = committed;
    committed = k;
}

void set_staged(const char *key, const char *value) {
    KV *k = find_staged(key);
    if (k) {
        free(k->value);
        k->value = strdup(value);
        return;
    }
    k = malloc(sizeof(KV));
    k->key = strdup(key);
    k->value = strdup(value);
    k->next = staged;
    staged = k;
}

void unset_committed(const char *key) {
    KV *k = committed, *prev = NULL;
    while (k) {
        if (strcmp(k->key, key) == 0) {
            if (prev) prev->next = k->next;
            else committed = k->next;
            free(k->key);
            free(k->value);
            free(k);
            return;
        }
        prev = k;
        k = k->next;
    }
}

void commit_staged(void) {
    KV *s = staged;
    while (s) {
        set_committed(s->key, s->value);
        s = s->next;
    }
    // clear staged
    while (staged) {
        KV *tmp = staged;
        staged = staged->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

void abort_staged(void) {
    while (staged) {
        KV *tmp = staged;
        staged = staged->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

void get_key(const char *key) {
    KV *s = find_staged(key);
    if (s) { printf("%s\n", s->value); return; }
    KV *c = find_committed(key);
    if (c) printf("%s\n", c->value);
}

void dump_committed(void) {
    KV *k = committed;
    while (k) {
        printf("%s %s\n", k->key, k->value);
        k = k->next;
    }
}

int main(void) {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char key[256], value[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", key, value) == 2)
                set_committed(key, value);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            abort_staged(); // start fresh
        } else if (strcmp(cmd, "TSET") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", key, value) == 2)
                set_staged(key, value);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            commit_staged();
        } else if (strcmp(cmd, "ABORT") == 0) {
            abort_staged();
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                get_key(key);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_committed();
        }
    }
    return 0;
}