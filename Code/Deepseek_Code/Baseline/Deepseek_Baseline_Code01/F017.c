// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct kv {
    char *key;
    char *value;
    struct kv *next;
} KV;

KV *head = NULL;
KV *tail = NULL;

KV *find_kv(const char *key) {
    KV *k = head;
    while (k) {
        if (strcmp(k->key, key) == 0) return k;
        k = k->next;
    }
    return NULL;
}

void set_kv(const char *key, const char *value) {
    KV *k = find_kv(key);
    if (k) {
        free(k->value);
        k->value = strdup(value);
        return;
    }
    k = malloc(sizeof(KV));
    k->key = strdup(key);
    k->value = strdup(value);
    k->next = NULL;
    if (tail) tail->next = k;
    else head = k;
    tail = k;
}

void unset_kv(const char *key) {
    KV *k = head, *prev = NULL;
    while (k) {
        if (strcmp(k->key, key) == 0) {
            if (prev) prev->next = k->next;
            else head = k->next;
            if (k == tail) tail = prev;
            free(k->key);
            free(k->value);
            free(k);
            return;
        }
        prev = k;
        k = k->next;
    }
}

void get_kv(const char *key) {
    KV *k = find_kv(key);
    if (k) printf("%s\n", k->value);
}

void write_kv(void) {
    KV *k = head;
    while (k) {
        printf("%s=%s\n", k->key, k->value);
        k = k->next;
    }
}

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            set_kv(line, eq + 1);
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        if (strcmp(line, "END\n") == 0 || strcmp(line, "END") == 0) break;
        char cmd[32];
        char key[256], value[4096];
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s %[^\n]", key, value) == 2)
                set_kv(key, value);
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                unset_kv(key);
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1)
                get_kv(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_kv();
        }
    }
    return 0;
}