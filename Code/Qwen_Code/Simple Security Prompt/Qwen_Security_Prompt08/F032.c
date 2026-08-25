// F032.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct KV {
    char *key;
    char *val;
    struct KV *next;
} KV;

KV *main_list = NULL;
KV *trans_list = NULL;
int in_trans = 0;

void set_kv(KV **list, char *key, char *val) {
    for (KV *p = *list; p; p = p->next) {
        if (!strcmp(p->key, key)) {
            free(p->val);
            p->val = strdup(val);
            return;
        }
    }
    KV *n = malloc(sizeof(KV));
    n->key = strdup(key);
    n->val = strdup(val);
    n->next = *list;
    *list = n;
}

void free_list(KV *list) {
    while (list) {
        KV *tmp = list;
        list = list->next;
        free(tmp->key);
        free(tmp->val);
        free(tmp);
    }
}

int main() {
    char cmd[20];
    char key[100];
    char val[1000];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "SET")) {
            scanf("%s", key);
            fgets(val, 1000, stdin);
            val[strcspn(val, "\n")] = 0;
            if (val[0] == ' ') memmove(val, val + 1, strlen(val));
            set_kv(&main_list, key, val);
        } else if (!strcmp(cmd, "BEGIN")) {
            in_trans = 1;
            free_list(trans_list);
            trans_list = NULL;
        } else if (!strcmp(cmd, "TSET")) {
            scanf("%s", key);
            fgets(val, 1000, stdin);
            val[strcspn(val, "\n")] = 0;
            if (val[0] == ' ') memmove(val, val + 1, strlen(val));
            if (in_trans) set_kv(&trans_list, key, val);
        } else if (!strcmp(cmd, "COMMIT")) {
            if (in_trans) {
                for (KV *p = trans_list; p; p = p->next) {
                    set_kv(&main_list, p->key, p->val);
                }
                free_list(trans_list);
                trans_list = NULL;
                in_trans = 0;
            }
        } else if (!strcmp(cmd, "ABORT")) {
            if (in_trans) {
                free_list(trans_list);
                trans_list = NULL;
                in_trans = 0;
            }
        } else if (!strcmp(cmd, "GET")) {
            scanf("%s", key);
            for (KV *p = main_list; p; p = p->next) {
                if (!strcmp(p->key, key)) {
                    printf("%s\n", p->val);
                    break;
                }
            }
        } else if (!strcmp(cmd, "DUMP")) {
            for (KV *p = main_list; p; p = p->next) {
                printf("%s=%s\n", p->key, p->val);
            }
        }
    }
    return 0;
}