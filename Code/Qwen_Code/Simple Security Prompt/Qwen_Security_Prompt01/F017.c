// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct KV {
    char *key;
    char *val;
    struct KV *next;
} KV;

KV *head = NULL;

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return;
    }
    char line[1000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = 0;
            KV *p = malloc(sizeof(KV));
            p->key = strdup(line);
            p->val = strdup(eq + 1);
            p->next = head;
            head = p;
        }
    }
    fclose(f);
}

void set_kv(const char *key, const char *val) {
    for (KV *p = head; p; p = p->next) {
        if (strcmp(p->key, key) == 0) {
            free(p->val);
            p->val = strdup(val);
            return;
        }
    }
    KV *p = malloc(sizeof(KV));
    p->key = strdup(key);
    p->val = strdup(val);
    p->next = head;
    head = p;
}

void unset_kv(const char *key) {
    KV *p = head;
    KV *prev = NULL;
    while (p) {
        if (strcmp(p->key, key) == 0) {
            KV *next = p->next;
            free(p->key);
            free(p->val);
            free(p);
            if (prev) {
                prev->next = next;
            } else {
                head = next;
            }
            return;
        }
        prev = p;
        p = p->next;
    }
}

void get_kv(const char *key) {
    for (KV *p = head; p; p = p->next) {
        if (strcmp(p->key, key) == 0) {
            printf("%s\n", p->val);
            return;
        }
    }
}

void write_kv() {
    for (KV *p = head; p; p = p->next) {
        printf("%s=%s\n", p->key, p->val);
    }
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char line[1000];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        if (strcmp(line, "END") == 0) {
            break;
        }
        char cmd[20];
        char key[200];
        char val[800];
        val[0] = 0;
        sscanf(line, "%s %s %[^\n]", cmd, key, val);
        if (strcmp(cmd, "SET") == 0) {
            set_kv(key, val);
        } else if (strcmp(cmd, "UNSET") == 0) {
            unset_kv(key);
        } else if (strcmp(cmd, "GET") == 0) {
            get_kv(key);
        } else if (strcmp(cmd, "WRITE") == 0) {
            write_kv();
        }
    }
    while (head) {
        KV *n = head->next;
        free(head->key);
        free(head->val);
        free(head);
        head = n;
    }
    return 0;
}