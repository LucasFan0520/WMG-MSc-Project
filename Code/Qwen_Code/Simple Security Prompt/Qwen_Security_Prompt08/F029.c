// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rec {
    char *key;
    char *val;
    struct Rec *next;
} Rec;

Rec *head = NULL;

int main() {
    char cmd[20];
    char key[100];
    char val[100];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END")) {
        if (!strcmp(cmd, "ADD")) {
            scanf("%s %s", key, val);
            Rec *r = malloc(sizeof(Rec));
            r->key = strdup(key);
            r->val = strdup(val);
            r->next = head;
            head = r;
        } else if (!strcmp(cmd, "DEDUP")) {
            Rec *p = head;
            while (p) {
                Rec *q = p;
                while (q->next) {
                    if (!strcmp(q->next->key, p->key)) {
                        Rec *tmp = q->next;
                        q->next = tmp->next;
                        free(tmp->key);
                        free(tmp->val);
                        free(tmp);
                    } else {
                        q = q->next;
                    }
                }
                p = p->next;
            }
        } else if (!strcmp(cmd, "DELETE")) {
            scanf("%s", key);
            Rec *p = head;
            Rec *prev = NULL;
            while (p) {
                if (!strcmp(p->key, key)) {
                    if (prev) prev->next = p->next;
                    else head = p->next;
                    Rec *tmp = p;
                    p = p->next;
                    free(tmp->key);
                    free(tmp->val);
                    free(tmp);
                } else {
                    prev = p;
                    p = p->next;
                }
            }
        } else if (!strcmp(cmd, "FIND")) {
            scanf("%s", key);
            for (Rec *r = head; r; r = r->next) {
                if (!strcmp(r->key, key)) {
                    printf("%s\n", r->val);
                }
            }
        } else if (!strcmp(cmd, "REPORT")) {
            for (Rec *r = head; r; r = r->next) {
                printf("%s %s\n", r->key, r->val);
            }
        }
    }
    return 0;
}