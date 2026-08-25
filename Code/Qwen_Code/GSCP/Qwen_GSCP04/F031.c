// F031.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rec { char *name; char *note; struct Rec *next; } Rec;
Rec *recs = NULL;

void cmd_batch(int n) {
    Rec *temp = NULL; int valid = 1;
    for (int i = 0; i < n; i++) {
        char line[4096];
        if (!fgets(line, sizeof(line), stdin)) { valid = 0; break; }
        line[strcspn(line, "\n")] = 0;
        char name[256], note[4096] = {0};
        if (sscanf(line, "%255s %4095[^\n]", name, note) < 1) { valid = 0; break; }
        Rec *r = malloc(sizeof(Rec)); r->name = strdup(name); r->note = strdup(note); r->next = temp; temp = r;
    }
    if (valid) {
        while (temp) { Rec *next = temp->next; temp->next = recs; recs = temp; temp = next; }
    } else {
        while (temp) { Rec *next = temp->next; free(temp->name); free(temp->note); free(temp); temp = next; }
    }
}
void cmd_delete(char *n) {
    Rec *curr = recs, *prev = NULL;
    while (curr) { if (!strcmp(curr->name, n)) { if (prev) prev->next = curr->next; else recs = curr->next; free(curr->name); free(curr->note); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_find(char *n) { for (Rec *r = recs; r; r = r->next) if (!strcmp(r->name, n)) { printf("%s %s\n", r->name, r->note); return; } }
void cmd_report() { for (Rec *r = recs; r; r = r->next) printf("%s %s\n", r->name, r->note); }
void free_all() { Rec *r = recs; while (r) { Rec *nr = r->next; free(r->name); free(r->note); free(r); r = nr; } }
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256]; int n;
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "BATCH") && sscanf(line, "%*s %d", &n) == 1) cmd_batch(n);
        else if (!strcmp(cmd, "DELETE") && sscanf(line, "%*s %255s", a1) == 1) cmd_delete(a1);
        else if (!strcmp(cmd, "FIND") && sscanf(line, "%*s %255s", a1) == 1) cmd_find(a1);
        else if (!strcmp(cmd, "REPORT")) cmd_report();
    }
    free_all(); return 0;
}