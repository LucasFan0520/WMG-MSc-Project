// F025.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Stud { char *name; char *note; struct Stud *next; } Stud;
typedef struct Cls { char *name; Stud *studs; struct Cls *next; } Cls;
Cls *clss = NULL;

Cls *find_cls(char *n) { for (Cls *c = clss; c; c = c->next) if (!strcmp(c->name, n)) return c; return NULL; }
Stud *find_stud(Cls *c, char *n) { for (Stud *s = c->studs; s; s = s->next) if (!strcmp(s->name, n)) return s; return NULL; }

void cmd_class(char *n) { if (find_cls(n)) return; Cls *c = malloc(sizeof(Cls)); c->name = strdup(n); c->studs = NULL; c->next = clss; clss = c; }
void cmd_student(char *cn, char *sn, char *n) { Cls *c = find_cls(cn); if (!c) return; Stud *s = malloc(sizeof(Stud)); s->name = strdup(sn); s->note = strdup(n); s->next = c->studs; c->studs = s; }
void cmd_transfer(char *fc, char *tc, char *sn) {
    Cls *c1 = find_cls(fc), *c2 = find_cls(tc); if (!c1 || !c2) return;
    Stud *curr = c1->studs, *prev = NULL;
    while (curr) { if (!strcmp(curr->name, sn)) { if (prev) prev->next = curr->next; else c1->studs = curr->next; curr->next = c2->studs; c2->studs = curr; return; } prev = curr; curr = curr->next; }
}
void cmd_drop(char *cn, char *sn) {
    Cls *c = find_cls(cn); if (!c) return;
    Stud *curr = c->studs, *prev = NULL;
    while (curr) { if (!strcmp(curr->name, sn)) { if (prev) prev->next = curr->next; else c->studs = curr->next; free(curr->name); free(curr->note); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_deleteclass(char *n) {
    Cls *curr = clss, *prev = NULL;
    while (curr) { if (!strcmp(curr->name, n)) { if (prev) prev->next = curr->next; else clss = curr->next; Stud *s = curr->studs; while (s) { Stud *ns = s->next; free(s->name); free(s->note); free(s); s = ns; } free(curr->name); free(curr); return; } prev = curr; curr = curr->next; }
}
void cmd_print() {
    for (Cls *c = clss; c; c = c->next) for (Stud *s = c->studs; s; s = s->next) printf("%s %s %s\n", c->name, s->name, s->note);
}
void free_all() {
    Cls *c = clss; while (c) { Cls *nc = c->next; Stud *s = c->studs; while (s) { Stud *ns = s->next; free(s->name); free(s->note); free(s); s = ns; } free(c->name); free(c); c = nc; }
}
int main() {
    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; if (!strcmp(line, "END")) break;
        char cmd[32], a1[256], a2[256], a3[4096] = {0};
        if (sscanf(line, "%31s", cmd) != 1) continue;
        if (!strcmp(cmd, "CLASS") && sscanf(line, "%*s %255s", a1) == 1) cmd_class(a1);
        else if (!strcmp(cmd, "STUDENT") && sscanf(line, "%*s %255s %255s %4095[^\n]", a1, a2, a3) >= 2) cmd_student(a1, a2, a3);
        else if (!strcmp(cmd, "TRANSFER") && sscanf(line, "%*s %255s %255s %255s", a1, a2, a3) == 3) cmd_transfer(a1, a2, a3);
        else if (!strcmp(cmd, "DROP") && sscanf(line, "%*s %255s %255s", a1, a2) == 2) cmd_drop(a1, a2);
        else if (!strcmp(cmd, "DELETECLASS") && sscanf(line, "%*s %255s", a1) == 1) cmd_deleteclass(a1);
        else if (!strcmp(cmd, "PRINT")) cmd_print();
    }
    free_all(); return 0;
}