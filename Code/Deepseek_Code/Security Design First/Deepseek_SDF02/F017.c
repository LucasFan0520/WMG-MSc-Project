/* F017.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct kv {
    char *key;
    char *value;
    struct kv *next;
} kv_t;

kv_t *head = NULL;
kv_t *tail = NULL;

kv_t *find_kv(const char *key) {
    kv_t *k = head;
    while (k) {
        if (strcmp(k->key, key) == 0) return k;
        k = k->next;
    }
    return NULL;
}

void set_kv(const char *key, const char *value) {
    kv_t *k = find_kv(key);
    if (k) {
        char *new = strdup(value);
        if (!new) return;
        free(k->value);
        k->value = new;
        return;
    }
    k = malloc(sizeof(kv_t));
    if (!k) return;
    k->key = strdup(key);
    k->value = strdup(value);
    k->next = NULL;
    if (!k->key || !k->value) {
        free(k->key); free(k->value); free(k);
        return;
    }
    if (tail) tail->next = k;
    else head = k;
    tail = k;
}

void unset_kv(const char *key) {
    kv_t *prev = NULL, *k = head;
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
    kv_t *k = find_kv(key);
    if (!k) {
        printf("NOT_FOUND\n");
        return;
    }
    printf("%s\n", k->value);
}

void write_kv(void) {
    kv_t *k = head;
    while (k) {
        printf("%s=%s\n", k->key, k->value);
        k = k->next;
    }
}

void free_all(void) {
    kv_t *k = head;
    while (k) {
        kv_t *nxt = k->next;
        free(k->key);
        free(k->value);
        free(k);
        k = nxt;
    }
}

int main(int argc, char **argv) {
    if (argc < 2) return 1;
    FILE *f = fopen(argv[1], "r");
    if (f) {
        char *line = NULL;
        size_t len = 0;
        while (getline(&line, &len, f) != -1) {
            line[strcspn(line, "\n")] = 0;
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = 0;
                set_kv(line, eq+1);
            }
        }
        free(line);
        fclose(f);
    }
    char *line = NULL;
    size_t len = 0;
    while (getline(&line, &len, stdin) != -1) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char cmd[10], key[256];
        if (sscanf(line, "%9s %255s", cmd, key) >= 1) {
            const char *rest = line + strlen(cmd) + 1;
            if (strcmp(cmd, "SET") == 0) {
                const char *val = rest + strlen(key) + 1;
                set_kv(key, val);
            } else if (strcmp(cmd, "UNSET") == 0) {
                unset_kv(key);
            } else if (strcmp(cmd, "GET") == 0) {
                get_kv(key);
            } else if (strcmp(cmd, "WRITE") == 0) {
                write_kv();
            }
        }
    }
    free(line);
    free_all();
    return 0;
}