// F010.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct V {
    char *val;
    struct V *next;
} V;

typedef struct K {
    char *key;
    V *vals;
    struct K *next;
} K;

K *head = NULL;

int main() {
    char cmd[20];
    char key[100];
    char val[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s %s", key, val);
            K *k = head;
            while (k && strcmp(k->key, key)) {
                k = k->next;
            }
            if (!k) {
                k = malloc(sizeof(K));
                k->key = strdup(key);
                k->vals = NULL;
                k->next = head;
                head = k;
            }
            V *nv = malloc(sizeof(V));
            nv->val = strdup(val);
            nv->next = k->vals;
            k->vals = nv;
        } else if (!strcmp(cmd, "REMOVE")) {
            scanf("%s %s", key, val);
            K *k = head;
            while (k && strcmp(k->key, key)) {
                k = k->next;
            }
            if (k) {
                V *p = k->vals;
                V *prev = NULL;
                while (p) {
                    if (!strcmp(p->val, val)) {
                        if (prev) {
                            prev->next = p->next;
                        } else {
                            k->vals = p->next;
                        }
                        free(p->val);
                        free(p);
                        break;
                    }
                    prev = p;
                    p = p->next;
                }
            }
        } else if (!strcmp(cmd, "ERASE")) {
            scanf("%s", key);
            K *p = head;
            K *prev = NULL;
            while (p) {
                if (!strcmp(p->key, key)) {
                    if (prev) {
                        prev->next = p->next;
                    } else {
                        head = p->next;
                    }
                    V *v = p->vals;
                    while (v) {
                        V *tmp = v;
                        v = v->next;
                        free(tmp->val);
                        free(tmp);
                    }
                    free(p->key);
                    free(p);
                    break;
                }
                prev = p;
                p = p->next;
            }
        } else if (!strcmp(cmd, "VALUES")) {
            scanf("%s", key);
            K *k = head;
            while (k && strcmp(k->key, key)) {
                k = k->next;
            }
            if (k) {
                for (V *v = k->vals; v; v = v->next) {
                    printf("%s\n", v->val);
                }
            }
        } else if (!strcmp(cmd, "KEYS")) {
            for (K *k = head; k; k = k->next) {
                printf("%s\n", k->key);
            }
        }
    }
    return 0;
}