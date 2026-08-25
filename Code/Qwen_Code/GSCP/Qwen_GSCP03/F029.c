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

void free_list() {
    while (head) {
        Rec *r = head; head = head->next;
        free(r->key); free(r->val); free(r);
    }
}

int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;
        char cmd[16], key[256], val[4096] = "";
        if (sscanf(line, "%15s", cmd) != 1) continue;
        if (strcmp(cmd, "END") == 0) break;

        if (strcmp(cmd, "ADD") == 0) {
            if (sscanf(line, "%*s %255s %4095[^\n]", key, val) < 1) continue;
            Rec *r = malloc(sizeof(Rec));
            r->key = strdup(key); r->val = strdup(val);
            r->next = head; head = r;
        } else if (strcmp(cmd, "DEDUP") == 0) {
            Rec *seen = NULL;
            Rec *prev = NULL, *curr = head;
            while (curr) {
                int dup = 0;
                for (Rec *s = seen; s; s = s->next) {
                    if (strcmp(s->key, curr->key) == 0) { dup = 1; break; }
                }
                if (dup) {
                    Rec *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->key); free(tmp->val); free(tmp);
                } else {
                    Rec *s = malloc(sizeof(Rec));
                    s->key = strdup(curr->key); s->val = NULL; s->next = seen; seen = s;
                    prev = curr; curr = curr->next;
                }
            }
            while (seen) { Rec *tmp = seen; seen = seen->next; free(tmp->key); free(tmp); }
        } else if (strcmp(cmd, "DELETE") == 0) {
            if (sscanf(line, "%*s %255s", key) != 1) continue;
            Rec *prev = NULL, *curr = head;
            while (curr) {
                if (strcmp(curr->key, key) == 0) {
                    Rec *tmp = curr; curr = curr->next;
                    if (prev) prev->next = curr; else head = curr;
                    free(tmp->key); free(tmp->val); free(tmp);
                } else {
                    prev = curr; curr = curr->next;
                }
            }
        } else if (strcmp(cmd, "FIND") == 0) {
            if (sscanf(line, "%*s %255s", key) != 1) continue;
            for (Rec *r = head; r; r = r->next) {
                if (strcmp(r->key, key) == 0) printf("%s\n", r->val);
            }
        } else if (strcmp(cmd, "REPORT") == 0) {
            for (Rec *r = head; r; r = r->next) {
                printf("%s %s\n", r->key, r->val);
            }
        }
    }
    free_list();
    return 0;
}