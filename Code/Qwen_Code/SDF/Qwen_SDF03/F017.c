// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct KV {
    char *key, *val;
    struct KV *next;
} KV;

KV *head = NULL;

char *get_token(char **s) {
    while (**s == ' ') (*s)++;
    if (!**s) return NULL;
    char *start = *s;
    while (**s && **s != ' ') (*s)++;
    if (**s) { **s = '\0'; (*s)++; }
    return start;
}

KV *find_kv(const char *key) {
    for (KV *k = head; k; k = k->next)
        if (strcmp(k->key, key) == 0) return k;
    return NULL;
}

void parse_kv(char *line) {
    char *eq = strchr(line, '=');
    if (eq) {
        *eq = '\0';
        char *key = line;
        char *val = eq + 1;
        KV *k = find_kv(key);
        if (!k) {
            k = malloc(sizeof(KV));
            if (k) { k->key = strdup(key); k->val = strdup(val); k->next = head; head = k; }
        } else { char *tmp = strdup(val); if (tmp) { free(k->val); k->val = tmp; } }
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                parse_kv(line);
            }
            fclose(f);
        }
    }
    char line[65536];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char *ptr = line;
        char *cmd = get_token(&ptr);
        if (!cmd) continue;
        if (strcmp(cmd, "SET") == 0) {
            char *key = get_token(&ptr);
            if (key) {
                KV *k = find_kv(key);
                if (!k) {
                    k = malloc(sizeof(KV));
                    if (k) { k->key = strdup(key); k->val = strdup(ptr); k->next = head; head = k; }
                } else { char *tmp = strdup(ptr); if (tmp) { free(k->val); k->val = tmp; } }
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = get_token(&ptr);
            if (key) {
                KV *prev = NULL, *curr = head;
                while (curr) {
                    if (strcmp(curr->key, key) == 0) {
                        if (prev) prev->next = curr->next; else head = curr->next;
                        free(curr->key); free(curr->val); free(curr); break;
                    }
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = get_token(&ptr);
            if (key) {
                KV *k = find_kv(key);
                if (k) printf("%s\n", k->val);
            }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (KV *k = head; k; k = k->next) printf("%s=%s\n", k->key, k->val);
        }
    }
    while (head) {
        KV *next = head->next;
        free(head->key); free(head->val); free(head);
        head = next;
    }
    return 0;
}