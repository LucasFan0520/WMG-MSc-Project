/*
 * F017.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct kv {
    char *key;
    char *value;
    struct kv *next;
} KV;

KV *kvs = NULL;

KV *find_kv(const char *key) {
    KV *p = kvs;
    while (p) {
        if (strcmp(p->key, key) == 0) return p;
        p = p->next;
    }
    return NULL;
}

void set_kv(const char *key, const char *value) {
    KV *p = find_kv(key);
    if (p) {
        free(p->value);
        p->value = strdup(value);
    } else {
        p = malloc(sizeof(KV));
        p->key = strdup(key);
        p->value = strdup(value);
        p->next = kvs;
        kvs = p;
    }
}

void unset_kv(const char *key) {
    KV *prev = NULL, *p = kvs;
    while (p) {
        if (strcmp(p->key, key) == 0) {
            if (prev) prev->next = p->next;
            else kvs = p->next;
            free(p->key);
            free(p->value);
            free(p);
            return;
        }
        prev = p;
        p = p->next;
    }
}

void get_kv(const char *key) {
    KV *p = find_kv(key);
    if (!p) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", p->value);
}

void write_kvs(void) {
    KV *p = kvs;
    while (p) {
        printf("%s=%s\n", p->key, p->value);
        p = p->next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[4096];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = 0;
                    set_kv(line, eq+1);
                }
            }
            fclose(f);
        }
    }
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[32], key[256];
        if (sscanf(line, "%31s %255s", cmd, key) < 1) continue;
        char *rest = line + strlen(cmd) + strlen(key) + 1;
        while (*rest == ' ') rest++;
        if (strcmp(cmd, "SET") == 0) {
            set_kv(key, rest);
        } else if (strcmp(cmd, "UNSET") == 0) {
            unset_kv(key);
        } else if (strcmp(cmd, "GET") == 0) {
            get_kv(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_kvs();
        }
    }
    return 0;
}