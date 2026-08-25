// F036.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Rec { char *name; char *data; struct Rec *next; } Rec;
Rec *recs = NULL;

void cmd_add(char *name, char *arg) {
    char *colon = strchr(arg, ':'); if (!colon) return;
    int len = atoi(arg); if (len < 0 || len != (int)strlen(colon + 1)) return;
    Rec *r = malloc(sizeof(Rec)); r->name = strdup(name); r->data = strdup(colon + 1); r->next = recs; recs = r;
}
void cmd_delete(char *name) {
    Rec *curr = recs, *prev = NULL;
    while (curr) { if (!strcmp(curr->name, name)) { if (prev) prev->next = curr->next; else recs = curr->next; free(curr->name); free(curr->data); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_show(char *name) {
    Rec *r = recs; while (r) { if (!strcmp(r->name, name)) { for (char *c = r->data; *c; c++) { if (*c == ' ') putchar('_'); else putchar(*c); } putchar('\n'); return; } r = r->next; }
}
void cmd_report() { for (Rec *r = recs; r; r = r->next) printf("%s\n", r->name); }
void free_all() { Rec *r = recs; while (r) { Rec *nr = r->next; free(r->name); free(r->data); free(r); r = nr; } }
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[4096] = {0};
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "ADD") && sscanf(line, "%*s %255s %4095[^\n]", a1, a2) == 2) cmd_add(a1, a2);
        else if (!strcmp(cmd, "DELETE") && sscanf(line, "%*s %255s", a1) == 1) cmd_delete(a1);
        else if (!strcmp(cmd, "SHOW") && sscanf(line, "%*s %255s", a1) == 1) cmd_show(a1);
        else if (!strcmp(cmd, "REPORT")) cmd_report();
    }
    free_all(); return 0;
}