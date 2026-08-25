// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct KV { char *key, *val; struct KV *next; } KV;
KV *head = NULL;

void set_kv(const char *key, const char *val) {
    for (KV *kv = head; kv; kv = kv->next) {
        if (strcmp(kv->key, key) == 0) {
            free(kv->val);
            kv->val = strdup(val);
            return;
        }
    }
    KV *kv = malloc(sizeof(KV));
    kv->key = strdup(key);
    kv->val = strdup(val);
    kv->next = head;
    head = kv;
}

void unset_kv(const char *key) {
    KV **p = &head;
    while (*p) {
        if (strcmp((*p)->key, key) == 0) {
            KV *tmp = *p;
            *p = tmp->next;
            free(tmp->key);
            free(tmp->val);
            free(tmp);
            return;
        }
        p = &(*p)->next;
    }
}

void get_kv(const char *key) {
    for (KV *kv = head; kv; kv = kv->next) {
        if (strcmp(kv->key, key) == 0) { printf("%s\n", kv->val); return; }
    }
}

void write_kv() {
    for (KV *kv = head; kv; kv = kv->next) printf("%s=%s\n", kv->key, kv->val);
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = 0;
            set_kv(line, eq + 1);
        }
    }
    fclose(f);
}

void cleanup() {
    while (head) {
        KV *tmp = head->next;
        free(head->key);
        free(head->val);
        free(head);
        head = tmp;
    }
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], key[256], val[1024] = "";
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1) {
                char *t = strchr(line, ' ');
                if (t) { t = strchr(t + 1, ' '); if (t) strcpy(val, t + 1); }
                set_kv(key, val);
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1) unset_kv(key);
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1) get_kv(key);
        } else if (strcmp(cmd, "WRITE") == 0) write_kv();
    }
    cleanup();
    return 0;
}