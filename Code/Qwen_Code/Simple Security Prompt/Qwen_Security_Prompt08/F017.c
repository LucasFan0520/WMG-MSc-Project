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

void load_file(char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        return;
    }
    char line[1000];
    while (fgets(line, 1000, f)) {
        line[strcspn(line, "\n")] = 0;
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = 0;
            char *val = eq + 1;
            if (*val == ' ') {
                val++;
            }
            KV *nk = malloc(sizeof(KV));
            nk->key = strdup(line);
            nk->val = strdup(val);
            nk->next = head;
            head = nk;
        }
    }
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        load_file(argv[1]);
    }
    char cmd[20];
    char key[100];
    char val[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "SET")) {
            scanf("%s", key);
            fgets(val, 1000, stdin);
            val[strcspn(val, "\n")] = 0;
            if (val[0] == ' ') {
                memmove(val, val + 1, strlen(val));
            }
            KV *p = head;
            while (p && strcmp(p->key, key)) {
                p = p->next;
            }
            if (p) {
                free(p->val);
                p->val = strdup(val);
            } else {
                KV *nk = malloc(sizeof(KV));
                nk->key = strdup(key);
                nk->val = strdup(val);
                nk->next = head;
                head = nk;
            }
        } else if (!strcmp(cmd, "UNSET")) {
            scanf("%s", key);
            KV *p = head;
            KV *prev = NULL;
            while (p) {
                if (!strcmp(p->key, key)) {
                    if (prev) {
                        prev->next = p->next;
                    } else {
                        head = p->next;
                    }
                    free(p->key);
                    free(p->val);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "GET")) {
            scanf("%s", key);
            KV *p = head;
            while (p && strcmp(p->key, key)) {
                p = p->next;
            }
            if (p) {
                printf("%s\n", p->val);
            }
        } else if (!strcmp(cmd, "WRITE")) {
            for (KV *p = head; p; p = p->next) {
                printf("%s=%s\n", p->key, p->val);
            }
        }
    }
    return 0;
}