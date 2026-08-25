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
int in_trans = 0;

KV *find_kv(KV *list, const char *key) {
    for (KV *p = list; p; p = p->next)
        if (strcmp(p->key, key) == 0) return p;
    return NULL;
}

void set_kv(KV **list, const char *key, const char *val) {
    KV *p = find_kv(*list, key);
    if (p) {
        free(p->val);
        p->val = strdup(val);
    } else {
        p = malloc(sizeof(KV));
        p->key = strdup(key);
        p->val = strdup(val);
        p->next = *list;
        *list = p;
    }
}

void unset_kv(KV **list, const char *key) {
    KV *p = *list, *prev = NULL;
    while (p) {
        if (strcmp(p->key, key) == 0) {
            if (prev) prev->next = p->next;
            else *list = p->next;
            free(p->key); free(p->val); free(p);
            return;
        }
        prev = p; p = p->next;
    }
}

void get_kv(const char *key) {
    KV *p = find_kv(staged, key);
    if (!p) p = find_kv(committed, key);
    if (p) printf("%s\n", p->val);
}

void dump_kv() {
    for (KV *p = committed; p; p = p->next) {
        printf("%s=%s\n", p->key, p->val);
    }
}

void free_list(KV **list) {
    while (*list) {
        KV *n = (*list)->next;
        free((*list)->key); free((*list)->val); free(*list);
        *list = n;
    }
}

int main() {
    char line[10000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[20], a[1000], b[8000];
        b[0] = 0;
        sscanf(line, "%s", cmd);
        if (strcmp(cmd, "SET") == 0) {
            sscanf(line, "%*s %s %[^\n]", a, b);
            if (in_trans) set_kv(&staged, a, b);
            else set_kv(&committed, a, b);
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_trans = 1;
            free_list(&staged);
        } else if (strcmp(cmd, "TSET") == 0) {
            sscanf(line, "%*s %s %[^\n]", a, b);
            set_kv(&staged, a, b);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            for (KV *p = staged; p; p = p->next) {
                set_kv(&committed, p->key, p->val);
            }
            free_list(&staged);
            in_trans = 0;
        } else if (strcmp(cmd, "ABORT") == 0) {
            free_list(&staged);
            in_trans = 0;
        } else if (strcmp(cmd, "GET") == 0) {
            sscanf(line, "%*s %s", a);
            get_kv(a);
        } else if (strcmp(cmd, "DUMP") == 0) {
            dump_kv();
        }
    }
    free_list(&committed);
    free_list(&staged);
    return 0;
}