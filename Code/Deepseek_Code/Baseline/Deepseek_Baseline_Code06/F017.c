/* F017.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct KV {
    char *key;
    char *value;
    struct KV *next;
} KV;

KV *head = NULL;

KV* find_kv(const char *key) {
    KV *kv = head;
    while (kv) {
        if (strcmp(kv->key, key) == 0) return kv;
        kv = kv->next;
    }
    return NULL;
}

void set_kv(const char *key, const char *value) {
    KV *kv = find_kv(key);
    if (kv) {
        free(kv->value);
        kv->value = strdup(value);
    } else {
        kv = malloc(sizeof(KV));
        kv->key = strdup(key);
        kv->value = strdup(value);
        kv->next = head;
        head = kv;
    }
}

void unset_kv(const char *key) {
    KV *kv = head, *prev = NULL;
    while (kv) {
        if (strcmp(kv->key, key) == 0) {
            if (prev) prev->next = kv->next;
            else head = kv->next;
            free(kv->key);
            free(kv->value);
            free(kv);
            return;
        }
        prev = kv;
        kv = kv->next;
    }
}

void get_kv(const char *key) {
    KV *kv = find_kv(key);
    if (kv) printf("%s\n", kv->value);
}

void write_kv() {
    KV *kv = head;
    while (kv) {
        printf("%s=%s\n", kv->key, kv->value);
        kv = kv->next;
    }
}

void load_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return;
    char line[4096];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        char key[256], value[4096];
        if (sscanf(line, "%255[^=]=%[^\n]", key, value) == 2) {
            set_kv(key, value);
        }
    }
    fclose(f);
}

void free_all() {
    while (head) {
        KV *tmp = head;
        head = head->next;
        free(tmp->key);
        free(tmp->value);
        free(tmp);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) load_file(argv[1]);
    char line[8192];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[32]; char key[256]; char value[4096];
        int n = sscanf(line, "%31s %255s %[^\n]", cmd, key, value);
        if (strcmp(cmd, "SET") == 0 && n == 3) {
            set_kv(key, value);
        } else if (strcmp(cmd, "UNSET") == 0 && n == 2) {
            unset_kv(key);
        } else if (strcmp(cmd, "GET") == 0 && n == 2) {
            get_kv(key);
        } else if (strcmp(cmd, "WRITE") == 0 && n == 1) {
            write_kv();
        }
    }
    free_all();
    return 0;
}