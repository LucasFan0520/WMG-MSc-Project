// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct KV {
    char *key, *val;
    struct KV *next;
} KV;

KV *committed = NULL;
KV *staged = NULL;
int in_transaction = 0;

void read_rest(char *buf, int size) {
    if (!fgets(buf, size, stdin)) { buf[0] = '\0'; return; }
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t len = strlen(buf);
    if (len > 0 && buf[len-1] == '\n') buf[len-1] = '\0';
}

void set_kv(KV **list, const char *key, const char *val) {
    KV *curr = *list;
    while (curr && strcmp(curr->key, key) != 0) curr = curr->next;
    if (curr) { free(curr->val); curr->val = strdup(val); }
    else {
        KV *nv = malloc(sizeof(KV));
        nv->key = strdup(key); nv->val = strdup(val); nv->next = *list; *list = nv;
    }
}

int main() {
    char cmd[20], key[256], val[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "SET") == 0) {
            scanf("%s", key); read_rest(val, sizeof(val));
            if (in_transaction) {
                set_kv(&staged, key, val);
            } else {
                set_kv(&committed, key, val);
            }
        } else if (strcmp(cmd, "BEGIN") == 0) {
            in_transaction = 1;
            for (KV *c = committed; c; c = c->next) set_kv(&staged, c->key, c->val);
        } else if (strcmp(cmd, "TSET") == 0) {
            scanf("%s", key); read_rest(val, sizeof(val));
            if (in_transaction) set_kv(&staged, key, val);
        } else if (strcmp(cmd, "COMMIT") == 0) {
            if (in_transaction) {
                while (committed) { KV *n = committed->next; free(committed->key); free(committed->val); free(committed); committed = n; }
                committed = staged;
                staged = NULL;
                in_transaction = 0;
            }
        } else if (strcmp(cmd, "ABORT") == 0) {
            if (in_transaction) {
                while (staged) { KV *n = staged->next; free(staged->key); free(staged->val); free(staged); staged = n; }
                in_transaction = 0;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            scanf("%s", key);
            KV *list = in_transaction ? staged : committed;
            for (KV *c = list; c; c = c->next) if (strcmp(c->key, key) == 0) { printf("%s\n", c->val); break; }
        } else if (strcmp(cmd, "DUMP") == 0) {
            KV *list = in_transaction ? staged : committed;
            for (KV *c = list; c; c = c->next) printf("%s=%s\n", c->key, c->val);
        }
    }
    return 0;
}