// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct KV {
    char *key, *val;
    struct KV *next;
} KV;

KV *head = NULL;

void free_list() {
    while (head) {
        KV *t = head; head = head->next;
        free(t->key); free(t->val); free(t);
    }
}

KV *find_kv(const char *key) {
    for (KV *k = head; k; k = k->next)
        if (strcmp(k->key, key) == 0) return k;
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = 0;
                    KV *k = malloc(sizeof(KV));
                    k->key = strdup(line); k->val = strdup(eq + 1);
                    k->next = head; head = k;
                }
            }
            fclose(f);
        }
    }

    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], key[256], val[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, val) < 1) continue;
            KV *k = find_kv(key);
            if (k) { free(k->val); k->val = strdup(val); }
            else {
                k = malloc(sizeof(KV));
                k->key = strdup(key); k->val = strdup(val); k->next = head; head = k;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (sscanf(line, "%*s %255s", key) != 1) continue;
            KV *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->key, key) == 0) {
                    KV *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->key); free(tmp->val); free(tmp);
                    break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s", key) != 1) continue;
            KV *k = find_kv(key);
            if (k) printf("%s\n", k->val);
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (KV *k = head; k; k = k->next) printf("%s=%s\n", k->key, k->val);
        }
    }
    free_list();
    return 0;
}