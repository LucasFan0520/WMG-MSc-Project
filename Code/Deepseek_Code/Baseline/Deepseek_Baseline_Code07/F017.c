// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct KV {
    char *key;
    char *value;
    struct KV *next;
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
        k->value = malloc(strlen(value)+1);
        strcpy(k->value, value);
    } else {
        KV *new = malloc(sizeof(KV));
        new->key = malloc(strlen(key)+1);
        strcpy(new->key, key);
        new->value = malloc(strlen(value)+1);
        strcpy(new->value, value);
        new->next = NULL;
        if (tail) tail->next = new;
        else head = new;
        tail = new;
    }
}

void unset_kv(const char *key) {
    KV *prev = NULL;
    KV *k = head;
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

void free_all(void) {
    KV *k = head;
    while (k) {
        KV *next = k->next;
        free(k->key);
        free(k->value);
        free(k);
        k = next;
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        FILE *f = fopen(argv[1], "r");
        if (f) {
            char line[1024];
            while (fgets(line, sizeof(line), f)) {
                line[strcspn(line, "\n")] = 0;
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = 0;
                    char *key = line;
                    char *value = eq + 1;
                    set_kv(key, value);
                }
            }
            fclose(f);
        }
    }
    char line[1024];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) break;
        char *cmd = strtok(line, " ");
        if (!cmd) continue;
        char *rest = strtok(NULL, "");
        if (!rest) rest = "";
        if (strcmp(cmd, "SET") == 0) {
            char *key = strtok(rest, " ");
            if (!key) continue;
            char *value = strtok(NULL, "");
            if (!value) value = "";
            set_kv(key, value);
        } else if (strcmp(cmd, "UNSET") == 0) {
            char *key = strtok(rest, " ");
            if (key) unset_kv(key);
        } else if (strcmp(cmd, "GET") == 0) {
            char *key = strtok(rest, " ");
            if (key) get_kv(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_kv();
        }
    }
    free_all();
    return 0;
}