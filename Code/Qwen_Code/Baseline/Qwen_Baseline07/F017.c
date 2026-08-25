// F017.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct KV {
    char *key, *val;
    struct KV *next;
} KV;

KV *head = NULL;

void load_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return;
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        size_t l = strlen(line);
        if (l > 0 && line[l-1] == '\n') line[l-1] = '\0';
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            KV *kv = malloc(sizeof(KV));
            kv->key = strdup(line); kv->val = strdup(eq + 1); kv->next = head; head = kv;
        }
    }
    fclose(f);
}

void read_rest(char *buf) {
    if (!fgets(buf, 1024, stdin)) buf[0] = '\0';
    if (buf[0] == ' ') memmove(buf, buf + 1, strlen(buf));
    size_t l = strlen(buf);
    if (l > 0 && buf[l-1] == '\n') buf[l-1] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc > 1) load_file(argv[1]);
    char cmd[20], key[256], val[1024];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "SET") == 0) {
            scanf("%s", key); read_rest(val);
            KV *kv = head;
            while (kv && strcmp(kv->key, key) != 0) kv = kv->next;
            if (kv) { free(kv->val); kv->val = strdup(val); }
            else {
                KV *nkv = malloc(sizeof(KV));
                nkv->key = strdup(key); nkv->val = strdup(val); nkv->next = head; head = nkv;
            }
        } else if (strcmp(cmd, "UNSET") == 0) {
            scanf("%s", key);
            KV *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->key, key) == 0) {
                    if (prev) prev->next = curr->next; else head = curr->next;
                    free(curr->key); free(curr->val); free(curr); break;
                }
                prev = curr; curr = curr->next;
            }
        } else if (strcmp(cmd, "GET") == 0) {
            scanf("%s", key);
            for (KV *kv = head; kv; kv = kv->next)
                if (strcmp(kv->key, key) == 0) { printf("%s\n", kv->val); break; }
        } else if (strcmp(cmd, "WRITE") == 0) {
            for (KV *kv = head; kv; kv = kv->next) printf("%s=%s\n", kv->key, kv->val);
        }
    }
    return 0;
}