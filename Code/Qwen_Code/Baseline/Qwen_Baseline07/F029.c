// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rec {
    char *key, *value;
    struct Rec *next;
} Rec;

Rec *head = NULL;

int main() {
    char cmd[20], key[256], val[256];
    while (scanf("%s", cmd) == 1 && strcmp(cmd, "END") != 0) {
        if (strcmp(cmd, "ADD") == 0) {
            scanf("%s %s", key, val);
            Rec *r = malloc(sizeof(Rec));
            r->key = strdup(key); r->value = strdup(val); r->next = head; head = r;
        } else if (strcmp(cmd, "DEDUP") == 0) {
            Rec *curr = head;
            while (curr) {
                Rec *check = head;
                int is_first = 1;
                while (check != curr) {
                    if (strcmp(check->key, curr->key) == 0) { is_first = 0; break; }
                    check = check->next;
                }
                if (!is_first) {
                    Rec *prev = NULL, *c = head;
                    while (c != curr) { prev = c; c = c->next; }
                    if (prev) prev->next = curr->next; else head = curr->next;
                    Rec *next = curr->next;
                    free(curr->key); free(curr->value); free(curr);
                    curr = next;
                } else {
                    curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "DELETE") == 0) {
            scanf("%s", key);
            Rec *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->key, key) == 0) {
                    Rec *next = curr->next;
                    if (prev) prev->next = next; else head = next;
                    free(curr->key); free(curr->value); free(curr);
                    curr = next;
                } else {
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            scanf("%s", key);
            for (Rec *r = head; r; r = r->next) if (strcmp(r->key, key) == 0) printf("%s\n", r->value);
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Rec *r = head; r; r = r->next) printf("%s %s\n", r->key, r->value);
        }
    }
    return 0;
}