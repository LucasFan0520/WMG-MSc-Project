/* F017.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct kv {
    char *key;
    char *value;
    struct kv *next;
} KV;

KV *head = NULL;

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
    KV *new = malloc(sizeof(KV));
    new->key = strdup(key);
    new->value = strdup(value);
    new->next = head;
    head = new;
}

void unset_kv(const char *key) {
    KV *k = head, *prev = NULL;
    while (k) {
        if (strcmp(k->key, key) == 0) {
            if (prev) prev->next = k->next;
            else head = k->next;
            free(k->key); free(k->value); free(k);
            return;
        }
        prev = k;
        k = k->next;
    }
}

void get_kv(const char *key) {
    KV *k = find_kv(key);
    if (!k) { printf("NOT_FOUND\n"); return; }
    printf("%s\n", k->value);
}

void write_kv() {
    KV *k = head;
    while (k) {
        printf("%s=%s\n", k->key, k->value);
        k = k->next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = '\0';
                char key[256], value[1024];
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = '\0';
                    strcpy(key, line);
                    strcpy(value, eq + 1);
                    set_kv(key, value);
                }
            }
            fclose(f);
        }
    }

    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(line, "END") == 0) break;
        char cmd[16], key[256], value[1024];
        if (sscanf(line, "%15s", cmd) < 1) continue;
        if (strcmp(cmd, "WRITE") == 0) { write_kv(); continue; }
        if (strcmp(cmd, "SET") == 0) {
            if (sscanf(line, "%*s %255s", key) < 1) continue;
            char *rest = line + strlen(cmd) + 1 + strlen(key) + 1;
            if (*rest == ' ') rest++;
            set_kv(key, rest);
        } else if (strcmp(cmd, "UNSET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1) unset_kv(key);
        } else if (strcmp(cmd, "GET") == 0) {
            if (sscanf(line, "%*s %255s", key) == 1) get_kv(key);
        }
    }
    return 0;
}