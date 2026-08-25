// F029.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rec { char *key; char *val; struct Rec *next; } Rec;
Rec *recs = NULL;

void cmd_add(char *k, char *v) { Rec *r = malloc(sizeof(Rec)); r->key = strdup(k); r->val = strdup(v); r->next = recs; recs = r; }
void cmd_dedup() {
    Rec *curr = recs, *prev = NULL;
    while (curr) {
        int dup = 0;
        for (Rec *c2 = recs; c2 != curr; c2 = c2->next) if (!strcmp(c2->key, curr->key)) { dup = 1; break; }
        if (dup) { if (prev) prev->next = curr->next; else recs = curr->next; free(curr->key); free(curr->val); free(curr); curr = prev ? prev->next : recs; }
        else { prev = curr; curr = curr->next; }
    }
}
void cmd_delete(char *k) {
    Rec *curr = recs, *prev = NULL;
    while (curr) { if (!strcmp(curr->key, k)) { if (prev) prev->next = curr->next; else recs = curr->next; free(curr->key); free(curr->val); free(curr); curr = prev ? prev->next : recs; } else { prev = curr; curr = curr->next; } }
}
void cmd_find(char *k) { for (Rec *r = recs; r; r = r->next) if (!strcmp(r->key, k)) printf("%s\n", r->val); }
void cmd_report() { for (Rec *r = recs; r; r = r->next) printf("%s %s\n", r->key, r->val); }
void free_all() { Rec *r = recs; while (r) { Rec *nr = r->next; free(r->key); free(r->val); free(r); r = nr; } }
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[4096] = {0};
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "ADD") && sscanf(line, "%*s %255s %4095[^\n]", a1, a2) >= 1) cmd_add(a1, a2);
        else if (!strcmp(cmd, "DEDUP")) cmd_dedup();
        else if (!strcmp(cmd, "DELETE") && sscanf(line, "%*s %255s", a1) == 1) cmd_delete(a1);
        else if (!strcmp(cmd, "FIND") && sscanf(line, "%*s %255s", a1) == 1) cmd_find(a1);
        else if (!strcmp(cmd, "REPORT")) cmd_report();
    }
    free_all(); return 0;
}